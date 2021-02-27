/*
 * Copyright (c) 2021, Jesse Buhagiar <jooster669@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Devices/USB/PacketTypes.h>
#include <Kernel/Devices/USB/UHCIController.h>
#include <Kernel/Devices/USB/USBPipe.h>
#include <Kernel/Devices/USB/USBTransfer.h>

namespace Kernel::USB {

OwnPtr<USBPipe> USBPipe::create_pipe(Type type, Direction direction, u8 endpoint_address, u16 max_packet_size, i8 device_address, u8 poll_interval)
{
    auto pipe = adopt_own_if_nonnull(new USBPipe(type, direction, endpoint_address, max_packet_size, device_address, poll_interval));
    if (!pipe)
        return {};

    return pipe;
}

USBPipe::USBPipe(Type type, USBPipe::Direction direction, u16 max_packet_size)
    : m_type(type)
    , m_direction(direction)
    , m_endpoint_address(0)
    , m_max_packet_size(max_packet_size)
    , m_poll_interval(0)
    , m_data_toggle(false)
{
}

USBPipe::USBPipe(Type type, Direction direction, USBEndpointDescriptor& endpoint [[maybe_unused]])
    : m_type(type)
    , m_direction(direction)
{
    // TODO: decode endpoint structure
}

USBPipe::USBPipe(Type type, Direction direction, u8 endpoint_address, u16 max_packet_size, u8 poll_interval, i8 device_address)
    : m_type(type)
    , m_direction(direction)
    , m_device_address(device_address)
    , m_endpoint_address(endpoint_address)
    , m_max_packet_size(max_packet_size)
    , m_poll_interval(poll_interval)
    , m_data_toggle(false)
{
}

size_t USBPipe::control_transfer(u8 request_type, u8 request, u16 value, u16 index, u16 length, void* data)
{
    USBRequestData usb_request;

    usb_request.request_type = request_type;
    usb_request.request = request;
    usb_request.value = value;
    usb_request.index = index;
    usb_request.length = length;

    auto transfer = USBTransfer::create(*this, length);

    // Transfer creation failed (probably OOM condition...)
    if (!transfer)
        return 0;

    transfer->set_setup_packet(usb_request);

    dbgln_if(USB_DEBUG, "USBPipe: USBTransfer allocated @ {:08x}", transfer->buffer_physical());
    auto transfer_len = UHCIController::the().submit_control_transfer(*transfer);

    // TODO: Check transfer for completion and copy data from transfer buffer into data
    if (length > 0)
        memcpy(reinterpret_cast<u8*>(data), transfer->buffer().as_ptr() + sizeof(USBRequestData), length);

    dbgln_if(USB_DEBUG, "USBPipe: Control Transfer complete!");
    return transfer_len;
}

}
