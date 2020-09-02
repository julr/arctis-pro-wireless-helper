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

// Based on
// http://eretik.omegahg.com/download/PolicyConfig.h
// http://social.microsoft.com/Forums/en/Offtopic/thread/9ebd7ad6-a460-4a28-9de9-2af63fd4a13e

#include<string>
#include<vector>

class AudioEndpoints
{
public:
    struct AudioEndpoint
    {
        std::wstring DeviceId{};
        std::wstring Name{};
        std::wstring EndpointGuid{};
    };

    AudioEndpoints();

    bool setActiveEnpoint(const AudioEndpoint& endpoint) const;

    const AudioEndpoint* getEndpoint(const std::wstring& endpointGuid) const;

    const std::vector<AudioEndpoint> getEndpoints() const
    {
        return endpoints_;
    }

    bool isEndpointAvailable(const std::wstring& endpointGuid) const
    {
        return getEndpoint(endpointGuid) != nullptr;
    }
private:
    std::vector<AudioEndpoint> endpoints_{};    
};