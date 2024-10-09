#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include "..\Python7Driver\Python7Common.h"

#define IO_READ_REQUEST CTL_CODE(0x8000, 0x812, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define IO_WRITE_REQUEST CTL_CODE(0x8000, 0x813, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

int main()
{
    HANDLE hDriver = CreateFileA("\\\\.\\Python7", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (hDriver == INVALID_HANDLE_VALUE)
    {
        printf("Error opening device (%u)\n", GetLastError());
        return 1;
    }
    // Test values
    Python7Input input;
    char ReadRequest[64] = "Hello World";
    strcpy_s(input.message, ReadRequest);
    Python7Output output;
    //std::string ReturnBytes;
    DWORD bytes;
    bool ok = DeviceIoControl(hDriver, IOCTL_OPEN_PROCESS, &input, sizeof(input), &output, sizeof(output), &bytes, 0);
}
