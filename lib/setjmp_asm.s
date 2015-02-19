global save_execution_state
; save_execution_state:
;   Saves the current execution state. Used by setjmp/longjmp.
; stack: [esp + 4] Pointer to execution_state.
;        [esp]     The return address.
save_execution_state:
    ;  Push all of the fields onto the stack.
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp

    ; Now copy all this data into the execution_state pointer.
    moveb or something [esp], [esp-24], 24?


	

global restore_execution_state