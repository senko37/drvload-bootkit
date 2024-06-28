#pragma once

namespace Hook {
	void OslArchTransferToKernel(void* Address);
}

// https://doxygen.reactos.org/db/d2f/rosload_8c.html#a6e7a68dc1192093a08809b571179d0a5
typedef void(*tOslArchTransferToKernel)(PLOADER_PARAMETER_BLOCK LoaderBlock, void* KernelEntrypoint);
// https://doxygen.reactos.org/d8/de9/ntoskrnl_2ex_2init_8c.html#ad5fa457150906ff5bc6e71733aebede7
typedef void(*tPhase1Initialization)(void* StartContext);