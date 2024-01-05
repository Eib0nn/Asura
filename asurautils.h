#pragma once
#pragma comment (lib, "ntdll")


#include <windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <shlobj.h>
#include <tchar.h>
#include <thread>

#define __kernel_entry __allowed(on_function)
#define FILE_SUPERSEDED 0x00000000
#define FILE_OPENED 0x00000001
#define FILE_CREATED 0x00000002
#define FILE_OVERWRITTEN 0x00000003
#define FILE_EXISTS 0x00000004
#define FILE_DOES_NOT_EXIST 0x00000005

#ifdef LIB_EXPORT
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif

//------------------------ Structures ------------------------

typedef struct _PS_ATTRIBUTE {
    ULONGLONG Attribute;
    SIZE_T Size;
    union {
        ULONG_PTR Value;
        PVOID ValuePtr;
    };
    PSIZE_T ReturnLength;
} PS_ATTRIBUTE, * PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST {
    SIZE_T TotalLength;
    PS_ATTRIBUTE Attributes[2];
} PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;

//0x10 bytes (sizeof)
typedef struct _IO_STATUS_BLOCK
{
    union
    {
        LONG Status;                                                        //0x0
        VOID* Pointer;                                                      //0x0
    };
    ULONGLONG Information;                                                  //0x8
}IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;                                                           //0x0
    VOID* RootDirectory;                                                    //0x8
    struct _UNICODE_STRING* ObjectName;                                     //0x10
    ULONG Attributes;                                                       //0x18
    VOID* SecurityDescriptor;                                               //0x20
    VOID* SecurityQualityOfService;                                         //0x28
}OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;

//------------------------ Native function structures ------------------------

typedef NTSTATUS(NTAPI* NtCreateFile)(
    _Out_ PHANDLE FileHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_opt_ PLARGE_INTEGER AllocationSize,
    _In_ ULONG FileAttributes,
    _In_ ULONG ShareAccess,
    _In_ ULONG CreateDisposition,
    _In_ ULONG CreateOptions,
    _In_reads_bytes_opt_(EaLength) PVOID EaBuffer,
    _In_ ULONG EaLength
);

// ------------------------ Not Native stuff ------------------------

extern "C" LIB_API int fileCreation (const std::string &path);
extern "C" LIB_API void iterate_subdirs(const std::string &dir_path, std::vector <std::string> &dirs);

typedef void (*PIterate_sub)(const std::string &dir_path, std::vector<std::string> &dirs);
typedef int (*PFileCreation)(const std::string &path);