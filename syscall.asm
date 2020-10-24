_text segment

SysQueryCompositionSurfaceStatistics proc
mov r10,rcx
mov eax,1348h
syscall
ret
SysQueryCompositionSurfaceStatistics endp

_text ends
end
