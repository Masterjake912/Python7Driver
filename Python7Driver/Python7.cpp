#include <ntddk.h>
#include "..\Python7Driver\Python7Common.h"

void Python7Unload(PDRIVER_OBJECT);
NTSTATUS Python7CreateClose(PDEVICE_OBJECT, PIRP);
NTSTATUS Python7DeviceControl(PDEVICE_OBJECT, PIRP);
//NTSTATUS Python7IoControl(PDEVICE_OBJECT, PIRP);


extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	KdPrint(("Python7: DriverEntry\n"));
	KdPrint(("Registry path: %wZ\n", RegistryPath));
	DriverObject->DriverUnload = Python7Unload;

	RTL_OSVERSIONINFOW vi = { sizeof(vi) };
	NTSTATUS status = RtlGetVersion(&vi);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Failed in RtlGetVersion (0x%X)\n", status));
		return status;
	}

	KdPrint(("Windows Version: %u.%u.%u\n", vi.dwMajorVersion, vi.dwMinorVersion, vi.dwBuildNumber));

	DriverObject->MajorFunction[IRP_MJ_CREATE] = Python7CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = Python7CreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Python7DeviceControl;
	//DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Python7IoControl;

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\Python7");
	// RtlInitUnicodeString(&devName, L"\\Device\\Python7"); Same initialization as above

	PDEVICE_OBJECT DeviceObject;
	status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Failed in IoCreateDevice (0x%X\n", status));
		return status;
	}

	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Python7");
	status = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DeviceObject);
		KdPrint(("Failed in IoCreateSymbolLink (0x%X\n", status));
		return status;
	}

	return STATUS_SUCCESS;
}

void Python7Unload(PDRIVER_OBJECT DriverObject)
{
	KdPrint(("Python7: Unload\n"));
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\Python7");
	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS Python7CreateClose(PDEVICE_OBJECT, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, 0);
	return STATUS_SUCCESS;
}

//NTSTATUS Python7IoControl(PDEVICE_OBJECT, PIRP Irp)
//{
//	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
//	NTSTATUS status = STATUS_SUCCESS;
//	ULONG codeReq = stack->Parameters.DeviceIoControl.IoControlCode;
//	KdPrint(("In IO CONTROL"));
//	if (codeReq == IO_READ_REQUEST)
//	{
//		KdPrint(("Input Buffer Length: %u", stack->Parameters.DeviceIoControl.InputBufferLength));
//		KdPrint(("Value of Input Buffer: %s", stack->Parameters.DeviceIoControl.Type3InputBuffer));
//		if (stack->Parameters.DeviceIoControl.InputBufferLength > 0 &&
//			stack->Parameters.DeviceIoControl.Type3InputBuffer != NULL)
//		{
//			status = STATUS_SUCCESS;
//			char* readRequest = (char*)stack->Parameters.DeviceIoControl.Type3InputBuffer;
//			KdPrint(("Value read from Usermode: %u\n", readRequest));
//		}
//	}
//	else if (codeReq == IO_WRITE_REQUEST)
//	{
//		status = STATUS_SUCCESS;
//	}
//
//	Irp->IoStatus.Status = status;
//	Irp->IoStatus.Information = 0;
//	IoCompleteRequest(Irp, 0);
//	return status;
//}

NTSTATUS Python7DeviceControl(PDEVICE_OBJECT, PIRP Irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	auto& dict = stack->Parameters.DeviceIoControl;
	ULONG len = 0;

	switch (dict.IoControlCode)
	{
	case IOCTL_OPEN_PROCESS:
		if (dict.Type3InputBuffer == nullptr || Irp->UserBuffer == nullptr)
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}
		if (dict.InputBufferLength < sizeof(Python7Input) || dict.OutputBufferLength < sizeof(Python7Output))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		Python7Input* input = (Python7Input*)dict.Type3InputBuffer;
		//Python7Output* output = (Python7Output*)Irp->UserBuffer;

		KdPrint(("Message is: %s", input->message));
		/*OBJECT_ATTRIBUTES attr;
		InitializeObjectAttributes(&attr, nullptr, 0, nullptr, nullptr);
		CLIENT_ID cid = { 0 };
		cid.UniqueProcess = ULongToHandle(input->ProcessId);
		status = ZwOpenProcess(&output->hProcess, PROCESS_ALL_ACCESS, &attr, &cid);
		if (NT_SUCCESS(status))
		{
			len = sizeof(output);
		}*/
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = len;
	IoCompleteRequest(Irp, 0);
	return STATUS_SUCCESS;
}

