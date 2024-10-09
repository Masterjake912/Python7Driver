#pragma once
//#include <iostream>

struct Python7Input
{
	ULONG ProcessId;
	char message[64];
	//std::string message;
};

struct Python7Output
{
	HANDLE hProcess;
};

#define IOCTL_OPEN_PROCESS CTL_CODE(0x8000, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IO_READ_REQUEST CTL_CODE(0x8000, 0x812, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define IO_WRITE_REQUEST CTL_CODE(0x8000, 0x813, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
