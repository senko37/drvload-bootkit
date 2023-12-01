#include "uefi.hpp"
#include "hook.hpp"
#include "utils.hpp"
#include "things.hpp"
#include <intrin.h>

void* Winload;

EFI_EVENT EventSetVirtualAddressMap;
void EFIAPI NotifySetVirtualAddressMap(
	IN EFI_EVENT Event,
	IN void* Context
) {
	Hook::OslArchTransferToKernel(Winload);

	g_ST->RuntimeServices->ConvertPointer(EFI_OPTIONAL_PTR, (void**)&g_ST);
};

EFI_EXIT_BOOT_SERVICES ExitBootServicesOriginal;
EFI_STATUS ExitBootServicesHook(
	IN EFI_HANDLE ImageHandle,
	IN UINTN MapKey
) {
	Winload = _ReturnAddress();

	g_ST->BootServices->ExitBootServices = ExitBootServicesOriginal;
	return g_ST->BootServices->ExitBootServices(ImageHandle, MapKey);
}

EFI_STATUS UefiMain(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE* SystemTable
) {
	g_ST = SystemTable;

	ExitBootServicesOriginal = g_ST->BootServices->ExitBootServices;
	g_ST->BootServices->ExitBootServices = ExitBootServicesHook;

	EFI_STATUS Status = g_ST->BootServices->CreateEvent(EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE, TPL_NOTIFY, NotifySetVirtualAddressMap, NULL, &EventSetVirtualAddressMap);

	return Status;
}