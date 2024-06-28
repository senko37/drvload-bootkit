#pragma once
#include "defines.hpp"

#define IN
#define OUT
#define OPTIONAL
#define EFIAPI
#define EFI_SUCCESS 0

#define EFI_BLACK 0x00
#define EFI_BLUE 0x01
#define EFI_GREEN 0x02
#define EFI_CYAN 0x03
#define EFI_RED 0x04
#define EFI_MAGENTA 0x05
#define EFI_BROWN 0x06
#define EFI_LIGHTGRAY 0x07
#define EFI_BRIGHT 0x08
#define EFI_DARKGRAY (EFI_BLACK | EFI_BRIGHT)
#define EFI_LIGHTBLUE 0x09
#define EFI_LIGHTGREEN 0x0A
#define EFI_LIGHTCYAN 0x0B
#define EFI_LIGHTRED 0x0C
#define EFI_LIGHTMAGENTA 0x0D
#define EFI_YELLOW 0x0E
#define EFI_WHITE 0x0F
#define EFI_BACKGROUND_BLACK 0x00
#define EFI_BACKGROUND_BLUE 0x10
#define EFI_BACKGROUND_GREEN 0x20
#define EFI_BACKGROUND_CYAN 0x30
#define EFI_BACKGROUND_RED 0x40
#define EFI_BACKGROUND_MAGENTA 0x50
#define EFI_BACKGROUND_BROWN 0x60
#define EFI_BACKGROUND_LIGHTGRAY 0x70

#define EVT_SIGNAL_EXIT_BOOT_SERVICES 0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202

#define TPL_APPLICATION 4
#define TPL_CALLBACK 8
#define TPL_NOTIFY 16
#define TPL_HIGH_LEVEL 31

#define EFI_OPTIONAL_PTR 0x00000001

typedef unsigned long long EFI_STATUS;
typedef void* EFI_HANDLE;
typedef UINT64 UINTN;
typedef UINTN EFI_TPL;
typedef void* EFI_EVENT;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI* EFI_TEXT_STRING) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
	IN CHAR16* String
	);

typedef
EFI_STATUS
(EFIAPI* EFI_TEXT_SET_ATTRIBUTE) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
	IN UINTN Attribute
	);

typedef
EFI_STATUS
(EFIAPI* EFI_TEXT_CLEAR_SCREEN) (
	IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This
	);

typedef struct {
	UINT64 Signature;
	UINT32 Revision;
	UINT32 HeaderSize;
	UINT32 CRC32;
	UINT32 Reserved;
} EFI_TABLE_HEADER;

