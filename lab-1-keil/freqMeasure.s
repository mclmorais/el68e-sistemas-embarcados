    THUMB                          

GPIO_PORTN_AHB_DATA_BITS_R  EQU    0x40064000
	
	AREA    |.text|, CODE, READONLY, ALIGN=2
	EXPORT	frequencyMeasure

; Entrada:
; R0 -> Valor m�ximo da base de tempo
; Sa�da:
; R0 -> Medida de Frequ�ncia
frequencyMeasure
	PUSH 	{R5}
	LDR 	R1, =GPIO_PORTN_AHB_DATA_BITS_R
	ADD 	R1, R1, #8
	MOV 	R2, R0 ; timeBaseCounter = MAX_TIME_COUNTER
	MOV 	R4, #0 ; expectedPinMeasure = false;
	MOV 	R5, #0 ; frequencyCounter = 0

	; R0: Tamanho da base de tempo
	; R1: Endereço do GPIO
	; R2: iterador da base de tempo
	; R3: valor medido do GPIO
	; R4: valor esperado do GPIO
	; R5: contador de frequencia

newMeasure
	LDR 		R3, [R1] 			; Read GPIO
	CMP 		R3, R4				
	ITT 		NE 						; if (pinMeasure != expectedPinMeasure)
	ADDNE 	R5, #1 				; frequencyCounter++
	MOVNE 	R4, R3 				; expectedPinMeasure = pinMeasure
	SUBS 		R2, R2, #1 		; timeBaseCounter++
	BPL 		newMeasure
	MOV 		R0, R5
	POP 		{R5}
	BX 			LR
	
	ALIGN
	END