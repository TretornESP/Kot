[BITS 64]

%include "Src/arch/x86-64/cpu/cpu.inc"

EXTERN  SyscallDispatch, InterruptHandler
GLOBAL	SyscallEnable

SyscallEnable:
	; Setup user gdt
	or		rsi, 0x3 ; or cs with ring 3 (userspace)
	sub		rsi, 0x10 ; or cs with ring
	; Load segments into STAR MSR
	mov		rcx, 0xc0000081
	rdmsr
	
	mov 	edx, edi
	sal 	esi, 16
	or      edx, esi
	wrmsr
	
	; Load handler RIP into LSTAR MSR
	mov		rcx, 0xc0000082
	mov		rax, SyscallEntry
	mov		rdx, rax
	shr		rdx, 32
	wrmsr

	; Setup flags for syscall
	mov		rcx, 0xC0000084
	rdmsr
	or		eax, 0xfffffffe 
	wrmsr

	; Enable syscall / sysret instruction
	mov		rcx, 0xc0000080
	rdmsr
	or		rax, 1 ; enable syscall extension
	wrmsr

	ret

SyscallEntry:
	swapgs

	mov [gs:0x10], rsp			; save userspace stack
	mov	rsp, [gs:0x8]			; setup the syscall stack
	mov	rsp, [rsp]  			; setup the stack
	push qword rbp				; save rbp
	mov rbp, [gs:0x8]			; 
	

	push qword [rbp + 0x10]    	; ss
	push qword [gs:0x10]    	; rsp
	
	sti
	push r11                	; rflags
	push qword [rbp + 0x8]    	; cs
	push rcx                	; rip
	push 0x0                	; error code
	push 0x0                	; interrupt number
	mov rbp, [rsp + 0x38]		; resotre rbp

	PUSH_REG

	mov rax, [gs:0x8] 
	mov rdi, rsp
	mov rsi, [rax + 0x18]

	call SyscallDispatch

	POP_REG

	mov r11, [rsp + 0x20]
	mov rcx, [rsp + 0x10]
	mov rsp, [rsp + 0x28]

	cli
    swapgs	 
	o64 sysret

