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
#include <Kernel/Devices/USB/USBTransfer.h>

namespace Kernel::USB
{

//
// NOTE: The USB Spec refers to what this class describes as a "pipe".
// For our own sanity, we'll just refer to it in more apt terminology..
//
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
    u8 max_packet_size() const { return m_max_packet_size; }

private:
    bool submit_request(u8 type, u8 request, u16 value, u16 index, u16 length, void* data);

private:
    PortNumber m_device_port;       // What port is this device attached to
    DeviceSpeed m_device_speed;     // What speed is this device running at
    u8 m_address;                   // USB address assigned to this device
    u8 m_max_packet_size;           // Maximum size of packets (in bytes) supported by this device

};

}
