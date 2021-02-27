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

#include <AK/Types.h>

//
// https://doc.lagout.org/electronics/usb/usb_spec.pdf, page 207
// This gives _somewhat_ of a description of what this layer of the USB stack
// should do.
//
// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-endpoints-and-their-pipes
// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-concepts-for-all-developers
// This write up by some NT Kernel devs is also really helpful explaining how
// this works at a fundamental level (Endpoints and Pipes)
//
namespace Kernel::USB
{

enum RequestTypeBitmap
{
    // Transfer Direction
    HostToDevice = (0 << 7),
    DeviceToHost = (1 << 7),

    // Type
    Standard = (0 << 6),
    Class = (1 << 6),
    Vendor = (2 << 6),
    Reserved = (3 << 6),

    // Receipient
    Device = (0 << 4),
    Interface = (1 << 4),
    Endpoint = (2 << 4),
    Other = (3 << 4)
};

enum DeviceRequest
{
    GET_STATUS = 0x00,
    CLEAR_FEATURE = 0x01,
    SET_FEATURE = 0x03,
    SET_ADDRESS = 0x05,
    GET_DESCRIPTOR = 0x06,
    SET_DESCRIPTOR = 0x07,
    GET_CONFIGURATION = 0x08,
    SET_CONFIGURATION = 0x09
};

struct SetupPacket
{
    u8 type;
    u8 request;
    u16 value;
    u16 index;
    u16 length;
};

class USBTransfer final
{
public:
    USBTransfer(SetupPacket packet, void* data, u16 length)
        : m_request_packet(packet),
          m_data_ptr(data),
          m_length(length),
          m_complete(false),
          m_successful(false)
    {

    }

    SetupPacket& request_packet() { return m_request_packet; }

    bool successful() const { return m_successful; }
    bool complete() const { return m_complete; }

    void* data() const { return m_data_ptr; }
    u16 length() const { return m_length; }

    void set_complete() { m_complete = true; }
    void set_successful_complete()
    {
        m_complete = true;
        m_successful = true;
    }

private:
    SetupPacket m_request_packet;
    void* m_data_ptr;
    u16 m_length;
    bool m_complete;
    bool m_successful;
};

}
