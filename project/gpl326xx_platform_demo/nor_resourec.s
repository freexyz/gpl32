
	PRESERVE8
	AREA  NOR_RES, DATA, READONLY, ALIGN=14

	EXPORT NorBootHeader_Start
	EXPORT NorBootHeader_End
	EXPORT NorBootLoader_Start
	EXPORT NorBootLoader_End
	EXPORT RunTimeCode_Start
	EXPORT RunTimeCode_End

NorBootHeader_Start
	INCBIN  resource\GPL326XX_NorHeader.bin
NorBootHeader_End

NorBootLoader_Start
	INCBIN  resource\GPL326XX_NorBootLoader.bin
NorBootLoader_End	

RunTimeCode_Start	
;	INCBIN  resource\RUNTIME_MINI.ROM
;	INCBIN resource\platform_code.bin
	INCBIN resource\platform_code_nor.bin
RunTimeCode_End

	END
