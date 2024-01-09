#include "asurautils.h"

//------------------------ Variables ------------------------

UNICODE_STRING usFileName;
NTSTATUS ntStatus;
HANDLE hFileHandle;
OBJECT_ATTRIBUTES oa = {0};
IO_STATUS_BLOCK IoStatus = {0};
HMODULE hNTDLL = NULL;
//HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

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
    }

    // get this more clean (and readable) and see how
    // getting multiple modules of NTDLL inside another dll
    // will work. tests and more tests lol
    RtlInitUnicodeString thRtlInitUnicodeString = (RtlInitUnicodeString)GetProcAddress(hNTDLL, "RtlInitUnicodeString");
    NtCreateFile thNtCreateFile = (NtCreateFile)GetProcAddress(hNTDLL,"NtCreateFile");


    int i = 1;
    std::ostringstream oss;
    oss << path << "\\" << i << ".txt";

    //thRtlInitUnicodeString(&usFileName, (PWSTR)oss.str());
    // the code above doesnt work because unicode is ugly and require a UTF-16 string
    // my code is ANSI so its UTF-8, so it doesnt fit in RtlInitUnicodeString()
    // i will make a function to convert strings probably using MultiByteToWideChar()
    // this will be a pain in the ass lol
    
    HANDLE hFile = CreateFileA(oss.str().c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY, NULL);
    
    oss.str("");
    
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Error when creatin file\n");
        CloseHandle(hFile);
        return 0;
    }
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

void testfoo1(){
    printf("function 1\n");
}

void testfoo2(){
    printf("function 2\n");
    testfoo1();
}