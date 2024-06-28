#pragma once
#include <intrin.h>
#include "uefi.hpp"

namespace Utils {
	void MemCpy(void* Dest, void* Src, UINT32 Size, bool DisableProtect = true);
	void MemSet(void* Dest, char Value, UINT32 Size, bool DisableProtect = true);

	void* SignatureScan(void* Mem, const char* Pattern, const char* Mask, UINT32 Size, UINT32 Limit);
}