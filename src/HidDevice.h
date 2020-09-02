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

#include <Windows.h>

class HidDevice
{
public:
    HidDevice(std::uint16_t vid, std::uint16_t pid, std::uint16_t usage = 0, std::uint16_t usagePage = 0);
    ~HidDevice();

    //first byte of data to write is the report id so the buffer should be one byte bigger than the actual payload
    bool write(const std::uint8_t* data, size_t length, std::uint32_t timeout = 100);
    //first byte of data that is read is the report id so the buffer should be one byte bigger than the actual payload
    bool read(std::uint8_t* dataOut, size_t length, std::uint32_t timeout = 100);

private:
    HANDLE deviceHandle_{ INVALID_HANDLE_VALUE };
    HANDLE readEventHandle_{ INVALID_HANDLE_VALUE };
    HANDLE writeEventHandle_{ INVALID_HANDLE_VALUE };
};