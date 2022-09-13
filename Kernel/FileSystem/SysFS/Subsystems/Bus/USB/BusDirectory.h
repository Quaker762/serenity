/*
 * Copyright (c) 2022, Jesse Buhagiar <jesse.buhagiar@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/FileSystem/SysFS/Component.h>
#include <Kernel/FileSystem/SysFS/Subsystems/Bus/USB/DeviceDirectory.h>

namespace Kernel {

class SysFSUSBBusDirectory final : public SysFSDirectory {
public:
    static NonnullLockRefPtr<SysFSUSBBusDirectory> create(SysFSDirectory const&, USB::USBController const&);

    virtual StringView name() const override { return m_device_directory_name->view(); }

    USB::USBController const& controller() const;

    void plug(Badge<USB::Device>, SysFSUSBDeviceDirectory&);
    void unplug(Badge<USB::Device>, SysFSUSBDeviceDirectory&);
    void plug(Badge<USB::Hub>, SysFSUSBDeviceDirectory&);
    void unplug(Badge<USB::Hub>, SysFSUSBDeviceDirectory&);

private:
    SysFSUSBBusDirectory(NonnullOwnPtr<KString> device_directory_name, SysFSDirectory const&, USB::USBController const&);
    LockRefPtr<USB::USBController> m_controller;
    NonnullOwnPtr<KString> m_device_directory_name;
};

}
