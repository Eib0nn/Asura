#include "asurautils.h"
#include "eibon_no_sho.h"

//*************************************************************************************************************
//*Gexis{                                                                                                     *
//*      "This place exists between dream and reality, mind and matter...", Igor (Persona series, Velvet Room)*
//*}                                                                                                          *
//*************************************************************************************************************


int main(int argc, char *argv[])
{


    if (argc < 2)
    {
        printf("usage: \"%s\" PID", argv[0]);
        return EXIT_FAILURE;
    }
    dll_injection(atoi(argv[1]));

    /*
    WCHAR DLL[MAX_PATH] = L"C:\\Users\\thirras\\Desktop\\kishin\\Asura\\AsuraDll.dll";
    SIZE_T DllPathSize = sizeof(DLL);
    std::string path_dot = "C:\\Users\\thirras\\Desktop\\DANGEROUSTEST";
    printf("Calling iteration_subdirs...\n");
    //thfileCreation(path_dot);
    std::vector<std::string> dirs;
    //thiterate_subdirs(path_dot, dirs);
    */
    //FreeLibrary(dllHandle);
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


