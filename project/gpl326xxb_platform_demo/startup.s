    INCLUDE arm.inc
 
    IMPORT 	|Load$$ROM1$$Base|				; Read-only region
    IMPORT 	|Image$$ROM1$$Base|
    IMPORT 	|Image$$ROM1$$Length|
    IMPORT 	|Load$$RAM1$$Base|				; Read-write region
    IMPORT 	|Image$$RAM1$$Base|
    IMPORT 	|Image$$RAM1$$Length|
    IMPORT 	|Image$$RAM1$$ZI$$Base|			; Zero initiated region
    IMPORT 	|Image$$RAM1$$ZI$$Length|
    IMPORT 	|Load$$ROM2$$Base|				; Read-only region
    IMPORT 	|Image$$ROM2$$Base|
    IMPORT 	|Image$$ROM2$$Length|
    IMPORT 	|Load$$ROM3$$Base|				; Read-only region
    IMPORT 	|Image$$ROM3$$Base|
    IMPORT 	|Image$$ROM3$$Length|
    IMPORT 	|Load$$PWR$$Base|				; Read-only region
    IMPORT 	|Image$$PWR$$Base|
    IMPORT 	|Image$$PWR$$Length|

	IMPORT  Main							; The entry of C program 
	IMPORT  system_power_on_ctrl

    EXPORT  pHandler_Undef
    EXPORT  pHandler_SWI
    EXPORT  pHandler_PAbort
    EXPORT  pHandler_DAbort
    EXPORT  pHandler_IRQ
    EXPORT  pHandler_FIQ

    EXPORT  SoftReset

    EXPORT	NorWakeupVector
    EXPORT	Wakeup

IRQ_STACK_SIZE		EQU		0x100
FIQ_STACK_SIZE		EQU		0x100
ABT_STACK_SIZE		EQU		0x20
UND_STACK_SIZE		EQU		0x20
SYS_STACK_SIZE		EQU		0x20
SVC_STACK_SIZE		EQU		0x2000

P_CACHE_CTRL		EQU     0xFF000000
P_INT_GMASK			EQU     0xD0100038
P_DMA0_CTRL			EQU     0xD0300000
P_DMA1_CTRL			EQU     0xD0300040
P_DMA2_CTRL			EQU     0xD0300080
P_DMA3_CTRL			EQU     0xD03000C0
P_PPU_ENABLE		EQU     0xD05001FC
P_JPEG_CTRL			EQU     0xD07023C0
P_KECON		        EQU     0xD0100020
P_NF_CTRL           EQU     0xD0900140
P_IOF_O_DATA        EQU	    0xC00000A4

;======================================================================================================
    AREA    StartUp, CODE, READONLY
    CODE32
	ENTRY
    ;==========================================================
    ; For LPC2210, exception vector in 0x80000000 will be remap to 0x00000000 (64 bytes).
    ; We need to store the addresses of exception handlers in the first 64 bytes.
    ; That's why LDR and DCD are used here. 
    ; If flash is located at 0x00000000 and remap is not necessary, use Branch instruction instead. 
    ;==========================================================	
SoftReset
    LDR		PC, Addr_Reset
    LDR		PC, Addr_Undef
    LDR		PC, Addr_SWI	
    LDR		PC, Addr_PAbort
    LDR		PC, Addr_DAbort	
    B 		.								; Reserved
    LDR		PC, Addr_IRQ
    LDR		PC, Addr_FIQ