typedef struct {
	INT32 MaxMode;
	// current settings
	INT32 Mode;
	INT32 Attribute;
	INT32 CursorColumn;
	INT32 CursorRow;
	BOOLEAN CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
	void* Reset;
	EFI_TEXT_STRING OutputString;
	void* TestString;
	void* QueryMode;
	void* SetMode;
	EFI_TEXT_SET_ATTRIBUTE SetAttribute;
	EFI_TEXT_CLEAR_SCREEN ClearScreen;
	void* SetCursorPosition;
	void* EnableCursor;
	SIMPLE_TEXT_OUTPUT_MODE* Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

#define EFI_RUNTIME_SERVICES_SIGNATURE 0x56524553544e5552

typedef
EFI_STATUS
(EFIAPI* EFI_CONVERT_POINTER)(
	IN UINTN DebugDisposition,
	IN void** Address
	);

typedef struct {
	EFI_TABLE_HEADER Hdr;
	//
	// Time Services
	//
	void* GetTime;
	void* SetTime;
	void* GetWakeupTime;
	void* SetWakeupTime;
	//
	// Virtual Memory Services
	//
	void* SetVirtualAddressMap;
	EFI_CONVERT_POINTER ConvertPointer;
	//
	// Variable Services
	//
	void* GetVariable;
	void* GetNextVariableName;
	void* SetVariable;
	//
	// Miscellaneous Services
	//
	void* GetNextHighMonotonicCount;
	void* ResetSystem;
	//
	// UEFI 2.0 Capsule Services
	//
	void* UpdateCapsule;
	void* QueryCapsuleCapabilities;
	//
	// Miscellaneous UEFI 2.0 Service
	//
	void* QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

#define EFI_BOOT_SERVICES_SIGNATURE 0x56524553544f4f42

typedef
EFI_STATUS
(EFIAPI* EFI_EXIT_BOOT_SERVICES) (
	IN EFI_HANDLE ImageHandle,
	IN UINTN MapKey
	);

typedef
void
(EFIAPI* EFI_EVENT_NOTIFY) (
	IN EFI_EVENT Event,
	IN void* Context
	);

typedef
EFI_STATUS
(EFIAPI* EFI_CREATE_EVENT) (
	IN UINT32 Type,
	IN EFI_TPL NotifyTpl,
	IN EFI_EVENT_NOTIFY NotifyFunction, OPTIONAL
	IN void* NotifyContext, OPTIONAL
	OUT EFI_EVENT* Event
	);

typedef struct {
	EFI_TABLE_HEADER Hdr;
	//
	// Task Priority Services
	//
	void* RaiseTPL; // EFI 1.0+
	void* RestoreTPL; // EFI 1.0+
	//
	// Memory Services
	//
	void* AllocatePages; // EFI 1.0+
	void* FreePages; // EFI 1.0+
	void* GetMemoryMap; // EFI 1.0+
	void* AllocatePool; // EFI 1.0+
	void* FreePool; // EFI 1.0+
	//
	// Event & Timer Services
	//
	EFI_CREATE_EVENT CreateEvent; // EFI 1.0+
	void* SetTimer; // EFI 1.0+
	void* WaitForEvent; // EFI 1.0+
	void* SignalEvent; // EFI 1.0+
	void* CloseEvent; // EFI 1.0+
	void* CheckEvent; // EFI 1.0+
	//
	// Protocol Handler Services
	//
	void* InstallProtocolInterface; // EFI 1.0+
	void* ReinstallProtocolInterface; // EFI 1.0+
	void* UninstallProtocolInterface; // EFI 1.0+
	void* HandleProtocol; // EFI 1.0+
	void* Reserved; // EFI 1.0+
	void* RegisterProtocolNotify; // EFI 1.0+
	void* LocateHandle; // EFI 1.0+
	void* LocateDevicePath; // EFI 1.0+
	void* InstallConfigurationTable; // EFI 1.0+
	//
	// Image Services
	//
	void* LoadImage; // EFI 1.0+
	void* StartImage; // EFI 1.0+
	void* Exit; // EFI 1.0+
	void* UnloadImage; // EFI 1.0+
	EFI_EXIT_BOOT_SERVICES ExitBootServices; // EFI 1.0+
	//
	// Miscellaneous Services
	//
	void* GetNextMonotonicCount; // EFI 1.0+
	void* Stall; // EFI 1.0+
	void* SetWatchdogTimer; // EFI 1.0+
	//
	// DriverSupport Services
	//
	void* ConnectController; // EFI 1.1
	void* DisconnectController;// EFI 1.1+
	//
	// Open and Close Protocol Services
	//
	void* OpenProtocol; // EFI 1.1+
	void* CloseProtocol; // EFI 1.1+
	void* OpenProtocolInformation; // EFI 1.1+
	//
	// Library Services
	//
	void* ProtocolsPerHandle; // EFI 1.1+
	void* LocateHandleBuffer; // EFI 1.1+
	void* LocateProtocol; // EFI 1.1+
	void*
		InstallMultipleProtocolInterfaces; // EFI 1.1+
	void*
		UninstallMultipleProtocolInterfaces; // EFI 1.1+
	//
	// 32-bit CRC Services
	//
	void* CalculateCrc32; // EFI 1.1+
	//
	// Miscellaneous Services
	//
	void* CopyMem; // EFI 1.1+
	void* SetMem; // EFI 1.1+
	void* CreateEventEx; // UEFI 2.0+
} EFI_BOOT_SERVICES;

typedef struct {
	EFI_TABLE_HEADER Hdr;
	CHAR16* FirmwareVendor;
	UINT32 FirmwareRevision;
	EFI_HANDLE ConsoleInHandle;
	void* ConIn;
	EFI_HANDLE ConsoleOutHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
	EFI_HANDLE StandardErrorHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* StdErr;
	EFI_RUNTIME_SERVICES* RuntimeServices;
	EFI_BOOT_SERVICES* BootServices;
	UINTN NumberOfTableEntries;
	//EFI_CONFIGURATION_TABLE* ConfigurationTable;
} EFI_SYSTEM_TABLE;

extern EFI_SYSTEM_TABLE* g_ST;