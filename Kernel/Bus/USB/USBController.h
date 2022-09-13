/*
 * Copyright (c) 2021, Luke Wilde <lukew@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/AtomicRefCounted.h>
#include <AK/Error.h>
#include <Kernel/Bus/USB/USBDevice.h>
#include <Kernel/Bus/USB/USBTransfer.h>
#include <Kernel/FileSystem/SysFS/Subsystems/Bus/USB/BusDirectory.h>

namespace Kernel::USB {

class USBController : public AtomicRefCounted<USBController> {
public:
    virtual ~USBController() = default;

    virtual ErrorOr<void> initialize() = 0;

    virtual ErrorOr<void> reset() = 0;
    virtual ErrorOr<void> stop() = 0;
    virtual ErrorOr<void> start() = 0;

    virtual ErrorOr<size_t> submit_control_transfer(Transfer&) = 0;
    virtual ErrorOr<size_t> submit_bulk_transfer(Transfer& transfer) = 0;

    u8 allocate_address();
    u8 bus_number() const;

    SysFSUSBBusDirectory& bus_sysfs_directory() const;

protected:
    void assign_bus_number();
    void create_sysfs_directory_for_bus();

    mutable LockRefPtr<SysFSUSBBusDirectory> m_sysfs_bus_directory;

private:
    u8 m_next_device_index { 1 };
    u8 m_bus_number { 0u };

    IntrusiveListNode<USBController, NonnullLockRefPtr<USBController>> m_controller_list_node;

public:
    using List = IntrusiveList<&USBController::m_controller_list_node>;
};

}
