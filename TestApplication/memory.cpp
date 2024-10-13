#include "memory.hpp"

DWORD MyGetProcessId(const wchar_t* ProcessName)
{
	DWORD pid = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot == INVALID_HANDLE_VALUE)
		return pid;

	PROCESSENTRY32W entry = {};
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(snapshot, &entry) != TRUE)
	{
		CloseHandle(snapshot);
		return pid;
	}
	if (_wcsicmp(ProcessName, entry.szExeFile) == 0)
	{
		pid = entry.th32ProcessID;
	}
	else
	{
		do
		{
			if (_wcsicmp(ProcessName, entry.szExeFile) == 0)
			{
				pid = entry.th32ProcessID;
				break;
			}
		} while (Process32NextW(snapshot, &entry) == TRUE);
	}

	CloseHandle(snapshot);
	return pid;
}


bool driver::AttachToProcess(HANDLE hDriver, const DWORD pid)
{
	RequestPacket requestPacket;
	requestPacket.pid = reinterpret_cast<HANDLE>(pid);
	return DeviceIoControl(hDriver, driver::codes::attachTo, &requestPacket,
						   sizeof(requestPacket), &requestPacket,
						   sizeof(requestPacket), nullptr, nullptr);
}

uintptr_t GetModuleBase(const DWORD pid, const wchar_t* ModuleName)
{
	uintptr_t ModuleBase = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (snapshot == INVALID_HANDLE_VALUE)
		return ModuleBase;

	MODULEENTRY32W entry = {};
	entry.dwSize = sizeof(MODULEENTRY32W);

	if (Module32FirstW(snapshot, &entry) != TRUE)
	{
		CloseHandle(snapshot);
		return ModuleBase;
	}

	do
	{
		if (wcsstr(ModuleName, entry.szModule) != 0)
		{
			ModuleBase = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
			break;
		}
	} while (Module32NextW(snapshot, &entry) == TRUE);

	CloseHandle(snapshot);
	return ModuleBase;
}