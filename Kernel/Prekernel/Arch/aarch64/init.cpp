/*
 * Copyright (c) 2021, Jesse Buhagiar <jooster660@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Types.h>

u64 __stack_chk_guard = 0xcafebabedeafbeef;
extern "C" [[noreturn]] void __stack_chk_fail();

[[noreturn]] static void halt()
{
    asm volatile("wfi"); // Put core into low power mode
    __builtin_unreachable();
}

extern "C" [[noreturn]] void __stack_chk_fail()
{
    halt();
}

namespace Kernel {

extern "C" [[noreturn]] void init();
extern "C" [[noreturn]] void init()
{
    volatile u64 x = 0;
    volatile u64 y = 1;
    for (;;) {
        y = y + 1;
        x = x * y;
    }
}

}