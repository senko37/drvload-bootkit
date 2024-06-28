#pragma once
#include "defines.hpp"

namespace Loader {
	bool LoadDriver(PLDR_DATA_TABLE_ENTRY Ntoskrnl);
}

// https://doxygen.reactos.org/d4/d67/sysldr_8c.html#a21f6d9eda6b929c322a7f1800eed3b1f
typedef NTSTATUS(*tMmLoadSystemImage)(PUNICODE_STRING FileName, PUNICODE_STRING NamePrefix, PUNICODE_STRING LoadedName, ULONG Flags, void** ModuleObject, void** ImageBaseAddress);