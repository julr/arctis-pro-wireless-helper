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


#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <thread>
#include <chrono>

#include <Windows.h>

#include "AudioEndpoints.h"
#include "Configuration.h"
#include "ArctisProWireless.h"
#include "Application.h"


bool Application::run(RunMode mode)
{
    bool error = false;
    try
    {
        AudioEndpoints endpoints;

        switch (mode)
        {
        case(RunMode::NORMAL):
        {
            Configuration config;
            if (!endpoints.isEndpointAvailable(config.getDeviceGuidConnected()) || !endpoints.isEndpointAvailable(config.getDeviceGuidDisconnected()))
            {
                outputText(L"Configured audio device not found", true);
                error = true;
            }
            else
            {
                ArctisProWireless headset;
                mainLoop(config, endpoints, headset);
            }
            break;
        }
 
        case(RunMode::LIST_DEVICES):
        {
            std::wstringstream devices;
            devices << L"Available Devices:";
            for (auto& device : endpoints.getEndpoints())
            {
                devices << std::endl << std::endl << device.Name << std::endl << device.EndpointGuid;
            }
            outputText(devices.str());
            break;
        }

        case(RunMode::PRINT_USAGE):
            outputText(L"Available Parameters:\n(none)\trun the application\nlist\tlist available devices and exit");
            break;

        default:
            outputText(L"Internal Error: Invalid mode", true);
            error = true;
            break;
        }
    }
    catch (std::exception& e)
    {
        const std::wstring errorWstring = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(e.what());
        outputText(errorWstring, true);
        error = true;
    }
    if (isConsoleAvailable_)
    {
        outputText(L"\nPress ENTER to return to the console");
    }
    return !error;
}

void Application::mainLoop(Configuration& config, AudioEndpoints& endpoints, ArctisProWireless& headset)
{
    const AudioEndpoints::AudioEndpoint* endpointConnected = endpoints.getEndpoint(config.getDeviceGuidConnected());
    const AudioEndpoints::AudioEndpoint* endpointDisconnected = endpoints.getEndpoint(config.getDeviceGuidDisconnected());

    //make sure to switch to the correct device on startup
    bool lastConnectionState = !headset.isHeadsetConnected();

    while (isRunning_)
    {
        bool connectionState = headset.isHeadsetConnected();
        if (connectionState != lastConnectionState)
        {
            endpoints.setActiveEnpoint((connectionState == true) ? *endpointConnected : *endpointDisconnected);
        }
        lastConnectionState = connectionState;

        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
}

bool Application::enableConsoleOutput()
{
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        const HANDLE handleStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        const HANDLE handleStdErr = GetStdHandle(STD_ERROR_HANDLE);

        if ((handleStdOut != INVALID_HANDLE_VALUE) && (handleStdErr != INVALID_HANDLE_VALUE))
        {
            FILE* dummy;
            freopen_s(&dummy, "CONOUT$", "w", stdout);
            freopen_s(&dummy, "CONOUT$", "w", stderr);
            setvbuf(stdout, NULL, _IONBF, 0);
            setvbuf(stderr, NULL, _IONBF, 0);

            std::cout << std::endl;
            return true;
        }
    }
    return false;
}

//Output via Messagebox or Console depending on availability
void Application::outputText(const std::wstring& text, bool isError)
{
    if (isConsoleAvailable_)
    {
        ((isError) ? std::wcerr : std::wcout) << text << std::endl;      
    }
    else
    {
        MessageBox(nullptr, text.c_str(), isError ? L"Auto Audio Switcher - ERROR" : L"Auto Audio Switcher", isError ? MB_ICONERROR : MB_OK);
    }
}
