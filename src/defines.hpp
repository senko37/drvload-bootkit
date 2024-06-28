#pragma once

#define RTL_CONSTANT_STRING(s) { sizeof(s) - sizeof((s)[0]), sizeof(s), (WCHAR*)s }
#define STATUS_SUCCESS 0x00000000

typedef unsigned char BOOLEAN;
typedef wchar_t CHAR16;
typedef char CHAR;
typedef wchar_t WCHAR;
typedef unsigned int UINT32;
typedef int INT32;
typedef unsigned long long UINT64;
typedef long long INT64;
typedef unsigned long ULONG;
typedef UINT64 ULONG_PTR;
typedef long LONG;
typedef long long LONGLONG;
typedef unsigned short USHORT;
typedef LONG NTSTATUS;

union LARGE_INTEGER {
    struct {
        ULONG LowPart;
        LONG HighPart;
    };
    struct {
        ULONG LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
};

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    WCHAR* Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY* Flink;
	struct _LIST_ENTRY* Blink;
} LIST_ENTRY;

typedef struct _LOADER_PARAMETER_BLOCK {
    ULONG OsMajorVersion;
    ULONG OsMinorVersion;
    ULONG Size;
    ULONG OsLoaderSecurityVersion;
    struct _LIST_ENTRY LoadOrderListHead;
    struct _LIST_ENTRY MemoryDescriptorListHead;
    struct _LIST_ENTRY BootDriverListHead;
    struct _LIST_ENTRY EarlyLaunchListHead;
    struct _LIST_ENTRY CoreDriverListHead;
    struct _LIST_ENTRY CoreExtensionsDriverListHead;
    struct _LIST_ENTRY TpmCoreDriverListHead;
} *PLOADER_PARAMETER_BLOCK;

typedef struct _LDR_DATA_TABLE_ENTRY {
    struct _LIST_ENTRY InLoadOrderLinks;
    struct _LIST_ENTRY InMemoryOrderLinks;
    struct _LIST_ENTRY InInitializationOrderLinks;
    void* DllBase;
    void* EntryPoint;
    ULONG SizeOfImage;
    struct _UNICODE_STRING FullDllName;
    struct _UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _DBGKD_GET_VERSION64 {
    USHORT MajorVersion;
    USHORT MinorVersion;
    CHAR ProtocolVersion;
    CHAR KdSecondaryVersion;
    USHORT Flags;
    USHORT MachineType;
    CHAR MaxPacketType;
    CHAR MaxStateChange;
    CHAR MaxManipulate;
    CHAR Simulation;
    USHORT Unused[1];
    UINT64 KernBase;
    _LIST_ENTRY* PsLoadedModuleList;
    UINT64 DebuggerDataList;
} DBGKD_GET_VERSION64, *PDBGKD_GET_VERSION64;

typedef struct _IMAGE_DOS_HEADER {
    USHORT e_magic;
    USHORT e_cblp;
    USHORT e_cp;
    USHORT e_crlc;
    USHORT e_cparhdr;
    USHORT e_minalloc;
    USHORT e_maxalloc;
    USHORT e_ss;
    USHORT e_sp;
    USHORT e_csum;
    USHORT e_ip;
    USHORT e_cs;
    USHORT e_lfarlc;
    USHORT e_ovno;
    USHORT e_res[4];
    USHORT e_oemid;
    USHORT e_oeminfo;
    USHORT e_res2[10];
    LONG e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
    USHORT Machine;
    USHORT NumberOfSections;
    ULONG TimeDateStamp;
    ULONG PointerToSymbolTable;
    ULONG NumberOfSymbols;
    USHORT SizeOfOptionalHeader;
    USHORT Characteristics;
};

typedef struct _IMAGE_DATA_DIRECTORY {
    ULONG VirtualAddress;
    ULONG Size;
};

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    USHORT Magic;
    CHAR MajorLinkerVersion;
    CHAR MinorLinkerVersion;
    ULONG SizeOfCode;
    ULONG SizeOfInitializedData;
    ULONG SizeOfUninitializedData;
    ULONG AddressOfEntryPoint;
    ULONG BaseOfCode;
    UINT64 ImageBase;
    ULONG SectionAlignment;
    ULONG FileAlignment;
    USHORT MajorOperatingSystemVersion;
    USHORT MinorOperatingSystemVersion;
    USHORT MajorImageVersion;
    USHORT MinorImageVersion;
    USHORT MajorSubsystemVersion;
    USHORT MinorSubsystemVersion;
    ULONG Win32VersionValue;
    ULONG SizeOfImage;
    ULONG SizeOfHeaders;
    ULONG CheckSum;
    USHORT Subsystem;
    USHORT DllCharacteristics;
    UINT64 SizeOfStackReserve;
    UINT64 SizeOfStackCommit;
    UINT64 SizeOfHeapReserve;
    UINT64 SizeOfHeapCommit;
    ULONG LoaderFlags;
    ULONG NumberOfRvaAndSizes;
    struct _IMAGE_DATA_DIRECTORY DataDirectory[16];
};

typedef struct _IMAGE_NT_HEADERS64 {
    ULONG Signature;
    struct _IMAGE_FILE_HEADER FileHeader;
    struct _IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef NTSTATUS(*tDriverEntry)(void* DriverObject, PUNICODE_STRING RegistryPath);