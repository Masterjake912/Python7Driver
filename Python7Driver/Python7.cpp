#include <ntifs.h>

extern "C" {
	NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName,
										PDRIVER_INITIALIZE InitializationFunction);

	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress,
											 PEPROCESS TargetProcess, PVOID TargetAddress,
											 SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode,
											 PSIZE_T ReturnSize);
}

// Helper for kernel debug printing. Prints messages in Debug but not Release
void dbg_print(PCSTR text) {
	// Otherwise you cannot build in Release mode.
#ifndef DEBUG
	UNREFERENCED_PARAMETER(text);
#endif  // !DEBUG

	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
}
namespace driver {
	namespace codes {
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

	NTSTATUS Python7CreateClose(PDEVICE_OBJECT, PIRP Irp)
	{
		Irp->IoStatus.Status = STATUS_SUCCESS;
		Irp->IoStatus.Information = 0;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}

	NTSTATUS Python7DeviceControl(PDEVICE_OBJECT, PIRP Irp)
	{
		dbg_print("[+] Device control function called.\n");

		NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
		PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
		
		auto& dict = stack->Parameters.DeviceIoControl;
		auto requestPacket = reinterpret_cast<RequestPacket*>(Irp->AssociatedIrp.SystemBuffer);
		static PEPROCESS targProcess = nullptr;
		
		if (stack == nullptr || requestPacket == nullptr) {
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
			return status;
		}

		switch (dict.IoControlCode)
		{
			case codes::attachTo:
				status = PsLookupProcessByProcessId(requestPacket->pid, &targProcess);
				break;
			case codes::readFrom:
				if (targProcess != nullptr)
					status = MmCopyVirtualMemory(targProcess, requestPacket->target,
						PsGetCurrentProcess(), requestPacket->buffer,
						requestPacket->size, KernelMode, &requestPacket->returnSize);
				break;
			case codes::writeTo:
				if (targProcess != nullptr)
					status = MmCopyVirtualMemory(PsGetCurrentProcess(), requestPacket->buffer,
						targProcess, requestPacket->target,
						requestPacket->size, KernelMode, &requestPacket->returnSize);
				break;
			default:
				break;
		}

		Irp->IoStatus.Status = status;
		Irp->IoStatus.Information = sizeof(RequestPacket);
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}

}

NTSTATUS DriverMain(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\Python7");
	PDEVICE_OBJECT DeviceObject = nullptr;
	NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		dbg_print("[-] Failed to create driver device successfully.\n");
		return status;
	}

	dbg_print("[+] Driver device created successfully!\n");

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\Python7");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DeviceObject);
		dbg_print("[-] Failed to create symbolic link to device successfully.\n");
		return status;
	}

	dbg_print("[+] Created a symbolic link to device successfully!\n");

	// Setup Device and Driver objects
	SetFlag(DeviceObject->Flags, DO_BUFFERED_IO);
	DriverObject->MajorFunction[IRP_MJ_CREATE] = driver::Python7CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = driver::Python7CreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driver::Python7DeviceControl;
	ClearFlag(DeviceObject->Flags, DO_DEVICE_INITIALIZING);

	dbg_print("[+] Driver has been succesfully intialized!\n");

	return STATUS_SUCCESS;
}

// KdMapper uses this as an entry point
extern "C" NTSTATUS DriverEntry()
{
	UNICODE_STRING driverName = RTL_CONSTANT_STRING(L"\\Driver\\Python7");
	dbg_print("[+] Driver is poopy!\n");
	return IoCreateDriver(&driverName, &DriverMain);
}
