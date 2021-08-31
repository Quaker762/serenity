/*
 * Copyright (c) 2021, Nico Weber <thakis@chromium.org>
 * Copyright (c) 2021, Jesse Buhagiar <jooster660@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#define SPSR_VALUE          (1 << 6) | (1 << 7)
#define SCTLR_EL1_VALUE     (3 << 28) | (3 << 22) | (1 << 20) | (1 << 18) | (1 << 16)
#define SCR_EL3_VALUE       (1 << 10) | (1 << 7) | (1 << 0)
#define DUMMY_VALUE1        0xDEADBEEF
#define DUMMY_VALUE2        0xCAFEBABE

.section .stack, "aw", @nobits
.align 16 /* Stack MUST be 16-byte aligned according to ARM */
stack_bottom:
.skip 32768
stack_top:

.text

.global start
.type start, @function
start:
  mrs x0, MPIDR_EL1         /* Read current processor ID into x0 */
  and x0, x0, 0xff
  cmp x0, 0
  bne Lhalt                 /* Put all other processors besides core0 to sleep */

  mrs x0, CurrentEL         /* Move current Exception Level into x0 */
  lsr x0, x0, 2
  cmp x0, 1                 /* Check to see if we're in EL1 ("Kernel Mode") */
  bne do_el1_mode_switch    /* We're not! Halt execution! */
  b el1_entry               /* We're in EL1 already, go to real entry point */

/**
 * Exception Level 1 Entry (EL1 Entry)
 *
 * "Real" entry point of the boot image. At this point, we guarantee that the
 * processor running this code is running in Exception Level 1.
 */
.global el1_entry_aarch64
.type el1_entry, @function
el1_entry:
  adr x0, stack_top
  mov sp, x0
  bl Lhalt

/**
 * EL1 mode switch
 *
 * This puts the procesor int Exception Level 1 (EL1) by  setting up
 * some registers (namely the Exception Level Link Register) and the
 * executing an `eret` instruction
 * NOTE: We assume that if the processor hasn't put us in EL1, then we're most
 * likely in EL3...
 */
do_el1_mode_switch:
    ldr x0, =SPSR_VALUE
    msr SPSR_EL3, x0
    ldr x0, =SCTLR_EL1_VALUE
    msr SCTLR_EL1, x0
    ldr x0, =SCR_EL3_VALUE
    msr SCR_EL3, x0

    adr x0, el1_entry           /* Load address of el1_entry into x0 */
    msr ELR_EL3, x0
    eret                        /* Make the jump to EL1 */

/**
 * Put the processor into low power mode and spin
 */
Lhalt:
  wfi
  b Lhalt
