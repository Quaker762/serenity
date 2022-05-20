/*
* Copyright (c) 2022, Jesse Buhagiar <jesse.buhagiar@serenityos.org>
*
* SPDX-License-Identifier: BSD-2-Clause
*/

#pragma once

#include <AK/Error.h>
#include <AK/NonnullRefPtr.h>
#include <Kernel/Bus/USB/USBDevice.h>

namespace Kernel::USB {

typedef void (*driver_init_function_t)();
#define USB_DEVICE_DRIVER(driver_name) \
    driver_init_function_t driver_init_function_ptr_##driver_name __attribute__((section(".driver_init"), unused)) = &driver_name::init

#define USB_DEVICE_TABLE_IDENTIFIER(vid, pid) { vid, pid },

class Driver : public RefCounted<Driver> {
public:
    struct DeviceId {
        u16 vendor_id;
        u16 product_id;

        bool operator==(DeviceId const& rhs) const { return ((rhs.vendor_id == vendor_id) && (rhs.product_id == product_id)); }
    };

    Driver(StringView const& name)
        : m_driver_name(name)
    {
    }
    
    virtual ~Driver() = default;

    virtual ErrorOr<void> probe(USBInterface const& interface) = 0;
    virtual ErrorOr<size_t> read(size_t count, u8* data) = 0;
    virtual ErrorOr<size_t> write(size_t count, u8 const* data) = 0;
    virtual void disconnect() = 0;
    virtual StringView const& name() const = 0;

    Vector<DeviceId> const& device_table() const { return m_supported_device_table; }

protected:
    StringView const m_driver_name;
    Vector<DeviceId> m_supported_device_table;
};
}
