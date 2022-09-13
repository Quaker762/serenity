/*
 * Copyright (c) 2021, Luke Wilde <lukew@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/USB/USBController.h>
#include <Kernel/FileSystem/SysFS/Subsystems/Bus/USB/Directory.h>

namespace Kernel::USB {

u8 USBController::allocate_address()
{
    // FIXME: This can be smarter.
    return m_next_device_index++;
}

void USBController::assign_bus_number()
{
    static u8 s_current_bus_number = 1u;
    m_bus_number = s_current_bus_number++;
}

u8 USBController::bus_number() const
{
    return m_bus_number;
}

SysFSUSBBusDirectory& USBController::bus_sysfs_directory() const
{
    return *m_sysfs_bus_directory;
}

void USBController::create_sysfs_directory_for_bus()
{
    auto sysfs_bus_directory = SysFSUSBBusDirectory::create(SysFSUSBDirectory::the(), *this);
    m_sysfs_bus_directory = sysfs_bus_directory;
    SysFSUSBDirectory::the().plug({}, *sysfs_bus_directory);
}
}
