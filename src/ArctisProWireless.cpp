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

#include <stdexcept>
#include <algorithm>

#include "ArctisProWireless.h"

bool ArctisProWireless::isHeadsetConnected()
{
    std::array<std::uint8_t, 32> readData;
    if (readStatus(RequestStatusCommand::HEADSET_CONNECTION_STATUS, readData))
    {
        return (readData.at(0) == 0x04);
    }

    throw std::runtime_error("Unable to read connection status");
}

ArctisProWireless::BatteryLevel ArctisProWireless::getHeadsetBatteryLevel()
{
    std::array<std::uint8_t, 32> readData;
    if (readStatus(RequestStatusCommand::HEADSET_BATTERY_STATUS, readData))
    {
        return static_cast<BatteryLevel>(readData.at(0));
    }

    throw std::runtime_error("Unable to read headset battery level");
}

ArctisProWireless::BatteryLevel ArctisProWireless::getStationBatteryLevel()
{
    std::array<std::uint8_t, 32> readData;
    if (readStatus(RequestStatusCommand::STATION_BATTERY_STATUS, readData))
    {
        return static_cast<BatteryLevel>(readData.at(0));
    }

    throw std::runtime_error("Unable to read station battery level");
}

bool ArctisProWireless::readStatus(RequestStatusCommand statusCommand, std::array<std::uint8_t, 32>& statusReportOut)
{
    //fist byte is the report id, rest is the report data
    const std::uint8_t command[33]{ 0x00, static_cast<std::uint8_t>(statusCommand), 0xAA };
    std::uint8_t dataIn[33]{ 0 };

    unsigned retryCounter{ 0 };
    for (retryCounter = 0; retryCounter < retries_; retryCounter++)
    {
        if (device_.write(command, sizeof(command)))
        {
            break;
        }
    }
    if (retryCounter >= retries_)
    {
        return false;
    }

    for (retryCounter = 0; retryCounter < retries_; retryCounter++)
    {
        if (device_.read(dataIn, sizeof(dataIn)))
        {
            break;
        }
    }
    if (retryCounter >= retries_)
    {
        return false;
    }

    std::copy_n(std::begin(dataIn) + 1, 32, std::begin(statusReportOut));
    return true;
}