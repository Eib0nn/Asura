#include "asurautils.h"

BOOL dll_injection(DWORD PID);
VOID IndirectPrelude(
    IN HMODULE hNTDLL,
    IN LPCSTR NtFunction,
    OUT DWORD *SSN,
    OUT UINT_PTR *Syscall
);