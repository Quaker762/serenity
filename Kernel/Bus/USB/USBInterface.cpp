/*
 * Copyright (c) 2022, Jesse Buhagiar <jesse.buhagiar@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Bus/USB/USBInterface.h>

namespace Kernel::USB {

ErrorOr<void> USBInterface::open()
{
    for (auto const& interface_descriptor : m_endpoint_descriptors) {
        auto endpoint = TRY(USBEndpoint::create(m_configuration.device(), interface_descriptor));
        m_endpoints.set(interface_descriptor.endpoint_address, move(endpoint));
    }

    return {};
}

void USBInterface::close()
{
    m_endpoints.clear();
}

ErrorOr<size_t> USBInterface::write_endpoint(u8 endpoint_address, size_t count, u8 const* const data)
{
    auto const& endpoint = m_endpoints.get(endpoint_address);
    if (!endpoint.has_value()) {
        dbgln("USBInterface: Couldn't find endpoint {} to write to!", endpoint_address);
        return ENODEV;
    }

    return endpoint.value()->write(count, data);
}

ErrorOr<size_t> USBInterface::read_endpoint(u8 endpoint_address, size_t count, u8* const data)
{
    auto const& endpoint = m_endpoints.get(endpoint_address);
    if (!endpoint.has_value()) {
        dbgln("USBInterface: Couldn't find endpoint {} to read from!", endpoint_address);
        return ENODEV;
    }

    return endpoint.value()->read(count, data);
}
}
