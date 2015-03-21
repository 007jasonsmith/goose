global system_halt
system_halt:
	cli			; Disable interrupts.
	hlt			; Halt the CPU until an interrupt arrives.
.system_halt_loop:              ; Loop forever.
        jmp .system_halt_loop
