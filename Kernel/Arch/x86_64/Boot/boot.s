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


.section .text

.global start
.type start, @function

/*
    At this point, the system is in protected mode, with paging disabled. According to the AMD
    x86_64 manual, Volume 2, Chapter 14, we need the following things before we can even think
    of switching to Long Mode.
        - IDT with 64-bit interrupt gate descriptors
        - GDT with a 64-bit Code-Segment descriptor for code segments executed in long mode, and a 64-bit data descriptor
        - A Single 64-bit TSS for holding DPL0,1 and 2 stack pointers and interrupt stack table pointers
        - Level 4 page table that contains a list of pointers to PDPTs (used in PAE in the i686 version)
    
    Once this minimum level of setup has been achieved, we can switch to long mode by performing
    the following actions:
        1. Paging MUST be disabled if it is enabled. We keep it disabled (CR0.PG = 0)
        2. Enable the PAE bit of the CR4 register
        3. Load CR3 with the Level 4 Page Table address
        4. Enable Long Mode via EFER.LME = 1
        5. Enable paging (CR0.PG = 1)

    The processor will then perform a consistency check to make sure everything is okay with the
    setup we have. If not, the processor will throw #GP (General Protection Fault), and the 
    system will halt. 

    The procedure above will look VERY similar to the following: https://wiki.osdev.org/Entering_Long_Mode_Directly
    This is considered "Entering Long Mode directly", but that's a lie. This is the steps above with hardcoded values.
    Assuming we reload everything in `init()` and `init_stage2()` in `init.cpp`, then it is fine to do this.
 */
start:
    cli
    cld