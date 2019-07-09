BITS 64
mov rax, 0x04
mov rbx, [rdi]
mov rcx, rdi
add rcx, 16
mov rdx, [rdi+8]
syscall
ret