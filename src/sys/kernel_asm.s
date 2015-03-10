global kernel_exit
kernel_exit:
	cli			; Disable interrupts.
	hlt			; Halt the CPU until an interrupt arrives.
.kexit_loop:                    ; Loop forever.
        jmp .kexit_loop
