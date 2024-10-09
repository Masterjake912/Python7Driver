#include <Windows.h>
#include <stdio.h>

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
    char ReadRequest[64] = "Hello World";
    char ReturnBytes[64];
    DWORD bytes;
    bool ok = DeviceIoControl(hDriver, IO_READ_REQUEST, &ReadRequest, sizeof(ReadRequest), &ReturnBytes, sizeof(ReadRequest), 0, 0);
}
