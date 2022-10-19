/*
 * Copyright (c) 2022, Jesse Buhagiar <jooster669@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/USB/USBController.h>
#include <Kernel/Bus/USB/USBEndpoint.h>

namespace Kernel::USB {

ErrorOr<NonnullOwnPtr<USBEndpoint>> USBEndpoint::create(USB::Device const& device, USBEndpointDescriptor const& descriptor)
{
    USB::Pipe::Type type;
    USB::Pipe::Direction direction;

    switch (descriptor.endpoint_attributes_bitmap & ENDPOINT_ATTRIBUTES_TRANSFER_TYPE_MASK) {
    case ENDPOINT_ATTRIBUTES_TRANSFER_TYPE_CONTROL:
        type = USB::Pipe::Type::Control;
        break;
    case ENDPOINT_ATTRIBUTES_TRANSFER_TYPE_ISOCHRONOUS:
        type = USB::Pipe::Type::Isochronous;
        break;
    case ENDPOINT_ATTRIBUTES_TRANSFER_TYPE_BULK:
        type = USB::Pipe::Type::Bulk;
        break;
    case ENDPOINT_ATTRIBUTES_TRANSFER_TYPE_INTERRUPT:
        type = USB::Pipe::Type::Interrupt;
        break;
    default:
        __builtin_unreachable(); // Give Mr. Compiler a hand; this situation is impossible
        break;
    }

    if (type == USB::Pipe::Type::Control) {
        direction = USB::Pipe::Direction::Bidirectional;
    } else {
        switch (descriptor.endpoint_address & ENDPOINT_ADDRESS_DIRECTION_MASK) {
        case ENDPOINT_ADDRESS_DIRECTION_IN:
            direction = USB::Pipe::Direction::In;
            break;
        case ENDPOINT_ADDRESS_DIRECTION_OUT:
            direction = USB::Pipe::Direction::Out;
            break;
        default:
            __builtin_unreachable(); // Give Mr. Compiler a hand; this situation is impossible
            break;
        }
    }

    auto pipe = TRY(USB::Pipe::try_create_pipe(device.controller(), type, direction, descriptor.endpoint_address, descriptor.max_packet_size, device.address(), descriptor.poll_interval_in_frames));
    auto endpoint = TRY(adopt_nonnull_own_or_enomem(new USBEndpoint(move(pipe), descriptor)));

    return endpoint;
}

USBEndpoint::USBEndpoint(NonnullOwnPtr<USB::Pipe> pipe, Kernel::USB::USBEndpointDescriptor const& descriptor)
    : m_descriptor(descriptor)
    , m_pipe(move(pipe))
{
}

ErrorOr<size_t> USBEndpoint::write(size_t count, u8 const* const data)
{
    ErrorOr<size_t> result = { 0 };
    switch (m_pipe->type()) {
    case USB::Pipe::Type::Bulk:
        result = m_pipe->bulk_transfer(count, const_cast<u8*>(data));
        break;
    case USB::Pipe::Type::Interrupt:
    case USB::Pipe::Type::Control:
    case USB::Pipe::Type::Isochronous:
    default:
        result = ENODEV;
        break;
    }

    return result;
}

ErrorOr<size_t> USBEndpoint::read(size_t count, u8* const data)
{
    ErrorOr<size_t> result = { 0 };
    switch (m_pipe->type()) {
    case USB::Pipe::Type::Bulk:
        result = m_pipe->bulk_transfer(count, const_cast<u8*>(data));
        break;
    case USB::Pipe::Type::Interrupt:
    case USB::Pipe::Type::Control:
    case USB::Pipe::Type::Isochronous:
    default:
        result = ENODEV;
        break;
    }

    return result;
}

}
