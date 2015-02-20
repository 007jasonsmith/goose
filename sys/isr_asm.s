; We define two macros for simplifying the code for all known interrupt
; service routines. These macros come in two flavors, depending on whether
; or not an error code is included with the interrupt.

extern interrupt_handler
extern irq_handler
	
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

; Defines an IRQ handler. It is identical to an interrupt handler, except
; we call a different function (irq_handler instead of interrupt_handler).
%macro irq_handler 1
    global irq_handler_%1
    irq_handler_%1:
        push    dword 0                     ; Default to 0 error code.
        push    dword %1                    ; Interrupt number.
        jmp     common_irq_handler
%endmacro
	
common_irq_handler:
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

    mov eax, irq_handler
    call eax       ; A special call, preserves the 'eip' register

    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
	
; The first 32 interrupts are reserved for hardware exceptions. 
no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler 8
no_error_code_interrupt_handler 9
error_code_interrupt_handler 10
error_code_interrupt_handler 11
error_code_interrupt_handler 12
error_code_interrupt_handler 13
error_code_interrupt_handler 14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
no_error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
no_error_code_interrupt_handler 30
no_error_code_interrupt_handler 31

; We will use the next 16 for handling IRQs.
irq_handler 32
irq_handler 33
irq_handler 34
irq_handler 35
irq_handler 36
irq_handler 37
irq_handler 38
irq_handler 39
irq_handler 40
irq_handler 41
irq_handler 42
irq_handler 43
irq_handler 44
irq_handler 45
irq_handler 46
irq_handler 47
