#include "eibon_no_sho.h"

BOOL DLLInjection(_In_ DWORD ProcessId, _In_ LPCWSTR DLLPath, _In_ SIZE_T DLLPathSize){
    
    BOOL STATE = TRUE;
    PVOID RemoteBuffer = NULL;
    HANDLE ThreadHandle = NULL;
    HANDLE ProcessHandle = NULL;
    HMODULE Kernel32Handle = NULL;
    SIZE_T BytesWritten = 0;

    if (NULL == DLLPath || 0 == DLLPathSize){
        printf("Dll path was not set, returning...\n");
        return FALSE;
    }

    printf("supplied DLL: \"%S\"\n", DLLPath);
    printf("trying to get a handle on the process (%ld)...\n", ProcessId);
    ProcessHandle = OpenProcess(
        (PROCESS_VM_OPERATION | PROCESS_VM_OPERATION),
        FALSE,
        ProcessId
    );
    if (NULL == ProcessHandle){
        printf("Error on OpenProcess: 0x%x\n", GetLastError());
        return FALSE;
    }

    printf("[0x%p] got a handle on the process!\n", ProcessHandle);

    Kernel32Handle = GetModuleHandleW(L"Kernel32");

    if (NULL == Kernel32Handle){
        printf("Error on GetModuleHandleW: 0x%x\n", GetLastError());
        return FALSE;
    }

    printf("[0x%p] got a handle on Kernel32.dll!\n", Kernel32Handle);

    LPTHREAD_START_ROUTINE p_LoadLibraryW = (LPTHREAD_START_ROUTINE)GetProcAddress(
        Kernel32Handle, "LoadLibraryW");
    if (NULL == p_LoadLibraryW)
    {
        printf("failed to get the address of LoadLibraryW()");
        STATE = FALSE;
        goto CLEANUP;
    }
    printf("[0x%p] got the address of LoadLibraryW()!", p_LoadLibraryW);

    RemoteBuffer = VirtualAllocEx(
        ProcessHandle,
        NULL,
        DLLPathSize,
        (MEM_COMMIT | MEM_RESERVE),
        PAGE_READWRITE);
    if (NULL == RemoteBuffer)
    {
        printf("Error when allocating virtual mem: 0x%x\n", GetLastError());
        STATE = FALSE;
        goto CLEANUP;
    }
    printf("[0x%p] [RW-] allocated a buffer with PAGE_READWRITE permissions.\n", RemoteBuffer);

    WriteProcessMemory(
        ProcessHandle,
        RemoteBuffer,
        DLLPath,
        DLLPathSize,
        &BytesWritten);
    printf("[0x%p] [RW-] wrote %zu-bytes to allocated buffer!\n", RemoteBuffer, DLLPathSize);

    ThreadHandle = CreateRemoteThread(
        ProcessHandle,
        NULL,
        0,
        p_LoadLibraryW,
        RemoteBuffer, /* argument for LoadLibrary() */
        0,
        0);
    if (NULL == ThreadHandle)
    {
        printf("Error when creating remote thread: 0x%x\n", GetLastError());
        STATE = FALSE;
        goto CLEANUP;
    }
    printf("[0x%p] got a handle on the thread! waiting for it to finish execution...\n", ThreadHandle);

    WaitForSingleObject(ThreadHandle, INFINITE);
    printf("[0x%p] thread finished execution! cleaning up...\n", ThreadHandle);

CLEANUP:

    printf("beginning cleanup...");
    if (ThreadHandle)
    {
        CloseHandle(ThreadHandle);
        printf("[0x%p] closed thread handle", ThreadHandle);
    }

    if (ProcessHandle)
    {
        CloseHandle(ProcessHandle);
        printf("[0x%p] closed process handle", ProcessHandle);
    }

    if (RemoteBuffer)
    {
        VirtualFree(RemoteBuffer, 0, MEM_RELEASE);
        printf("[0x%p] remote buffer freed", RemoteBuffer);
    }

    return STATE;
}