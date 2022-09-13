/*
 * Copyright (c) 2021, Liav A. <liavalb@hotmail.co.il>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/FileSystem/SysFS/Registry.h>
#include <Kernel/FileSystem/SysFS/Subsystems/Bus/USB/Directory.h>
#include <Kernel/KBufferBuilder.h>

namespace Kernel {

static SysFSUSBDirectory* s_sysfs_usb_bus_directory;

void SysFSUSBDirectory::plug(Badge<USB::USBController>, SysFSUSBBusDirectory& bus_directory_node)
{
    MUST(m_child_components.with([&](auto& list) -> ErrorOr<void> {
        list.append(bus_directory_node);
        return {};
    }));
}
void SysFSUSBDirectory::unplug(Badge<USB::USBController>, SysFSUSBBusDirectory& bus_directory_node)
{
    MUST(m_child_components.with([&](auto& list) -> ErrorOr<void> {
        list.remove(bus_directory_node);
        return {};
    }));
}

SysFSUSBDirectory& SysFSUSBDirectory::the()
{
    VERIFY(s_sysfs_usb_bus_directory);
    return *s_sysfs_usb_bus_directory;
}

UNMAP_AFTER_INIT SysFSUSBDirectory::SysFSUSBDirectory(SysFSBusDirectory& buses_directory)
    : SysFSDirectory(buses_directory)
{
}

UNMAP_AFTER_INIT void SysFSUSBDirectory::initialize()
{
    auto directory = adopt_lock_ref(*new SysFSUSBDirectory(SysFSComponentRegistry::the().buses_directory()));
    SysFSComponentRegistry::the().register_new_bus_directory(directory);
    s_sysfs_usb_bus_directory = directory;
}

}
