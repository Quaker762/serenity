.set MULTIBOOT_MAGIC,         0x1badb002
.set MULTIBOOT_PAGE_ALIGN,    0x1
.set MULTIBOOT_MEMORY_INFO,   0x2
.set MULTIBOOT_VIDEO_MODE,    0x4
.set multiboot_flags,         MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE
.set multiboot_checksum,      -(MULTIBOOT_MAGIC + multiboot_flags)

.section .multiboot
.align 4

.long MULTIBOOT_MAGIC
.long multiboot_flags
.long multiboot_checksum


/* for MULTIBOOT_MEMORY_INFO */
.long 0x00000000    /* header_addr */
.long 0x00000000    /* load_addr */
.long 0x00000000    /* load_end_addr */
.long 0x00000000    /* bss_end_addr */
.long 0x00000000    /* entry_addr */

/* for MULTIBOOT_VIDEO_MODE */
.long 0x00000000    /* mode_type */
.long 1280          /* width */
.long 1024          /* height */
.long 32            /* depth */

.section .stack, "aw", @nobits
stack_bottom:
.skip 32768
stack_top:

.section .page_tables
.align 4096
page_tables_start:
.skip 4096*3

# These are the paging structures we initially create to map the kernel to the 
# 3GiB mark. This allows us to actually call the kernel's 'init' function correctly,
# otherwise we'd jump to 0xc0000000, where there's no .text!
boot_page_dir:
.skip 4096
boot_page_table1:
.skip 4096
boot_page_table2:
.skip 4096

.section .text.boot

.global start
.type start, @function

.extern init 
.type init, @function

.extern multiboot_info_ptr
.type multiboot_info_ptr, @object

start:
    cli
    cld

    # We first save the multiboot_info_ptr so it doesn't get trampled
    addl $0xc0000000, %ebx
    movl %ebx, multiboot_info_ptr - 0xc0000000

    # First, let's set up the first page table to map the the first 4MiB of memory.
    movl $0x0, %eax # eax is the page counter
    movl $0x0, %ebx # ebx is the base pointer (kernel base is at physical address 0 in this case)
    movl $0x0, %ecx
    movl $(boot_page_table1 - 0xc0000000), %edx

    .make_table1:
        movl %ebx, %ecx
        orl $0x3, %ecx # addr | READ_WRITE | PAGE_PRESENT
        movl %ecx, 0(%edx, %eax, 4)
        addl $0x1000, %ebx
        inc %eax
        cmp $0x400, %eax
        jne .make_table1

    movl $0x0, %eax # eax is the page counter
    movl $0x400000, %ebx # ebx is the base pointer (kernel base is at physical address 0 in this case)
    movl $0x0, %ecx
    movl $(boot_page_table2 - 0xc0000000), %edx

    .make_table2:
        movl %ebx, %ecx
        orl $0x3, %ecx # addr | READ_WRITE | PAGE_PRESENT
        movl %ecx, 0(%edx, %eax, 4)
        addl $0x1000, %ebx
        inc %eax
        cmp $0x400, %eax
        jne .make_table2
        
    
    # Okay, so we have a page table that contains addresses of the first 4MiB of memory. Let's insert this into the 
    # boot page directory. The index we need to insert it into is at vaddr >> 22, which is the page directory index.
    # This reveals that we need to insert the page directory into 0xc0000000 >> 22 = 768
    # An interesting quirk is that we must also identity map the first 4MiB too, as the next instruction after enabling
    # paging is at a physical address, which cause a page fault. As we have no handler, this would cause a triple fault.
    movl $(boot_page_table1 - 0xc0000000 + 0x003), boot_page_dir - 0xc0000000 + 0
    movl $(boot_page_table1 - 0xc0000000 + 0x003), boot_page_dir - 0xc0000000 + 768 * 4
    movl $(boot_page_table2 - 0xc0000000 + 0x003), boot_page_dir - 0xc0000000 + 4
    movl $(boot_page_table2 - 0xc0000000 + 0x003), boot_page_dir - 0xc0000000 + 769 * 4

    # Now let's load the CR3 register with our page directory
    movl $(boot_page_dir - 0xc0000000), %ecx
    movl %ecx, %cr3

    # Let's enable paging!
    movl %cr0, %ecx
    orl $0x80000001, %ecx
	movl %ecx, %cr0 

    lea higher_half_start, %ecx
    jmp *%ecx

# At this point, the CPU now starts reading instructions from (virtual) address 0xc00100000 
higher_half_start:
    # movl $0x0, boot_page_dir + 0x0000
    # movl $0x0, boot_page_dir + 0x0004
    # movl %cr3, %ecx
    # movl %ecx, %cr3

    mov $stack_top, %esp

    and $-16, %esp

    pushl $page_tables_start
    call init
    add $4, %esp

    pushl $exit_message
    call kprintf
    add $4, %esp

    cli

loop:
    hlt
    jmp loop

exit_message:
    .asciz "Kernel exited."
