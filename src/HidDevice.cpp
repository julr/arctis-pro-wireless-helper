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

#include "HidDevice.h"
#include <SetupAPI.h>
#include <Hidsdi.h>
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "hid.lib")


HidDevice::HidDevice(std::uint16_t vid, std::uint16_t pid, std::uint16_t usage, std::uint16_t usagePage)
{
	GUID guid;	
	HidD_GetHidGuid(&guid);

	HDEVINFO deviceInformation = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (deviceInformation == INVALID_HANDLE_VALUE)
	{
		throw std::runtime_error("Unable to get device information");
	}

	for (DWORD index = 0; true; index++)
	{
		SP_DEVICE_INTERFACE_DATA interfaceData;
		interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		bool result = SetupDiEnumDeviceInterfaces(deviceInformation, NULL, &guid, index, &interfaceData);
		if (!result)
		{
			throw std::runtime_error("Device not found");
		}

		DWORD requiredSize;
		SetupDiGetInterfaceDeviceDetail(deviceInformation, &interfaceData, NULL, 0, &requiredSize, NULL);
		SP_DEVICE_INTERFACE_DETAIL_DATA* interfaceDetails = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(requiredSize);
		if (interfaceDetails == nullptr)
		{
			throw std::runtime_error("Memory allocation failed");
		}
		ZeroMemory(interfaceDetails, requiredSize);
		interfaceDetails->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		result = SetupDiGetDeviceInterfaceDetail(deviceInformation, &interfaceData, interfaceDetails, requiredSize, NULL, NULL);
		if (!result)
		{
			free(interfaceDetails);
			continue;
		}
		HANDLE devcieHandle = CreateFile(interfaceDetails->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		free(interfaceDetails);

		if (devcieHandle == INVALID_HANDLE_VALUE) continue;

		HIDD_ATTRIBUTES hidDeviceAttributes;
		hidDeviceAttributes.Size = sizeof(HIDD_ATTRIBUTES);
		result = HidD_GetAttributes(devcieHandle, &hidDeviceAttributes);

		PHIDP_PREPARSED_DATA hidDeviceData;
		if (!result || (hidDeviceAttributes.VendorID != vid) ||	(hidDeviceAttributes.ProductID != pid) || !HidD_GetPreparsedData(devcieHandle, &hidDeviceData))
		{
			CloseHandle(devcieHandle);
			continue;
		}

		HIDP_CAPS hidDeviceCapabilities;
		if (!HidP_GetCaps(hidDeviceData, &hidDeviceCapabilities) || (usagePage > 0 && hidDeviceCapabilities.UsagePage != usagePage) ||	(usage > 0 && hidDeviceCapabilities.Usage != usage))
		{
			HidD_FreePreparsedData(hidDeviceData);
			CloseHandle(devcieHandle);
			continue;
		}

		HidD_FreePreparsedData(hidDeviceData);
		deviceHandle_ = devcieHandle;
		break;
	}

	readEventHandle_ = CreateEvent(NULL, TRUE, TRUE, NULL);
	writeEventHandle_ = CreateEvent(NULL, TRUE, TRUE, NULL);
}

bool HidDevice::write(const std::uint8_t* data, size_t length, std::uint32_t timeout)
{
	OVERLAPPED overlapped{ 0 };

	ResetEvent(&writeEventHandle_);
	overlapped.hEvent = writeEventHandle_;

	if (!WriteFile(deviceHandle_, data, static_cast<DWORD>(length), NULL, &overlapped))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			return false;
		}
		const auto result = WaitForSingleObject(writeEventHandle_, timeout);
		if (result == WAIT_TIMEOUT)
		{
			CancelIo(deviceHandle_);
			return false;
		}
		if (result != WAIT_OBJECT_0)
		{
			return false;
		}
	}

	DWORD bytesWritten{ 0 };
	if (!GetOverlappedResult(deviceHandle_, &overlapped, &bytesWritten, FALSE))
	{
		return false;
	}

	return (bytesWritten == length);
}

bool HidDevice::read(std::uint8_t* dataOut, size_t length, std::uint32_t timeout)
{
	OVERLAPPED overlapped{ 0 };

	ResetEvent(&readEventHandle_);
	overlapped.hEvent = readEventHandle_;
	if (!ReadFile(deviceHandle_, dataOut, static_cast<DWORD>(length), NULL, &overlapped))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			return false;
		}
		const auto result = WaitForSingleObject(readEventHandle_, timeout);
		if (result == WAIT_TIMEOUT)
		{
			CancelIo(deviceHandle_);
			return false;
		}
		if (result != WAIT_OBJECT_0)
		{
			return false;
		}
	}

	DWORD bytesRead{ 0 };
	if (!GetOverlappedResult(deviceHandle_, &overlapped, &bytesRead, FALSE))
	{
		return false;
	}
	return(bytesRead == length);
}

HidDevice::~HidDevice()
{
	if (deviceHandle_ != INVALID_HANDLE_VALUE)
	{
		CloseHandle(deviceHandle_);
	}
	if (readEventHandle_ != INVALID_HANDLE_VALUE)
	{
		CloseHandle(readEventHandle_);
	}
	if (writeEventHandle_ != INVALID_HANDLE_VALUE)
	{
		CloseHandle(writeEventHandle_);
	}
}