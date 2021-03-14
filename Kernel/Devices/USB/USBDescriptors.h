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

namespace Kernel::USB
{

struct [[gnu::packed]] descriptor_common
{
    u8 length;
    u8 descriptor_type;
};

//
//  Device Descriptor
//  =================
//
//  This descriptor type (stored on the device), represents the device, and gives
//  information related to it, such as the USB specification it complies to,
//  as well as the vendor and product ID of the device.
//
//  https://beyondlogic.org/usbnutshell/usb5.shtml#DeviceDescriptors
struct [[gnu::packed]] device_descriptor
{
    descriptor_common descriptor_header;
    u16 usb_spec_compliance_bcd;
    u8 device_class;
    u8 device_sub_class;
    u8 device_protocol;
    u8 endpoint_max_packet_size;
    u8 vendor_id;
    u8 product_id;
    u8 device_release_bcd;
    u8 manufacturer_id_descriptor_index;
    u8 product_string_descriptor_index;
    u8 serial_number_descriptor_index;
    u8 num_configurations;
};

//
//  Configuration Descriptor
//  ========================
//
//  A USB device can have multiple configurations, which tells us about how the
//  device is physically configured (e.g how it's powered, max power consumption etc).
//
struct [[gnu::packed]] configuration_descriptor
{
    descriptor_common descriptor_header;
    u16 total_length;
    u8 number_of_interfaces;
    u8 configuration_value;
    u8 configuration_string_descriptor_index;
    u8 attributes_bitmap;
    u8 max_power_in_ma;
};

//
//  Interface Descriptor
//  ====================
//
//  An interface descriptor describes to us one or more endpoints, grouped
//  together to define a singular function of a device.
//  As an example, a USB webcam might have two interface descriptors; one
//  for the camera, and one for the microphone.
//
struct [[gnu::packed]] interface_descriptor
{
    descriptor_common descriptor_header;
    u8 interface_id;
    u8 alternate_setting;
    u8 number_of_endpoints;
    u8 interface_class_code;
    u8 interface_sub_class_code;
    u8 interface_protocol;
    u8 interface_string_descriptor_index;
};

//
//  Endpoint Descriptor
//  ===================
//
//  The lowest leaf in the configuration tree. And endpoint descriptor describes
//  the physical transfer properties of the endpoint (that isn't endpoint0).
//  The description given by this structure is used by a pipe to create a
//  "connection" from the host to the device.
//  https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/usb-endpoints-and-their-pipes
struct [[gnu::packed]] endpoint_descriptor
{
    descriptor_common descriptor_header;
    u8 endpoint_address;
    u8 endpoint_attributes_bitmap;
    u16 max_packet_size;
    u8 poll_interval_in_frames;
};

static constexpr u8 DESCRIPTOR_TYPE_DEVICE = 0x01;
static constexpr u8 DESCRIPTOR_TYPE_CONFIGURATION = 0x02;
static constexpr u8 DESCRIPTOR_TYPE_STRING = 0x03;
static constexpr u8 DESCRIPTOR_TYPE_INTERFACE = 0x04;
static constexpr u8 DESCRIPTOR_TYPE_ENDPOINT = 0x05;
static constexpr u8 DESCRIPTOR_TYPE_DEVICE_QUALIFIER = 0x06;

}
