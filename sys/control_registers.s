global get_cr2

; get_cr2:
;   Return the contents of CR2.
; stack: [esp] return address
get_cr2:
    mov eax, cr2	 ; Move CR2 into AX.
    ret                  ; Return to the calling function.

global get_cr3
; get_cr3:
;   Return the contents of CR3.
; stack: [esp] return address
get_cr3:
    mov eax, cr3	 ; Move CR3 into AX.
    ret                  ; Return to the calling function.
	
global load_pdt

; load_pdt:
;   Load a page directory table.
; stack: [esp + 4] The address of the page to load.
;        [esp] return address
load_pdt:
    mov eax, [esp + 4]   ; move the new PDT address into CR3.
    mov cr3, eax        
    ret                  ; return to the calling function
