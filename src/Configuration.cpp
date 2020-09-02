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

#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "ShLwApi.Lib")

#include "Configuration.h"

Configuration::Configuration()
{
    WCHAR temp[MAX_PATH];
    GetModuleFileName(NULL, temp, MAX_PATH);
    PathRemoveFileSpec(temp);

    std::wstring configPath{ temp };
    configPath += L"\\";
    configPath += configurationFile_;

    DWORD result = GetPrivateProfileString(L"AutoSwitch", L"DeviceConnected", nullptr, temp, MAX_PATH, configPath.c_str());
    if (result != 0)
    {
        deviceGuidConnected_ = std::wstring{ temp };
        result = GetPrivateProfileString(L"AutoSwitch", L"DeviceDisconnected", nullptr, temp, MAX_PATH, configPath.c_str());
    }
    if (result != 0)
    {
        deviceGuidDisconnected_ = std::wstring{ temp };
    }
    else
    {
        throw std::runtime_error("Unable to read configuration");
    }
}
