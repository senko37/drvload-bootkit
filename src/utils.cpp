#include "utils.hpp"
#include "things.hpp"
#include <intrin.h>

void Utils::MemCpy(void* Dest, void* Src, UINT32 Size, bool DisableProtect) {
	UINT64 Cr0, Cr4;
	if (DisableProtect) {
		Cr0 = __readcr0();
		Cr4 = __readcr4();

		// See intel docs
		__writecr4(Cr4 & ~(UINT64)(1 << 23)); // Set CR4.CET
		__writecr0(Cr0 & ~(UINT64)(1 << 16)); // Set CR0.WP
	}

	for (UINT32 i = 0; i < Size; i++)
		((char*)Dest)[i] = ((char*)Src)[i];

	if (DisableProtect) {
		__writecr0(Cr0); // Set CR0 back
		__writecr4(Cr4); // Set CR4 back
	}
}

void Utils::MemSet(void* Dest, CHAR Value, UINT32 Size, bool DisableProtect) {
	UINT64 Cr0, Cr4;
	if (DisableProtect) {
		Cr0 = __readcr0();
		Cr4 = __readcr4();

		// See intel docs
		__writecr4(Cr4 & ~(UINT64)(1 << 23)); // Set CR4.CET
		__writecr0(Cr0 & ~(UINT64)(1 << 16)); // Set CR0.WP
	}

	for (UINT32 i = 0; i < Size; i++)
		((char*)Dest)[i] = Value;

	if (DisableProtect) {
		__writecr0(Cr0); // Set CR0 back
		__writecr4(Cr4); // Set CR4 back
	}
}

void* Utils::SignatureScan(void* Mem, const char* Pattern, const char* Mask, UINT32 Size, UINT32 Limit) {
	for (UINT32 i = 0; i < Limit - Size; i++) {
		for (UINT32 a = 0; a < Size; a++) {
			if (Mask[a] == '?' || ((char*)Mem)[i + a] == Pattern[a]) {
				if (a == Size - 1)
					return &((char*)Mem)[i];
			}
			else
				break;
		}
	}

	return nullptr;
}