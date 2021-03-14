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
#include <Kernel/VM/Region.h>
#include <Kernel/Devices/USB/USBDescriptors.h>

namespace Kernel::USB
{

//
// A pipe is the logical connection between a memory buffer on the PC (host) and
// an endpoint on the device.
class USBPipe
{
public:
    enum class Type : u8
    {
        Control = 0,
        Isochronous = 1,
        Bulk = 2,
        Interrupt = 3
    };

    enum class Direction : u8
    {
        Out = 0,
        In = 1
    };

public:
    static OwnPtr<USBPipe> create_pipe(Type type, Direction direction, u16 max_packet_size);
    static OwnPtr<USBPipe> create_pipe(Type type, Direction direction, endpoint_descriptor& endpoint);
    static OwnPtr<USBPipe> create_pipe(Type type, Direction direction, u8 device_address, u8 endpoint_address, u16 max_packet_size, u8 poll_interval);

    u8 type() const
    {
        switch(m_type)
        {
        case Type::Control:
            return 0;
        case Type::Isochronous:
            return 1;
        case Type::Bulk:
            return 2;
        case Type::Interrupt:
            return 3;
        }
    }

    u8 direction() const
    {
        switch(m_direction)
        {
        case Direction::Out:
            return 0;
        case Direction::In:
            return 1;
        }
    }

    void set_device_address(u8 address) { m_device_address = address; }

    u8 device_address() const { return m_device_address; }
    u8 endpoint_address() const { return m_endpoint_address; }
    u16 max_packet_size() const { return m_max_packet_size; }
    u8 poll_interval() const { return m_poll_interval; }

private:
    USBPipe(Type type, Direction direction, u16 max_packet_size);
    USBPipe(Type type, Direction direction, endpoint_descriptor& endpoint);
    USBPipe(Type type, Direction direction, u8 device_address, u8 endpoint_address, u16 max_packet_size, u8 poll_interval);

private:
    friend class USBDevice;

    Type m_type;
    Direction m_direction;

    u8 m_device_address;    // Device address for this pipe
    u8 m_endpoint_address;  // Corresponding endpoint address for this pipe
    u16 m_max_packet_size;  // Max packet size for this pipe
    u8 m_poll_interval;     // Polling interval (in frames)
};

}
