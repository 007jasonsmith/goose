; Global descriptor table definition. 
extern global_descriptor_table_ptr

global gdt_flush
gdt_flush:
    lgdt [global_descriptor_table_ptr]
    mov ax, 0x10 		; 0x10 is the offset to the data segment.
    mov ds, ax	
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:gdt_flush2		; 0x08 is the offset to the code segment.
	                        ; Execute a far jump, using the new gdt.  
gdt_flush2:
    ret