Addr_Reset	DCD		Handle_Reset
Addr_Undef	DCD		Handle_Undef
Addr_SWI	DCD		Handle_SWI
Addr_PAbort	DCD		Handle_PAbort
Addr_DAbort	DCD		Handle_DAbort
Addr_IRQ	DCD		Handle_IRQ
Addr_FIQ	DCD		Handle_FIQ
    
    
;======================================================================================================
Handle_Reset
    ;==========================================================
    ; (1) Make sure ARM is in SVC mode with interrupt disabled after software reset
    ;==========================================================
	MSR     CPSR_cxsf, #(INT_MASK | MODE_SVC)

    ;==========================================================
    ; (2) User marco definition to do basic initiation to MCU special registers
    ;==========================================================
    LDR     R0, =P_CACHE_CTRL
    LDR     R1, =0x0
    STR     R1, [R0]

    LDR		R0, =P_INT_GMASK
    LDR		R1, =0x1
    STR     R1, [R0]
    
    LDR		R0, =P_DMA0_CTRL
    LDR		R1, =0x200
    STR     R1, [R0]
    LDR		R0, =P_DMA1_CTRL
    STR     R1, [R0]
    LDR		R0, =P_DMA2_CTRL
    STR     R1, [R0]
    LDR		R0, =P_DMA3_CTRL
    STR     R1, [R0]     

    LDR     R0, =P_PPU_ENABLE
    LDR     R1, =0x0
    STR     R1, [R0]
    
    LDR     R0, =P_JPEG_CTRL
    LDR     R1, =0x0
    STR     R1, [R0]
    
    LDR     R0, =P_IOF_O_DATA
    LDR     R1, [R0] 
    ORR     R1, R1, #0x2
    STR     R1, [R0]
    
    LDR     R0, =P_NF_CTRL
    LDR     R1, =0x0
    STR     R1, [R0]
    
	;==========================================================
	; (3.1) Copy code and read-only data to execution address
	;==========================================================
    LDR     R1, =|Load$$ROM1$$Base|
    LDR     R2, =|Image$$ROM1$$Base|
    LDR     R3, =|Image$$ROM1$$Length|
	CMP		R1, R2
	BEQ		%F2								; If load address is equal to execution address, movement is not needed
    CMP     R3, #0x0
    BLE     %F2
1
    LDR     R4, [R1], #4
    STR     R4, [R2], #4
    SUBS    R3, R3, #4
    BHI     %B1
2
	LDR     R0, =P_KECON
	LDR     R5, [R0]
	TST     R5, #4
	BNE     %F26
	LDR     R1, =|Load$$ROM2$$Base|
    LDR     R2, =|Image$$ROM2$$Base|
    LDR     R3, =|Image$$ROM2$$Length|
	CMP		R1, R2
	BEQ		%F22								; If load address is equal to execution address, movement is not needed
    CMP     R3, #0x0
    BLE     %F22
21
    LDR     R4, [R1], #4
    STR     R4, [R2], #4
    SUBS    R3, R3, #4
    BHI     %B21

22
	LDR     R1, =|Load$$ROM3$$Base|
    LDR     R2, =|Image$$ROM3$$Base|
    LDR     R3, =|Image$$ROM3$$Length|
	CMP		R1, R2
	BEQ		%F24								; If load address is equal to execution address, movement is not needed
    CMP     R3, #0x0
    BLE     %F24
23
    LDR     R4, [R1], #4
    STR     R4, [R2], #4
    SUBS    R3, R3, #4
    BHI     %B23
    
24
	LDR     R1, =|Load$$PWR$$Base|
    LDR     R2, =|Image$$PWR$$Base|
    LDR     R3, =|Image$$PWR$$Length|
	CMP		R1, R2
	BEQ		%F26								; If load address is equal to execution address, movement is not needed
    CMP     R3, #0x0
    BLE     %F26
25
    LDR     R4, [R1], #4
    STR     R4, [R2], #4
    SUBS    R3, R3, #4
    BHI     %B25
    
26
	;==========================================================
	; (3.2) Copy read-write data to execution address
	;==========================================================
    LDR     R1, =|Load$$RAM1$$Base|
    LDR     R2, =|Image$$RAM1$$Base|
    LDR     R3, =|Image$$RAM1$$Length|
	CMP		R1, R2
	BEQ		%F4
    CMP     R3, #0x0
    BLE     %F4    
3
    LDR     R4, [R1], #4
    STR     R4, [R2], #4
    SUBS    R3, R3, #4
    BHI     %B3       
4
	;==========================================================
	; (3.3) Setup zero initiated global variables
	;==========================================================
    LDR     R2, =|Image$$RAM1$$ZI$$Base|
    LDR     R3, =|Image$$RAM1$$ZI$$Length|
    CMP     R3, #0x0
    BLE     PrepareImageEnd 

    MOV     R4, #0x0
5
    STR     R4, [R2], #4
    SUBS    R3, R3, #4  
    BHI     %B5        
