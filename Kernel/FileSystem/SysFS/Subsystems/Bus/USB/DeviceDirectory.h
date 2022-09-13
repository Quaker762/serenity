/*
 * Copyright (c) 2022, Jesse Buhagiar <jesse.buhagiar@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Bus/USB/USBDevice.h>
#include <Kernel/FileSystem/SysFS.h>

namespace Kernel {

class SysFSUSBDeviceDirectory final : public SysFSDirectory {
public:
    static ErrorOr<NonnullLockRefPtr<SysFSUSBDeviceDirectory>> create(SysFSDirectory const&, USB::Device const&);

    virtual StringView name() const override { return m_device_directory_name->view(); }

private:
    SysFSUSBDeviceDirectory(NonnullOwnPtr<KString> device_directory_name, SysFSDirectory const&, USB::Device const&);
    LockRefPtr<USB::Device> m_device;
    NonnullOwnPtr<KString> m_device_directory_name;
};

}
