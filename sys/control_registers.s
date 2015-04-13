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
	
global set_cr3

; set_cr3:
;   Set the contents of CR3, i.e. load a page directory table.
; stack: [esp + 4] The address of the page to load.
;        [esp] return address
set_cr3:
    mov eax, [esp + 4]   ; move the new PDT address into CR3.
    mov ebx, [esp]	 ; DEBUGGING: What is the return address? Guess: 0xc01033ef.
    mov cr3, eax        
    ret                  ; return to the calling function

global get_cr4

; get_cr4:
;   Return the contents of CR4.
; stack: [esp] return address
get_cr4:
    mov eax, cr4	 ; Move CR4 into AX.
    ret                  ; Return to the calling function.
	
global set_cr4

; set_cr4:
;   Set CR4.
; stack: [esp + 4] The new value.
;        [esp] return address
set_cr4:
    mov eax, [esp + 4]   ; move the new PDT address into CR4.
    mov cr4, eax
    ret                  ; return to the calling function
