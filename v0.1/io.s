global outb

; outb:
;   Send a byte to an I/O port.
; stack: [esp + 8] The data byte
;        [esp + 4] The I/O port
;        [esp] return address
outb:
    mov al, [esp + 8]    ; move the data to be sent into the al register
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    out dx, al           ; send the data to the I/O port
    ret                  ; return to the calling function


global inb

; inb:
;  Returns a byte from the given I/O port.
; stack: [esp + 4] The address of the I/O port
;        [esp    ] The return address
inb:
    mov dx, [esp + 4]       ; move the address of the I/O port to the dx register
    in  al, dx              ; read a byte from the I/O port and store it in al.
    ret                     ; return the read byte	
