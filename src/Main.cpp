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

#include <string>
#include <cwctype>
#include <algorithm>

#include <Windows.h>

#include "Application.h"

Application application;

BOOL WINAPI consoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT)
    {
        application.stop();
    }
    return TRUE;
}

int WINAPI wWinMain(_In_ HINSTANCE /* hInstance */, _In_opt_ HINSTANCE /* hPrevInstance */, _In_ LPWSTR lpCmdLine, _In_ int /* nShowCmd */)
{
    Application::RunMode mode;

    std::wstring cmdLine{ lpCmdLine };
    std::transform(cmdLine.begin(), cmdLine.end(), cmdLine.begin(), std::towupper);

    if (cmdLine.empty())
    {
        mode = Application::RunMode::NORMAL;
        SetConsoleCtrlHandler(consoleHandler, TRUE);
    }
    else if (cmdLine == L"LIST")
    {
        mode = Application::RunMode::LIST_DEVICES;
    }
    else
    {
        mode = Application::RunMode::PRINT_USAGE;
    }

    return application.run(mode) ? 0 : 1;
}