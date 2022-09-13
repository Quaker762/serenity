/*
 * Copyright (c) 2022, Jesse Buhagiar <jesse.buhagiar@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/USB/USBController.h>
#include <Kernel/FileSystem/SysFS/Subsystems/Bus/USB/BusDirectory.h>

namespace Kernel {

USB::USBController const& SysFSUSBBusDirectory::controller() const
{
    return *m_controller;
}

void SysFSUSBBusDirectory::plug(Badge<USB::Device>, SysFSUSBDeviceDirectory& new_device_directory)
{
    MUST(m_child_components.with([&](auto& list) -> ErrorOr<void> {
        list.append(new_device_directory);
        auto pointed_component_base_name = MUST(KString::try_create(new_device_directory.name()));
        auto pointed_component_relative_path = MUST(new_device_directory.relative_path(move(pointed_component_base_name), 0));
        return {};
    }));
}

void SysFSUSBBusDirectory::unplug(Badge<USB::Device>, SysFSUSBDeviceDirectory& removed_device_directory)
{
    MUST(m_child_components.with([&](auto& list) -> ErrorOr<void> {
        list.remove(removed_device_directory);
        return {};
    }));
}

void SysFSUSBBusDirectory::plug(Badge<USB::Hub>, SysFSUSBDeviceDirectory& new_device_directory)
{
    MUST(m_child_components.with([&](auto& list) -> ErrorOr<void> {
        list.append(new_device_directory);
        auto pointed_component_base_name = MUST(KString::try_create(new_device_directory.name()));
        auto pointed_component_relative_path = MUST(new_device_directory.relative_path(move(pointed_component_base_name), 0));
        return {};
    }));
}

void SysFSUSBBusDirectory::unplug(Badge<USB::Hub>, SysFSUSBDeviceDirectory& removed_device_directory)
{
    MUST(m_child_components.with([&](auto& list) -> ErrorOr<void> {
        list.remove(removed_device_directory);
        return {};
    }));
}

UNMAP_AFTER_INIT NonnullLockRefPtr<SysFSUSBBusDirectory> SysFSUSBBusDirectory::create(SysFSDirectory const& parent_directory, USB::USBController const& controller)
{
    auto bus_directory_name = MUST(KString::formatted("usb{}", controller.bus_number()));
    return adopt_lock_ref(*new (nothrow) SysFSUSBBusDirectory(move(bus_directory_name), parent_directory, controller));
}

UNMAP_AFTER_INIT SysFSUSBBusDirectory::SysFSUSBBusDirectory(NonnullOwnPtr<KString> device_directory_name, SysFSDirectory const& parent_directory, USB::USBController const& controller)
    : SysFSDirectory(parent_directory)
    , m_controller(controller)
    , m_device_directory_name(move(device_directory_name))
{
}
}
