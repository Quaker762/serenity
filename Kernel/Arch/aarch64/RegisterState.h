/*
 * Copyright (c) 2018-2021, James Mintram <me@jamesrm.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

namespace Kernel {

struct RegisterState {
    FlatPtr userspace_sp() const { return 0; }
    FlatPtr ip() const { return 0; }
    void set_userspace_sp(FlatPtr value [[maybe_unused]]){}
    void set_ip(FlatPtr value [[maybe_unused]]){}
};

struct DebugRegisterState {
};

}
