BITS 64
mov rax, 0x04
mov rbx, 0x01
mov rcx, rdi
add rcx, 0x08
mov rdx, rdi
syscall
ret