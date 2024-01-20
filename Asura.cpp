#include "asurautils.h"

int main()
{
    HMODULE dllHandle = LoadLibraryA("AsuraDll.dll");

    if(dllHandle == NULL){
        printf("Error when loading asuralib, error: 0x%lx\n", GetLastError());
        return 1;
    }

    //PIterate_sub iterate_foo = (PIterate_sub)GetProcAddress(dllHandle, "iterate_subdirs");
    //PFileCreation copy_func = (PFileCreation)GetProcAddress(dllHandle, "fileCreation");

    foo1 func1 = (foo1)GetProcAddress(dllHandle, "testfoo1");
    foo2 func2 = (foo2)GetProcAddress(dllHandle, "testfoo2");

    // test tommorrow these things
    AnsiToUnicode_ thAnsiToUnicode = (AnsiToUnicode_)GetProcAddress(dllHandle, "AsuraToUnicode");
    const char* test = "hi";    
    wchar_t* wideCharToString = thAnsiToUnicode(test);
    std::wcout<<L"converted string: " << wideCharToString<< std::endl;
    delete[] wideCharToString;
    // till there.

    if (func1 == NULL || func2 == NULL){
        printf("Error when exporting functions, error: 0x%lx", GetLastError());
        FreeLibrary(dllHandle);
        return 1;
    }
    std::cout << "Calling foo1" << std::endl;
    func1();
    std::cout <<"Calling foo2" << std::endl;
    func2();

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

