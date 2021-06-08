/*
 * Copyright (c) 2021, Jesse Buhagiar <jooster669@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/OwnPtr.h>
#include <AK/Types.h>
#include <AK/Vector.h>
#include <Kernel/Devices/USB/UHCIController.h>
#include <Kernel/Devices/USB/USBDescriptors.h>
#include <Kernel/Devices/USB/USBDevice.h>
#include <Kernel/Devices/USB/USBRequest.h>

static u32 s_next_usb_address = 1; // Next address we hand out to a device once it's plugged into the machine

namespace Kernel::USB {

static Array<USBDevice*, 2> s_devices;

void USBDevice::create(PortNumber port, DeviceSpeed speed)
{
    auto device = new USBDevice(port, speed);
    device->enumerate();
    s_devices.at(static_cast<u8>(port)) = device;
}

USBDevice* USBDevice::get(PortNumber port)
{
    VERIFY(port <= PortNumber::Port2);
    return s_devices.at(static_cast<u8>(port));
}

const USBDevice* USBDevice::get_device_from_id(u8 device_address)
{
    for (auto* const device : s_devices) {
        if (!device)
            continue;

        if (device->address() == device_address)
            return device;
    }

    return nullptr;
}

USBDevice::USBDevice(PortNumber port, DeviceSpeed speed)
    : m_device_port(port)
    , m_device_speed(speed)
    , m_address(0)
{
    // FIXME: How should we propagate the error from `create_pipe` here in the constructor???
    m_default_pipe = USBPipe::create_pipe(USBPipe::Type::Control, USBPipe::Direction::In, 0, 8, 0);
}

void USBDevice::enumerate()
{
    USBDeviceDescriptor dev_descriptor;

    // FIXME: 0x100 is a magic number for now, as I'm not quite sure how these are constructed....
    // Send 8-bytes to get at least the `max_packet_size` from the device
    u32 transfer_length = m_default_pipe->control_transfer(USB_DEVICE_REQUEST_DEVICE_TO_HOST, USB_REQUEST_GET_DESCRIPTOR, 0x100, 0, 8, &dev_descriptor);

    // FIXME: This shouldn't crash! Do some correct error handling on me please!
    VERIFY(transfer_length > 0);

    // Ensure that this is actually a valid device descriptor...
    VERIFY(dev_descriptor.descriptor_header.descriptor_type == DESCRIPTOR_TYPE_DEVICE);
    m_default_pipe->set_max_packet_size(dev_descriptor.max_packet_size);

    transfer_length = m_default_pipe->control_transfer(USB_DEVICE_REQUEST_DEVICE_TO_HOST, USB_REQUEST_GET_DESCRIPTOR, 0x100, 0, sizeof(USBDeviceDescriptor), &dev_descriptor);

    // FIXME: This shouldn't crash! Do some correct error handling on me please!
    VERIFY(transfer_length > 0);

    // Ensure that this is actually a valid device descriptor...
    VERIFY(dev_descriptor.descriptor_header.descriptor_type == DESCRIPTOR_TYPE_DEVICE);

    if constexpr (USB_DEBUG) {
        dbgln("USB Device Descriptor for {:04x}:{:04x}", dev_descriptor.vendor_id, dev_descriptor.product_id);
        dbgln("Device Class: {:02x}", dev_descriptor.device_class);
        dbgln("Device Sub-Class: {:02x}", dev_descriptor.device_sub_class);
        dbgln("Device Protocol: {:02x}", dev_descriptor.device_protocol);
        dbgln("Max Packet Size: {:02x} bytes", dev_descriptor.max_packet_size);
        dbgln("Number of configurations: {:02x}", dev_descriptor.num_configurations);
    }

    // Attempt to set devices address on the bus
    m_default_pipe->control_transfer(USB_DEVICE_REQUEST_HOST_TO_DEVICE, USB_REQUEST_SET_ADDRESS, s_next_usb_address, 0, 0, nullptr);
    VERIFY(transfer_length > 0);
    m_address = s_next_usb_address++;

    // Copy the device descriptor
    memcpy(&m_device_descriptor, &dev_descriptor, sizeof(USBDeviceDescriptor));
}

USBDevice::~USBDevice()
{
}

}
