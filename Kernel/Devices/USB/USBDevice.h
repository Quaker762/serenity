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

#pragma once

#include <AK/OwnPtr.h>
#include <AK/Types.h>
#include <Kernel/Devices/USB/USBPipe.h>

namespace Kernel::USB
{

//
// Some nice info from FTDI on device enumeration and how some of this
// glues together:
//
// https://www.ftdichip.com/Support/Documents/TechnicalNotes/TN_113_Simplified%20Description%20of%20USB%20Device%20Enumeration.pdf
class USBDevice
{
public:
    enum class PortNumber : u8
    {
        Port1 = 0,
        Port2
    };

    enum class DeviceSpeed : u8
    {
        FullSpeed = 0,
        LowSpeed
    };

public:
    static void create_usb_device(PortNumber port, DeviceSpeed speed);
    static USBDevice* get_device(PortNumber port);

    USBDevice(PortNumber port, DeviceSpeed speed);
    ~USBDevice();

    void initialize_device();

    PortNumber port() const { return m_device_port; }
    DeviceSpeed speed() const { return m_device_speed; }

    u8 address() const { return m_address; }

private:

private:
    PortNumber m_device_port;       // What port is this device attached to
    DeviceSpeed m_device_speed;     // What speed is this device running at
    u8 m_address;                   // USB address assigned to this device

    // Device description
    u16 m_vendor_id;                // This device's vendor ID assigned by the USB group
    u16 m_product_id;               // This device's product ID assigned by the USB group

    OwnPtr<USBPipe> m_default_pipe; // Default communication pipe (endpoint0) used during enumeration
};

}