PrepareImageEnd      
        
    ;==========================================================
    ; (4) Initialize stack
    ; Note: R2 point to top of free memory
    ;==========================================================
    MSR     CPSR_c, #(INT_MASK | MODE_IRQ)
    LDR		R1, =(IRQ_STACK_SIZE + 3)
    ADD		R2, R2, R1
    BIC		R2, R2, #3						; Make sure it is word alignment
    MOV     SP, R2

    MSR     CPSR_c, #(INT_MASK | MODE_FIQ)
    LDR		R1, =(FIQ_STACK_SIZE + 3)
    ADD		R2, R2, R1
    BIC		R2, R2, #3
    MOV     SP, R2

    MSR     CPSR_c, #(INT_MASK | MODE_ABT)
    LDR		R1, =(ABT_STACK_SIZE + 3)
    ADD		R2, R2, R1
    BIC		R2, R2, #3
    MOV     SP, R2

    MSR     CPSR_c, #(INT_MASK | MODE_UND)
    LDR		R1, =(UND_STACK_SIZE + 3)
    ADD		R2, R2, R1
    BIC		R2, R2, #3
    MOV     SP, R2
	
    MSR     CPSR_c, #(INT_MASK | MODE_SYS)	; User mode and System mode use the same register set
    LDR		R1, =(SYS_STACK_SIZE + 3)
    ADD		R2, R2, R1
    BIC		R2, R2, #3
    MOV     SP, R2	
	
    MSR     CPSR_c, #(INT_MASK | MODE_SVC)
    LDR		R1, =(SVC_STACK_SIZE + 3)
    ADD		R2, R2, R1
    BIC		R2, R2, #3
    MOV     SP, R2

	ADD		R2, R2, #4						; This is top of heap memory
    STMFD	SP!, {R2}						; Store in SVC stack

	;==========================================================
    ; (5) We are done here. Byebye assemble code.
    ;==========================================================    
	LDMFD	SP!, {R0}						; Pop out heap memory address and pass it to C program
    B		Main	    					; Branch to Main C function


Handle_Undef
    SUB		R0, LR, #4
    LDR	    R1, =pHandler_Undef
    LDR	    PC, [R1]

    
Handle_SWI
    MOV		R0, LR
    LDR	    R1, =pHandler_SWI
    LDR	    PC, [R1]


Handle_PAbort
    SUB		R0, LR, #4
    LDR	    R1, =pHandler_PAbort
    LDR	    PC, [R1]

        
Handle_DAbort
    SUB		R0, LR, #8
    LDR	    R1, =pHandler_DAbort
    LDR	    PC, [R1]


Handle_IRQ
    SUB	    SP, SP, #4						; Decrement SP
    STMFD   SP!, {R0}						; Store R0

    MRS		R0, SPSR						; Test IRQ disable bit to see if it is a spurious interrupt
    TST		R0, #INT_IRQ
    BNE		SpuriousInterrupt
        
    LDR	    R0, =pHandler_IRQ				; Load the address of HandleXXX to R0
    LDR	    R0, [R0]						; Load the service routine start address of HandleXXX
    STR	    R0, [SP, #4]
    LDMFD   SP!, {R0, PC}					; Resotre R0 and load ISR to PC

SpuriousInterrupt
    SUB		R0, LR, #4						; Return immediately if it is a spurious interrupt
    STR		R0, [SP, #4]
    LDMFD	SP!, {R0, PC}^
    

Handle_FIQ
    SUB	    SP, SP, #4
    STMFD   SP!, {R0}
    LDR	    R0, =pHandler_FIQ
    LDR	    R0, [R0]
    STR	    R0, [SP, #4]
    LDMFD   SP!, {R0, PC}
 
    AREA    NorWakeupVector, CODE, READONLY
    CODE32
	LDR		PC,=0x10001A04
    LDR		PC,=0x10001A08
    LDR		PC,=0x10001A0C
    LDR		PC,=0x10001A10	
    LDR		PC,=0x10001A18
    LDR		PC,=0x10001A1C
    LDR		PC,=0xF8000FA0

	AREA    Wakeup, CODE, READONLY
    CODE32
    LDR     R0, =system_power_on_ctrl
	MOV     LR, PC
	BX      R0
	LDR     R0, =0xBBF
	BX      R0 ;GPL326xx
   
	;MOV     R2,#0
    ;MOV     PC,R2
 
	LTORG
	
;======================================================================================================

	ALIGN
	AREA ExceptionHandler, DATA, READWRITE

pHandler_Undef		SPACE	4
pHandler_SWI		SPACE	4
pHandler_PAbort		SPACE	4
pHandler_DAbort		SPACE	4
pHandler_IRQ		SPACE	4
pHandler_FIQ		SPACE	4


    END
