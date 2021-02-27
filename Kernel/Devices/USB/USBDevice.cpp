/*
 * Copyright (c) 2021, Jesse Buhagiar <jooster669@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AK/OwnPtr.h>
#include <AK/Types.h>
#include <AK/Vector.h>
#include <Kernel/Devices/USB/UHCIController.h>
#include <Kernel/Devices/USB/USBDescriptors.h>
#include <Kernel/Devices/USB/USBDevice.h>

static u32 s_next_usb_address [[maybe_unused]] = 1;                           // Next address we hand out to a device once it's plugged into the machine

namespace Kernel::USB {


void USBDevice::create_usb_device(PortNumber port, DeviceSpeed speed)
{
    USBDevice test_device(port, speed);
    test_device.initialize_device();
}

USBDevice::USBDevice(PortNumber port, DeviceSpeed speed)
    : m_device_port(port)
    , m_device_speed(speed)
    , m_address(0)
    , m_max_packet_size(0)
{
}

USBDevice::~USBDevice()
{

}

//
// USB Device Enumeration
//
void USBDevice::initialize_device()
{
    struct DeviceDescriptor device_descriptor;

    // At this point, the user has plugged a device into the USB port. The `UHCIHotplug` Kernel Process as detected this,
    // and created a new USB device (us). We first begin by either reading 8 bytes (if the device is Low-Speed), or 64 bytes
    // if the device is Full-Speed (USB 2.0 compliant).
    //
    // This initial read returns to us the top level Device Descriptor, which tells us some basic information about
    // the device that was just plugged into the machine.

    dbgln("USBDevice: Initializing device...");

    // First, let's determine how many bytes we actually have to read...
    switch(m_device_speed)
    {
    case DeviceSpeed::LowSpeed:
        m_max_packet_size = 8;
        break;
    case DeviceSpeed::FullSpeed:
        m_max_packet_size = 64;
        break;
    default:
        break;
    }

    (void)submit_request(RequestTypeBitmap::DeviceToHost | RequestTypeBitmap::Standard | RequestTypeBitmap::Device,
        DeviceRequest::GET_DESCRIPTOR, DescriptorType::DeviceDescriptor << 8, 0, m_max_packet_size, &device_descriptor);
}

bool USBDevice::submit_request(u8 type, u8 request, u16 value, u16 index, u16 length, void* data)
{
    SetupPacket packet { type, request, value, index, length };
    USBTransfer transfer(packet, data, length);

    dbgln("USBDevice: Submitting request to UHCI Controller...");
    UHCIController::the().control_transfer(*this, transfer);

    return false;
}

}
