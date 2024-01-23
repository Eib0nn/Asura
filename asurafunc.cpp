#include "asurautils.h"

//------------------------ Variables ------------------------

UNICODE_STRING usFileName;
NTSTATUS ntStatus;
HANDLE hFileHandle;
OBJECT_ATTRIBUTES oa = {0};
IO_STATUS_BLOCK IoStatus = {0};
HMODULE hNTDLL = NULL;
WINBOOL lib_cleanup;
//HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


//------------------------ Function prototypes ------------------------
wchar_t *AnsiToUnicode(const char *str){
    int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    wchar_t *wideCharString = new wchar_t[wideCharLength];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wideCharString, wideCharLength);
    return wideCharString;
}

HMODULE getMod(IN LPCWSTR modName)
{

    HMODULE hModule = NULL;

    printf("trying to get a handle to %S", modName);
    hModule = GetModuleHandleW(modName);

    if (hModule == NULL)
    {
        printf("failed to get a handle to the module. error: 0x%lx\n", GetLastError());
        return NULL;
    }

    else
    {
        printf("got a handle to the module!");
        printf("\\___[ %S\n\t\\_0x%p]\n", modName, hModule);
        return hModule;
    }
}

int fileCreation (const std::string &path){

    hNTDLL = getMod(L"NTDLL");
    if (hNTDLL == NULL)
    {
        printf("unable to get a handle to NTDLL, error: 0x%lx", GetLastError());
        CLEANUP(NULL,hNTDLL,NULL,NULL);
        return EXIT_FAILURE;
    }

    //Make a function to dont have this enormous line
    //make  a cleanup GOT
    //Make a good testing lol
    RtlInitUnicodeStringEx thRtlInitUnicodeStringEx = (RtlInitUnicodeStringEx)GetProcAddress(hNTDLL, "RtlInitUnicodeString");
    NtCreateFile thNtCreateFile = (NtCreateFile)GetProcAddress(hNTDLL,"NtCreateFile");
    
    int i = 1;
    std::ostringstream oss;
    oss << path << "\\" << i << ".txt";
    wchar_t* wideCharToString = AnsiToUnicode(oss.str().c_str());
    thRtlInitUnicodeStringEx(&usFileName, wideCharToString);
    InitializeObjectAttributes(&oa, &usFileName, OBJ_CASE_INSENSITIVE, NULL, NULL);
    ntStatus = thNtCreateFile(&hFileHandle, GENERIC_WRITE | GENERIC_READ, &oa, &IoStatus, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN,0,NULL,0);
    if (ntStatus == NULL){
        printf("Error when creating file with thNtCreateFile: 0x%lx", GetLastError());
        CLEANUP(NULL, NULL, hFileHandle, NULL);
        return EXIT_FAILURE;
    }
    
    //HANDLE hFile = CreateFileA(oss.str().c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY, NULL);
    
    oss.str("");
    return 1;
}

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

DWORD CLEANUP(_In_opt_ HANDLE _hProcess_, _In_opt_ HMODULE _dllHandle_, _In_opt_ HANDLE _hFileHandle_, _In_opt_ HANDLE _hThread_){
    
    NtClose thNtClose = (NtClose)GetProcAddress(hNTDLL, "NtClose");

    if (_hProcess_ != NULL){
        printf("Closing handle to injected process...");
        ntStatus = thNtClose(_hProcess_);
        if (!ntStatus == STATUS_SUCCESS){
            printf("Failed to close process handle, error: 0x%x", ntStatus);
            return EXIT_FAILURE;
        }
        printf("Closed handle.");
    }

    if (_hThread_ != NULL){
        printf("Closing handle to thread....");
        ntStatus = thNtClose(_hThread_);
        if (!ntStatus == STATUS_SUCCESS){
            printf("Failed to close thread handle, error: 0x%x", ntStatus);
            return EXIT_FAILURE;
        }
        printf("Closed thread handle.");
    }

    if (_dllHandle_ != NULL){
        printf("Freeing library handle...");
        lib_cleanup = FreeLibrary(_dllHandle_);
        if(lib_cleanup == 0){
            printf("Failed to free library, error: 0x%lx", GetLastError());
            return EXIT_FAILURE;
        }
        printf("Loaded library freed.");
    }

    if (_hFileHandle_ != NULL){
        printf("Closing handle to file...");
        ntStatus = thNtClose(_hFileHandle_);
        if (!ntStatus == STATUS_SUCCESS){
            printf("Failed to close file handle, error: 0x%x", ntStatus);
            return EXIT_FAILURE;
        }
        printf("Closed file handle.");
    }
    printf("Finished the cleanup! bye bye");
    return EXIT_SUCCESS;
}

void testfoo1(){
    printf("function 1\n");
}

void testfoo2(){
    printf("function 2\n");
    testfoo1();
}
