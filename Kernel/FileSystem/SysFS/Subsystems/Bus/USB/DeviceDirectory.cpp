/*
 * Copyright (c) 2022, Jesse Buhagiar <jesse.buhagiar@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/USB/USBController.h>
#include <Kernel/Bus/USB/USBDevice.h>
#include <Kernel/FileSystem/SysFS/Subsystems/Bus/USB/DeviceDirectory.h>

namespace Kernel {

ErrorOr<NonnullLockRefPtr<SysFSUSBDeviceDirectory>> SysFSUSBDeviceDirectory::create(SysFSDirectory const& parent_directory, USB::Device const& usb_device)
{
    auto device_name = MUST(KString::formatted("{}-{}", usb_device.controller().bus_number(), usb_device.port()));
    auto device_directory = adopt_nonnull_lock_ref_or_enomem(new (nothrow) SysFSUSBDeviceDirectory(move(device_name), parent_directory, usb_device));

    // TODO: Fill device_directory up here
    return device_directory;
}

SysFSUSBDeviceDirectory::SysFSUSBDeviceDirectory(NonnullOwnPtr<KString> device_directory_name, SysFSDirectory const& parent_directory, USB::Device const& usb_device)
    : SysFSDirectory(parent_directory)
    , m_device(usb_device)
    , m_device_directory_name(move(device_directory_name))
{
}

}