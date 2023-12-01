#pragma once
#include "things.hpp"

// https://doxygen.reactos.org/d4/d67/sysldr_8c.html#a21f6d9eda6b929c322a7f1800eed3b1f
typedef NTSTATUS(*tMmLoadSystemImage)(PUNICODE_STRING FileName, PUNICODE_STRING NamePrefix, PUNICODE_STRING LoadedName, ULONG Flags, void** ModuleObject, void** ImageBaseAddress);
extern tMmLoadSystemImage MmLoadSystemImage;

extern UINT32* PspNotifyEnableMask;
extern LIST_ENTRY* PsLoadedModuleList;
extern void* SeValidateImageHeader,
	*KeInitAmd64SpecificState,
	*ExpLicenseWatchInitWorker;

namespace Loader {
	NTSTATUS LoadDriver();
}