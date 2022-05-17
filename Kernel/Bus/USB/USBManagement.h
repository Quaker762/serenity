/*
 * Copyright (c) 2021, Luke Wilde <lukew@serenityos.org>
 * Copyright (c) 2022, Jesse Buhagiar <jesse.buhagiar@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/USB/USBController.h>
#include <Kernel/Library/NonnullLockRefPtr.h>
#include <Kernel/Library/NonnullLockRefPtrVector.h>
#include <AK/NonnullRefPtrVector.h>
#include <Kernel/Bus/USB/Drivers/USBDriver.h>

namespace Kernel::USB {

class USBManagement {

public:
    USBManagement();
    static bool initialized();
    static void initialize();
    static USBManagement& the();

    void register_driver(NonnullRefPtr<Driver> driver);
    void unregister_driver(NonnullRefPtr<Driver> driver);

    NonnullRefPtrVector<Driver>& available_drivers() { return m_available_drivers; }

private:
    void enumerate_controllers();

    USBController::List m_controllers;
    NonnullRefPtrVector<Driver> m_available_drivers;
};

}
