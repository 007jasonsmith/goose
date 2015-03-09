; Interrupt descriptor table definition.
extern interrupt_descriptor_table_ptr
	
global idt_load
idt_load:
    lidt [interrupt_descriptor_table_ptr]
    ret
