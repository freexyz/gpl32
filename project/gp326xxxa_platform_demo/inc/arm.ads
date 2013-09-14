
INT_FIQ     EQU     0x40                 	; FIQ disable bit
INT_IRQ     EQU     0x80                 	; IRQ disable bit
INT_MASK    EQU     0xC0					; Interrupt disable mask (Both FIR and IRQ)
MODE_USR   	EQU		0x10
MODE_FIQ  	EQU     0x11
MODE_IRQ  	EQU     0x12
MODE_SVC  	EQU     0x13
MODE_ABT	EQU		0x17
MODE_UND  	EQU		0x1B
MODE_SYS   	EQU		0x1F
MODE_MASK   EQU		0x1F

	END