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
#include <Kernel/Devices/USB/USBPipe.h>

namespace Kernel::USB
{

OwnPtr<USBPipe> USBPipe::create_pipe(Type type, Direction direction, u16 max_packet_size)
{

}

OwnPtr<USBPipe> USBPipe::create_pipe(Type type, Direction direction, u8 device_address, u8 endpoint_address, u16 max_packet_size, u8 poll_interval)
{

}

USBPipe::USBPipe(Type type, USBPipe::Direction direction, u16 max_packet_size)
    : m_type(type),
      m_direction(direction),
      m_device_address(0),
      m_endpoint_address(0),
      m_max_packet_size(max_packet_size),
      m_poll_interval(0)
{

}

USBPipe::USBPipe(Type type, Direction direction, endpoint_descriptor& endpoint [[maybe_unused]])
    : m_type(type),
      m_direction(direction)
{
    // TODO: decode endpoint structure
}

USBPipe::USBPipe(Type type, Direction direction, u8 device_address, u8 endpoint_address, u16 max_packet_size, u8 poll_interval)
    : m_type(type),
      m_direction(direction),
      m_device_address(device_address),
      m_endpoint_address(endpoint_address),
      m_max_packet_size(max_packet_size),
      m_poll_interval(poll_interval)
{

}

}
