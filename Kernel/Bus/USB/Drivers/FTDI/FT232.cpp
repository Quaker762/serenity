//
// Created by quaker on 17/5/22.
//

#include <Kernel/Bus/USB/Drivers/FTDI/FT232.h>
#include <Kernel/Bus/USB/USBManagement.h>

namespace Kernel::USB
{

static constexpr u16 FTDI_VENDOR_ID     = 0x0403u;
static constexpr u16 FTDI232BM_PID      = 0x6001u;

static AK::Array<Driver::DeviceId, 1> const s_support_device_list = {{
    USB_DEVICE_TABLE_IDENTIFIER(FTDI_VENDOR_ID, FTDI232BM_PID)
}};

void FT232::init(void)
{
    auto driver = adopt_nonnull_ref_or_enomem(new (nothrow) FT232());
    if (driver.is_error())
    {
        dbgln("FT232: Failed to create driver!");
        return;
    }

    USBManagement::the().register_driver(driver.release_value());
}

FT232::FT232() : Driver("FTDI FT232BM USB UART"sv)
{
    for (auto const& device_id : s_support_device_list)
        m_supported_device_table.append(device_id);
}

FT232::~FT232()
{

}

ErrorOr<void> FT232::probe(USBInterface const& interface)
{
    dbgln("Probing for FTDI USB UART...");
    auto& device [[maybe_unused]] = interface.configuration().device(); // Get the device from the
    return {};
}

ErrorOr<size_t> FT232::read(size_t, u8*)
{
    return { 0 };
}

ErrorOr<size_t> FT232::write(size_t, u8 const*)
{
    return { 0 };
}

void FT232::disconnect()
{

}

USB_DEVICE_DRIVER(FT232);

}