#include "asurautils.h"

//------------------------ Variables ------------------------

UNICODE_STRING usFileName;
NTSTATUS ntStatus;
HANDLE hFileHandle;
OBJECT_ATTRIBUTES oa = {0};
IO_STATUS_BLOCK IoStatus = {0};
HMODULE hNTDLL = NULL;
WINBOOL lib_cleanup;
//add ntcreatefile flags here
//HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


//------------------------ Function prototypes ------------------------

//  Converts ANSI_STRING (in c-style) to UNICODE_STRING
wchar_t *AnsiToUnicode(const char *str){
    int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    wchar_t *wideCharString = new wchar_t[wideCharLength];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wideCharString, wideCharLength);
    return wideCharString;
}

//  Get a handle of the library module
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

//  Create multiple files using NtCreateFile and a loop with a oss combined string
int fileCreation (const std::string &path){

    hNTDLL = getMod(L"NTDLL");
    if (hNTDLL == NULL)
    {
        printf("unable to get a handle to NTDLL, error: 0x%lx\n", GetLastError());
        CLEANUP(NULL,hNTDLL,NULL,NULL);
        return EXIT_FAILURE;
    }

    RtlInitUnicodeStringEx thRtlInitUnicodeStringEx = (RtlInitUnicodeStringEx)GetProcAddress(hNTDLL, "RtlInitUnicodeStringEx");
    printf("Got the address of RtlInitUnicodeStringEx from NTDLL: 0x%p\n", thRtlInitUnicodeStringEx);

    NtCreateFile thNtCreateFile = (NtCreateFile)GetProcAddress(hNTDLL,"NtCreateFile");
    printf("Got the address of NtCreateFile from NTDLL: 0x%p\n", thNtCreateFile);

    int i = 1;
    std::ostringstream oss;
    while(i < 5){
        i++;
        oss << "\\??\\" << path << "\\" << i << ".txt";
        std::cout << "Created path and file: " << oss.str() << std::endl;
        
        wchar_t* wideCharToString = AnsiToUnicode(oss.str().c_str());
        
        ntStatus = thRtlInitUnicodeStringEx(&usFileName, wideCharToString);
        if (!NT_SUCCESS(ntStatus))
        {
            printf("Error when intializing unicode string, error: 0x%lx\n", ntStatus);
            CLEANUP(NULL, hNTDLL, NULL, NULL);
            return EXIT_FAILURE;
        }
        InitializeObjectAttributes(&oa, &usFileName, OBJ_CASE_INSENSITIVE, NULL, NULL)

        LARGE_INTEGER filesize;
        filesize.LowPart = IoStatus.Information;
        filesize.HighPart = IoStatus.Status;

        ntStatus = thNtCreateFile(&hFileHandle, 
            FILE_GENERIC_WRITE | FILE_GENERIC_READ, 
            &oa, 
            &IoStatus, 
            NULL, 
            FILE_ATTRIBUTE_NORMAL, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, 
            FILE_OPEN_IF, 
            FILE_NON_DIRECTORY_FILE, 
            NULL, 
            0);
            
        
        if (!NT_SUCCESS(ntStatus)){
            printf("Error when creating file with thNtCreateFile: 0x%lx\n", ntStatus);
            CLEANUP(NULL, hNTDLL, hFileHandle, NULL);
            return EXIT_FAILURE;
        }
        printf("Size of the written file: %zu bytes\n", filesize.QuadPart);

        delete[] wideCharToString;
        oss.str("");
    }
    CLEANUP(NULL, hNTDLL, hFileHandle, NULL);
    return 0;
}

//  Iterarate all subdirs and create one thread of fileCreation() for each one of it
void iterate_subdirs(const std::string &dir_path, std::vector<std::string> &dirs)
{
    std::string search_path = dir_path + "\\*";
    intptr_t handle;
    _finddata_t file_info;

    handle = _findfirst(search_path.c_str(), &file_info);
    if (handle == -1)
    {
        std::cerr << "Error: unable to open directory " << dir_path << std::endl;
        return;
    }

    do
    {
        std::string name = file_info.name;
        if (name == "." || name == "..")
        {
            continue;
        }

        std::string subdir_path = dir_path + "\\" + name;

        if (file_info.attrib & _A_SUBDIR)
        {
            std::cout << "Found subdirectory: " << subdir_path << std::endl;
            dirs.push_back(subdir_path); // add subdir_path to the vector
            iterate_subdirs(subdir_path, dirs);
            // create a new thread for Copy_A() and pass the subdirectory to it
            std::thread t(fileCreation, subdir_path);
            t.detach(); // detach the thread so it runs independently
        }
    } while (_findnext(handle, &file_info) == 0);

    _findclose(handle);
}

//  Cleanup.
DWORD CLEANUP(_In_opt_ HANDLE _hProcess_, _In_opt_ HMODULE _dllHandle_, _In_opt_ HANDLE _hFileHandle_, _In_opt_ HANDLE _hThread_){
    
    NtClose thNtClose = (NtClose)GetProcAddress(hNTDLL, "NtClose");

    if (_hProcess_ != NULL){
        printf("Closing handle to injected process...\n");
        ntStatus = thNtClose(_hProcess_);
        if (!ntStatus == STATUS_SUCCESS){
            printf("Failed to close process handle, error: 0x%x\n", ntStatus);
            return EXIT_FAILURE;
        }
        printf("Closed handle.\n");
    }

    if (_hThread_ != NULL){
        printf("Closing handle to thread....\n");
        ntStatus = thNtClose(_hThread_);
        if (!ntStatus == STATUS_SUCCESS){
            printf("Failed to close thread handle, error: 0x%x\n", ntStatus);
            return EXIT_FAILURE;
        }
        printf("Closed thread handle.\n");
    }

    if (_dllHandle_ != NULL){
        printf("Freeing library handle...\n");
        lib_cleanup = FreeLibrary(_dllHandle_);
        if(lib_cleanup == 0){
            printf("Failed to free library, error: 0x%lx\n", GetLastError());
            return EXIT_FAILURE;
        }
        printf("Loaded library freed.\n");
    }

    if (_hFileHandle_ != NULL){
        printf("Closing handle to file...\n");
        ntStatus = thNtClose(_hFileHandle_);
        if (!ntStatus == STATUS_SUCCESS){
            printf("Failed to close file handle, error: 0x%x\n", ntStatus);
            return EXIT_FAILURE;
        }
        printf("Closed file handle.\n");
    }
    printf("Finished the cleanup!\n");
    return EXIT_SUCCESS;
}

void testfoo1(){
    printf("function 1\n");
}

void testfoo2(){
    printf("function 2\n");
    testfoo1();
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved){
    switch (dwReason){
        case DLL_PROCESS_ATTACH:
        MessageBoxW(NULL, L"WTF", L"TNC", MB_OK|MB_HELP);
        std::string path = "C:\\Users\\thirras\\Desktop\\DANGEROUSTEST";
        std::vector<std::string> dirs;
        iterate_subdirs(path, dirs);
    }
    return TRUE;
}