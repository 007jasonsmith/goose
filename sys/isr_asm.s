; We define two macros for simplifying the code for all known interrupt
; service routines. These macros come in two flavors, depending on whether
; or not an error code is included with the interrupt.

extern interrupt_handler

; Defines an interrupt handler not taking an error code.
%macro no_error_code_interrupt_handler 1
    global interrupt_handler_%1
    interrupt_handler_%1:
        push    dword 0                     ; Default to 0 error code.
        push    dword %1                    ; Interrupt number.
        jmp     common_interrupt_handler
%endmacro

; Defines an interrupt handler which takes an error code.
%macro error_code_interrupt_handler 1
    global interrupt_handler_%1
    interrupt_handler_%1:
        push    dword %1                    ; Interrupt number.
        jmp     common_interrupt_handler
%endmacro

; Common handler for interrupts. Handles saving the stack, etc.
; TODO(chris): Double check register state, etc.
common_interrupt_handler:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp   ; Push us the stack
    push eax

    mov eax, interrupt_handler
    call eax       ; A special call, preserves the 'eip' register

    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

	

; Defines known/handled interrupts. For more information such as what these
; actually respond to, see isr.c.
no_error_code_interrupt_handler 0
