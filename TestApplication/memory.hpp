#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>

DWORD MyGetProcessId(const wchar_t* ProcessName);
uintptr_t GetModuleBase(const DWORD pid, const wchar_t* ModuleName);

namespace driver
{
	namespace codes
	{
		constexpr ULONG attachTo = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
		constexpr ULONG readFrom = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
		constexpr ULONG writeTo = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
	}

	struct RequestPacket {
		HANDLE pid;

		PVOID target;
		PVOID buffer;

		SIZE_T size;
		SIZE_T returnSize;
	};

	bool AttachToProcess(HANDLE hDriver, const DWORD pid);

	template <class T>
	T ReadMemory(HANDLE hDriver, const uintptr_t address)
	{
		T temp = {};
		RequestPacket requestPacket;
		requestPacket.target = reinterpret_cast<PVOID>(address);
		requestPacket.buffer = &temp;
		requestPacket.size = sizeof(T);
		DeviceIoControl(hDriver, codes::readFrom, &requestPacket, sizeof(requestPacket),
			&requestPacket, sizeof(requestPacket), nullptr, nullptr);
		return temp;
	}

	template <class T>
	void WriteMemory(HANDLE hDriver, const uintptr_t address, const T& value)
	{
		RequestPacket requestPacket;
		requestPacket.target = reinterpret_cast<PVOID>(address);
		requestPacket.buffer = (PVOID)&value;
		requestPacket.size = sizeof(T);
		DeviceIoControl(hDriver, codes::writeTo, &requestPacket, sizeof(requestPacket),
			&requestPacket, sizeof(requestPacket), nullptr, nullptr);
	}
}