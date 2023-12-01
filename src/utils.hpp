#pragma once
#include "uefi.hpp"

extern CHAR16 SharedBufferWide[255];
extern CHAR SharedBuffer[255];

namespace Utils {
	bool WStrCmp(PUNICODE_STRING String1, PUNICODE_STRING String2);
	bool ToWide(CHAR* Text, INT32 Size, CHAR16* Out);
	void Print(const CHAR* Text, ...);
	void MemCpy(void* Dest, void* Src, UINT32 Size, bool DisableProtect = true);
	void MemSet(void* Dest, CHAR Value, UINT32 Size, bool DisableProtect = true);

	void* SignatureScan(void* Mem, const char* Pattern, const char* Mask, UINT32 Size, UINT32 Limit);
	PLDR_DATA_TABLE_ENTRY GetKernelModule(LIST_ENTRY* List, const WCHAR* Str, UINT32 Size);
}