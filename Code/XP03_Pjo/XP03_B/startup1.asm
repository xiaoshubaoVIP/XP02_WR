;;--------------------------------------------------------------;;
;;    This file is part of the Holtek C Compiler V3 package     ;;
;;    For the initialization of static linkage variables        ;;
;;    Copyright (C) 2017 Holtek Semiconductor Inc.              ;;
;;    Version: 1.07 (Above IDE3000 V7.94)                       ;;
;;    Date:    2017/03/06                                        ;;
;;--------------------------------------------------------------;;

acc equ [05h]
mp equ [01h]
iar equ [00h]
bp equ [04h]
z equ [0ah].2
c equ [0ah].0
public _main_startup
@start   .SECTION 'CODE'
_main_startup:
 ;;	CLR WDT2    ;;PD,TO flag will be clear. This line can be removed if a small number of global variables in C program are used.
	Mov a, offset __initial_value_end
	dec acc
	MOV mp,A
	clr c
	Sub a, offset __initial_value_begin
	snz c
	jmp startupend1
L0004:
	mov ra,a
	CALL romdata_base1
	MOV iar,A
	dec mp
  ;;	CLR WDT   ;;PD,TO flag will be clear. This line can be removed if a small number of global variables in C program are used.
	deca ra
	sz ra
	jmp L0004 
startupend1:
ifndef Disable_Bit_Initial
	MOV A,offset bitdatasec1_end
	mov mp,A
	dec mp
	clr z
	sub a,offset bitdatasec1_start
	sz z
	jmp startupend2
L0005:
	set iar
	dec mp
	sdz  acc
	jmp L0005

startupend2:
	MOV A,offset bitdatasec0_end
	mov mp,A
	dec mp
	clr z
	sub a,offset bitdatasec0_start
	sz z
	jmp startupend3
L0006:
	clr iar
	dec mp
	sdz  acc
	jmp L0006
		
startupend3:
endif
ROMBANK 0 @ROMDATA_BASE 
@ROMDATA_BASE .SECTION  inpage com_l 'CODE'
romdata_base1:
	ADDM A,[06H]

@HCCINIT   .SECTION  COM_L 'DATA'
__initial_value_begin:
@HCCINIT  .SECTION COM_E 'DATA'
__initial_value_end:


@BITDATASEC1 .SECTION com_l 'DATA'  
bitdatasec1_start:

@BITDATASEC1 .SECTION com_e 'DATA'  
bitdatasec1_end:

@BITDATASEC .SECTION com_l 'DATA'  
startup_value:
bitdatasec0_start:

@BITDATASEC .SECTION com_e 'DATA'  
bitdatasec0_end:

extern ra:byte