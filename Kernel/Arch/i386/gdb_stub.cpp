//
// Serenity gdb stub for the i386 platform
//
// Author: Jesse Buhagiar
// Reference:
//      http://www.logix.cz/michal/doc/i386/chp12-00.htm (a really handy guide!)
//      https://ftp.gnu.org/old-gnu/Manuals/gdb-5.1.1/html_node/gdb_129.html#SEC134
//

#include <AK/String.h>
#include <AK/StringBuilder.h>
#include <Kernel/Arch/i386/CPU.h>
#include <Kernel/IO.h>

#define INT_BREAKPOINT      0x3
#define SERIAL_COM2_ADDR    0x2F8

#define BUFFMAX             2048 // Max buffer size for tx/rx
#define I386_NUM_REGS       16

#define SIGTRAP             5
#define SIGINT              2

int     registers[I386_NUM_REGS];
char    buff[BUFFMAX];
char    hex[17] = "0123456789abcdef";

// These are just offsets into 'registers'
// Roughly the same order as PUSHAD
enum Registers
{
    // General Registers
    EAX,
    ECX,
    EDX,
    EBX,

    // Base registers
    ESP,
    EBP,

    // Source/Destination register
    ESI,
    EDI,

    // Special registers
    EIP,
    EFLAGS,

    // Segment registers
    CS,
    SS,
    DS,
    ES,
    FS,
    GS
};

// Apparently GDB stores segment registers as as a dword?! What!?
#define SAVE_REGISTERS() \
    __asm__("movl %eax, registers"); \
    __asm__("movl %ecx, registers + 4"); \
    __asm__("movl %edx, registers + 8"); \
    __asm__("movl %ebx, registers + 12"); \
    __asm__("movl %esi, registers + 24"); \
    __asm__("movl %edi, registers + 28"); \

    
// Saves EIP, ESP, CS EFLAGS and SS
#define SAVE_SPECIAL_REGISTERS() \
    // Let's get EIP first
    __asm__("popl %ebx"); \
    __asm__("movl %ebx, registers + 32"); \
    // Now let's get CS
    __asm__("popl %ebx"); \
    __asm__("movl %ebx, registers + 38"); \
    // EFLAGS
    __asm__("popl %ebx"); \
    __asm__("movl %ebx, registers + 34"); \
    // Old ESP
    __asm__("popl %ebx"); \
    __asm__("movl %ebx, registers + 16"); \
    // Old SS
    __asm__("popl %ebx"); \
    __asm__("movl %ebx, registers + 42");

static inline bool _serial_rx_ready()
{
    return IO::in8(SERIAL_COM2_ADDR + 5) & 0x1;
}

static inline bool _serial_tx_ready()
{
    return IO::in8(SERIAL_COM2_ADDR + 5) & 0x20;
}

static void _stub_putchar(int c)
{
    while(!_serial_tx_ready());

    return IO::out8(SERIAL_COM2_ADDR, (c & 0xff));
}

static int _stub_getchar()
{
    while(!_serial_rx_ready());

    return IO::in8(SERIAL_COM2_ADDR);
}

static char ascii2char(char c)
{
    if(c >= 'a' && c <= 'f')
        return c - 'a';
    else if(c >= '0' && c <= '9')
        return c - '0';
    else if(c >= 'A' && c <= 'F')
        return c - 'A';

    return -1;
}

void _stub_init_com2(int baud)
{
    IO::out8(SERIAL_COM2_ADDR + 0x1, 0x00); // Disable interrupts first

    // Now let's set up the baud rate
    IO::out8(SERIAL_COM2_ADDR + 3, IO::in8(SERIAL_COM2_ADDR + 3) | 0x80); // turn on DLAB
    IO::out8(SERIAL_COM2_ADDR + 0, ((char)(baud)) >> 2); // lower half of divisor
    IO::out8(SERIAL_COM2_ADDR + 1, ((char)(baud)) & 0xff); // upper half of divisor
    IO::out8(SERIAL_COM2_ADDR + 3, IO::in8(SERIAL_COM2_ADDR + 3) & 0x7f); // turn off DLAB

    IO::out8(SERIAL_COM2_ADDR + 3, 0x03); // Set up the line control. I can't find the GDB docs for this, so let's assume it's 8-N-1
    IO::out8(SERIAL_COM2_ADDR + 2, 0xC7); // Enable FIFO, clear with 14-byte threshold
    IO::out8(SERIAL_COM2_ADDR + 4, 0x0B); // IRQ enable, RTS/DSR set
}

static void sendpacket()
{
    char ch;
    unsigned char checksum = 0;
    int count = 0;

    _stub_putchar('$');

    while((ch = buff[count++]))
    {
        _stub_putchar(ch);
        checksum += ch;
    }

    _stub_putchar('#');
    _stub_putchar(hex[checksum >> 4]);
    _stub_putchar(hex[checksum & 0x0f]);

    while(_stub_getchar() != '+');
}

// Turn an integer into an AK::String
//
// This currently always returns a Big Endian number,
// so you'll need to set the endianess of GDB.
static void _stub_itoa(unsigned int num, String& str)
{
    StringBuilder builder;
    do
    {
        builder.append(hex[num & 0x0f]);
    } while((num >>= 4) > 0);

    str = builder.to_string(); 
    str.reverse();
}

static void handle_int3()
{
    char            ch;
    unsigned char   checksum_recv;
    unsigned char   checksum = 0;

    // Send T[SIGTRAP]
    String          sendbuff = "";
    String          eip = "";
    String          esp = "";
    StringBuilder   builder;

    builder.append("T05");
    _stub_itoa(registers[EIP], eip);

    builder.append(eip);
    sendbuff = builder.to_string();
    kprintf("%s\n", sendbuff.characters());
    sendpacket();

    while(1) {
        // Let's get the first character that GDB sent us.
        // This blocks!
        ch = _stub_getchar();

        if(ch == '$')
        {
            // Read the whole packet
            int count = 0;

            while(count < BUFFMAX - 1)
            {
                ch = _stub_getchar();

                if(ch == '#')
                {
                    ch = _stub_getchar();
                    checksum_recv = ascii2char(ch) << 4;
                    ch = _stub_getchar();
                    checksum_recv |= ascii2char(ch);

                    if(checksum_recv != checksum)
                    {
                        _stub_putchar('-');
                    }
                    else
                    {
                        _stub_putchar('+');
                    }
                }
            }
        }
    }
}

/**
 * Interrupt 3 handler
 *
 * This function is jumped to when INT $3 is fired. The value in CS:IP points to
 * the byte following the breakpoint.
 */
void _stub_int3_handler()
{
    SAVE_REGISTERS();
    SAVE_SPECIAL_REGISTERS();
    handle_int3();
}

/**
 * Set up the debug traps that allow GDB to take control of the current
 * flow of execution. This routine inserts functions into the IDT via
 * the function 'register_interrupt_handler()' found in CPU.h
 */
void _stub_set_debug_traps()
{
    memset(registers, 0, sizeof(int) * I386_NUM_REGS);
    register_interrupt_handler(INT_BREAKPOINT, _stub_int3_handler);
}

inline void breakpoint() __attribute__((always_inline));
inline void breakpoint()
{
    __asm__("int $3");
}