global get_cr2

; get_cr2:
;   Return the contents of CR2.
; stack: [esp] return address
get_cr2:
    mov eax, cr2	 ; Move CR2 into AX.
    ret                  ; Return to the calling function.
