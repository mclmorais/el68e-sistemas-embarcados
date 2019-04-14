    THUMB                          

GPIO_PORTN_AHB_DATA_BITS_R  EQU    0x40064000
	
	AREA    |.text|, CODE, READONLY, ALIGN=2
	EXPORT	frequencyMeasure

; Entrada:
; R0 -> Valor máximo da base de tempo
; Saída:
; R0 -> Medida de Frequência
frequencyMeasure
	PUSH 	{R5}
	LDR 	R1, =GPIO_PORTN_AHB_DATA_BITS_R
	ADD 	R1, R1, #8
	MOV 	R2, #0 ; timeBaseCounter = 0
	MOV 	R4, #0 ; expectedPinMeasure = false;
	MOV 	R5, #0 ; frequencyCounter = 0

newMeasure
	LDR 	R3, [R1] ; pinMeasure = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) & GPIO_PIN_1)
	CMP 	R3, R4
	ITT 	NE ; if (pinMeasure != expectedPinMeasure)
	ADDNE 	R5, #1 ; frequencyCounter++
	MOVNE 	R4, R3 ; expectedPinMeasure = pinMeasure
	ADD 	R2, R2, #1 ; timeBaseCounter++
	CMP 	R2, R0 ; timeBaseCounter < timerBaseMax
	BLT 	newMeasure
	MOV 	R0, R5
	POP 	{R5}
	BX 		LR
	
	ALIGN
	END