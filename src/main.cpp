#include "uefi.hpp"
#include "hooks.hpp"
#include "utils.hpp"
#include "defines.hpp"

static EFI_EXIT_BOOT_SERVICES ExitBootServicesOriginal;
static EFI_EVENT EventSetVirtualAddressMap;
static void* Winload;

void EFIAPI NotifySetVirtualAddressMap(
	IN EFI_EVENT Event,
	IN void* Context
) {
	Hook::OslArchTransferToKernel(Winload);

	g_ST->RuntimeServices->ConvertPointer(EFI_OPTIONAL_PTR, (void**)&g_ST);
};


EFI_STATUS EFIAPI ExitBootServicesHook(
	IN EFI_HANDLE ImageHandle,
	IN UINTN MapKey
) {
	Winload = _ReturnAddress();

	g_ST->BootServices->ExitBootServices = ExitBootServicesOriginal;
	return g_ST->BootServices->ExitBootServices(ImageHandle, MapKey);
}

EFI_STATUS EFIAPI UefiMain(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE* SystemTable
) {
	g_ST = SystemTable;

	EFI_STATUS Status = g_ST->BootServices->CreateEvent(EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE, TPL_NOTIFY, NotifySetVirtualAddressMap, NULL, &EventSetVirtualAddressMap);
	if (Status == EFI_SUCCESS) {
		ExitBootServicesOriginal = g_ST->BootServices->ExitBootServices;
		g_ST->BootServices->ExitBootServices = ExitBootServicesHook;
	}

	return Status;
}