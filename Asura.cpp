#include "asurautils.h"

int main()
{
    HMODULE dllHandle = LoadLibrary(L"AsuraDll.dll");

    if(dllHandle == NULL){
        printf("Error when loading asuralib, error: 0x%lx\n", GetLastError());
        return 1;
    }

    PIterate_sub iterate_foo = (PIterate_sub)GetProcAddress(dllHandle, "iterate_subdirs");
    PFileCreation copy_func = (PFileCreation)GetProcAddress(dllHandle, "fileCreation");

    if (iterate_foo == NULL || copy_func == NULL){
        printf("Error when exporting functions, error: 0x%lx", GetLastError());
        FreeLibrary(dllHandle);
        return 1;
    }

    //{...}
    /*
    std::vector<std::string> dirs;
    TCHAR path[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, path))){
        std::basic_string<TCHAR> userFolderPath(path, path+ _tcslen(path));
        std::cout << "User folder path: " << std::string(userFolderPath.begin(), userFolderPath.end()) << std::endl;
        iterate_subdirs(std::string(userFolderPath.begin(), userFolderPath.end()), dirs);
    }
    else{
        std::cerr << "Failed to retrieve user folder path. error --> " << GetLastError() << std::endl;
    }
    while(true){}
    */
   // make everything here as a dll call
}

