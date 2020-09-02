#pragma region License
// Copyright (c) 2020 julr
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma endregion

#pragma once
#include <cstdint>
#include <array>

#include "HidDevice.h"

class ArctisProWireless
{
public:
    enum class BatteryLevel : std::uint8_t
    {
        NO_BATTERY = 0x00,
        ONE_BAR    = 0x01,
        TWO_BARS   = 0x02,
        THREE_BARS = 0x03,
        FOUR_BARS  = 0x04,
        INVALID    = 0xFF
    };

    bool isHeadsetConnected();
    BatteryLevel getHeadsetBatteryLevel();
    BatteryLevel getStationBatteryLevel();

private:
    enum class RequestStatusCommand : std::uint8_t
    {
        HEADSET_BATTERY_STATUS    = 0x40,
        HEADSET_CONNECTION_STATUS = 0x41,
        STATION_BATTERY_STATUS       = 0x42
    };

    static constexpr std::uint16_t vid_ = 0x1038;
    static constexpr std::uint16_t pid_ = 0x1290; //0x1294 is also present as HID but I don't know what it is used for
    static constexpr unsigned retries_{ 5 }; //how many times can we try to read/write to the device without a fail
     
    HidDevice device_{ vid_, pid_ };

    bool readStatus(RequestStatusCommand statusCommand, std::array<std::uint8_t, 32>& statusReportOut);
};