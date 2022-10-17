/*
 * Copyright (c) 2021, Leon Albrecht <leon2002.la@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>

#define RREGISTER(num)                         \
    union {                                    \
        u64 r##num;                            \
        struct {                               \
            u32 _unused##num;                  \
            union {                            \
                u32 r##num##d;                 \
                struct {                       \
                    u16 __unused##num;         \
                    union {                    \
                        u16 r##num##w;         \
                        struct {               \
                            u8 ___unused##num; \
                            u8 r##num##b;      \
                        };                     \
                    };                         \
                };                             \
            };                                 \
        };                                     \
    }

#define GPREGISTER(letter)                \
    union {                               \
        u64 r##letter##x;                 \
        struct                            \
        {                                 \
            u32 _unused##letter;          \
            union {                       \
                u32 e##letter##x;         \
                struct                    \
                {                         \
                    u16 __unused##letter; \
                    union {               \
                        u16 letter##x;    \
                        struct {          \
                            u8 letter##h; \
                            u8 letter##l; \
                        };                \
                    };                    \
                };                        \
            };                            \
        };                                \
    }

#define SPREGISTER(name)                        \
    union {                                     \
        u64 r##name;                            \
        struct                                  \
        {                                       \
            u32 _unused##name;                  \
            union {                             \
                u32 e##name;                    \
                struct                          \
                {                               \
                    u16 __unused##name;         \
                    union {                     \
                        u16 name;               \
                        struct {                \
                            u8 ___unused##name; \
                            u8 name##l;         \
                        };                      \
                    };                          \
                };                              \
            };                                  \
        };                                      \
    }

struct [[gnu::packed]] PtraceRegisters {
    RREGISTER(0);
    RREGISTER(1);
    RREGISTER(2);
    RREGISTER(3);
    RREGISTER(4);
    RREGISTER(5);
    RREGISTER(6);
    RREGISTER(7);
    RREGISTER(8);
    RREGISTER(9);
    RREGISTER(10);
    RREGISTER(11);
    RREGISTER(12);
    RREGISTER(13);
    RREGISTER(14);
    RREGISTER(15);
    RREGISTER(16);
    RREGISTER(17);
    RREGISTER(18);
    RREGISTER(19);
    RREGISTER(20);
    RREGISTER(21);
    RREGISTER(23);
    RREGISTER(24);
    RREGISTER(25);
    RREGISTER(26);
    RREGISTER(27);
    RREGISTER(28);
    RREGISTER(29);
    RREGISTER(30);
    RREGISTER(31);

    SPREGISTER(spsr_el1);
    SPREGISTER(elr_el1);
    SPREGISTER(tpidr_el1);
    SPREGISTER(sp_el0);

    // FIXME: Add FPU registers and Flags
    // FIXME: Add Ymm Xmm etc.
};
