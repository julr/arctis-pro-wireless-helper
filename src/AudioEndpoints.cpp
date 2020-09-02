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

#include <initguid.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <PropIdl.h>
#include <functiondiscoverykeys_devpkey.h>

#include "AudioEndpoints.h"

// taken from PolicyConfig.h by EreTIk (for Windows 7 and Later)
interface DECLSPEC_UUID("f8679f50-850a-41cf-9c72-430f290290c8") IPolicyConfig;
class DECLSPEC_UUID("870af99c-171d-4f9e-af0d-e63df40c2bc9") CPolicyConfigClient;
interface IPolicyConfig : public IUnknown
{
public:
    virtual HRESULT GetMixFormat(PCWSTR, WAVEFORMATEX**);
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, WAVEFORMATEX**);
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR);
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*, WAVEFORMATEX*);
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64, PINT64);
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64);
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, struct DeviceShareMode*);
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, struct DeviceShareMode*);
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*);
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*);
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(__in PCWSTR wszDeviceId, __in ERole eRole);
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT);
};

AudioEndpoints::AudioEndpoints()
{
	HRESULT result = CoInitialize(nullptr);
	if (SUCCEEDED(result))
	{
		IMMDeviceEnumerator* deviceEnumatraotor{ nullptr };
		result = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&deviceEnumatraotor));
		if (SUCCEEDED(result))
		{
			IMMDeviceCollection* deviceCollection;
			result = deviceEnumatraotor->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
			if (SUCCEEDED(result))
			{
				UINT count;
				result = deviceCollection->GetCount(&count);
				if (SUCCEEDED(result))
				{
					for (UINT i = 0; i < count; i++)
					{
						IMMDevice* device;
						result = deviceCollection->Item(i, &device);
						if (SUCCEEDED(result))
						{
							LPWSTR deviceId{ nullptr };
							result = device->GetId(&deviceId);
							if (SUCCEEDED(result))
							{
								IPropertyStore* propertyStore;
								result = device->OpenPropertyStore(STGM_READ, &propertyStore);
								if (SUCCEEDED(result))
								{
									PROPVARIANT friendlyName;
									PropVariantInit(&friendlyName);
									result = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
									if (SUCCEEDED(result))
									{
										PROPVARIANT deviceGuid;
										PropVariantInit(&deviceGuid);
										result = propertyStore->GetValue(PKEY_AudioEndpoint_GUID, &deviceGuid);
										if (SUCCEEDED(result))
										{
											endpoints_.emplace_back(AudioEndpoint{ deviceId, friendlyName.pwszVal, deviceGuid.pwszVal });
											PropVariantClear(&deviceGuid);
										}
										PropVariantClear(&friendlyName);
									}
									propertyStore->Release();
								}
							}
							device->Release();
						}
					}
				}
				deviceCollection->Release();
			}
			deviceEnumatraotor->Release();
		}
	}

	if (FAILED(result))
	{
		throw std::runtime_error("Unable to read station battery level");
	}
}

bool AudioEndpoints::setActiveEnpoint(const AudioEndpoint& endpoint) const
{
	IPolicyConfig* policyConfig{ nullptr };
    HRESULT result = CoCreateInstance(__uuidof(CPolicyConfigClient), nullptr, CLSCTX_ALL, __uuidof(IPolicyConfig), reinterpret_cast<void**>(&policyConfig));
    if (SUCCEEDED(result))
    {
        result = policyConfig->SetDefaultEndpoint(endpoint.DeviceId.c_str(), eConsole);
        policyConfig->Release();
    }
    return SUCCEEDED(result);
}

const AudioEndpoints::AudioEndpoint* AudioEndpoints::getEndpoint(const std::wstring& endpointGuid) const
{
	for (auto& endpoint : endpoints_)
	{
		if (endpoint.EndpointGuid == endpointGuid)
		{
			return &endpoint;
		}
	}
	return nullptr;
}
