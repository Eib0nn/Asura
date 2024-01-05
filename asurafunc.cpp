#include "asurautils.h"

// TRANSFORM IN DLL and see how shits works lol

int fileCreation (const std::string &path){
    int i = 1;
    std::ostringstream oss;
    oss << path << "\\" << i << ".txt";
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