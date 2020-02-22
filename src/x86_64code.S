.data
	EXTRN stack_ptr: QWORD
.code

user_asm_function PROC
	;do your stuff here

	;PUSH EBP
	MOV RBP, RSP
	XOR EAX, EAX
	MOV AH, BYTE PTR[RBP + 9h]
	SHL AX, 10h
	SUB AL, BYTE PTR[RBP + 0dh]
	ADD AH, BYTE PTR[RBP + 0eh]
	XOR AX, WORD PTR[RBP + 12h]
	NOP
	;POP EBP

	;MOV RAX, 00000000000000000h;
	;MOV RBX, 01111111111111111h;
	;MOV RCX, 02222222222222222h;
	;MOV RDX, 03333333333333333h;
	;MOV RSI, 04444444444444444h;
	;MOV RDI, 05555555555555555h;
	;;MOV RSP, 06666666666666666h;
	;MOV RBP, 07777777777777777h;
	;MOV R8,  08888888888888888h;
	;MOV R9,  09999999999999999h;
	;MOV R10, 0AAAAAAAAAAAAAAAAh;
	;MOV R11, 0BBBBBBBBBBBBBBBBh;
	;MOV R12, 0CCCCCCCCCCCCCCCCh;
	;MOV R13, 0DDDDDDDDDDDDDDDDh;
	;MOV R14, 0EEEEEEEEEEEEEEEEh;
	;MOV R15, 0FFFFFFFFFFFFFFFFh;

	RET
user_asm_function ENDP

store_regs PROC
	;RCX contains address of REG_TABLE	
	MOV	RDX, [RSP - 8h]

	;store FLAGS
	PUSHFQ
	MOV R8, [RSP]	
	AND R8, 0FFFFFFFFFFFFFEFFh; clear TF flag if needed
	MOV [RCX + 80h], R8
	LEA RSP, [RSP + 8h]; MOV [RSP], RDX
	MOV [RSP - 8h], RDX; LEA RSP, [RSP - 8h]
	
	;store REGS
	MOV [RCX], RAX;
	MOV [RCX + 8h], RBX
	MOV [RCX + 10h], RCX
	MOV QWORD PTR[RCX + 18h], 0h; RDX
	MOV [RCX + 20h], RSI
	MOV [RCX + 28h], RDI
	LEA RSP, [RSP + 8h]
	MOV [RCX + 30h], RSP
	LEA RSP, [RSP - 8h]
	MOV [RCX + 38h], RBP
	MOV QWORD PTR[RCX + 40h], 0h; R8
	MOV QWORD PTR[RCX + 48h], 0h; R9
	MOV [RCX + 50h], R10
	MOV [RCX + 58h], R11
	MOV [RCX + 60h], R12
	MOV [RCX + 68h], R13
	MOV [RCX + 70h], R14
	MOV [RCX + 78h], R15

	RET	
store_regs ENDP

store_results PROC	
	MOV RSP, [stack_ptr]
	MOV [RSP + 20h], RAX
	MOV [RSP + 28h], RBX
	MOV [RSP + 30h], RCX
	MOV [RSP + 38h], RDX
	MOV [RSP + 40h], RSI
	MOV [RSP + 48h], RDI
	MOV RCX, [RSP + 18h]
	MOV [RSP + 50h], RCX; RSP points to main function
	MOV [RSP + 58h], RBP
	MOV [RSP + 60h], R8
	MOV [RSP + 68h], R9
	MOV [RSP + 70h], R10
	MOV [RSP + 78h], R11
	MOV [RSP + 80h], R12
	MOV [RSP + 88h], R13
	MOV [RSP + 90h], R14
	MOV [RSP + 98h], R15

	MOV RCX, [load_regs]
	MOV [RSP + 0A0h], RCX
	LEA RSP, [RSP + 0A0h]

	RET
store_results ENDP

load_regs PROC
	MOV RSP, [stack_ptr]
	MOV [stack_ptr], 0h; zeroing pointer

	MOV RCX, [RSP + 10h]
	MOV RBP, [RSP + 18h]

	MOV RDX, [RCX + 80h]; restoring flags
	MOV [RSP + 0A8h], RDX
	LEA RSP, [RSP + 0A8h]
	POPFQ
	
	MOV RAX, [RCX]
	MOV RBX, [RCX + 8h]
	MOV RDX, [RCX + 18h]
	MOV RSI, [RCX + 20h]
	MOV RDI, [RCX + 28h]
	MOV RSP, RBP;
	MOV RBP, [RCX + 38h]
	MOV R8,  [RCX + 40h]
	MOV R9,  [RCX + 48h]
	MOV R10, [RCX + 50h]
	MOV R11, [RCX + 58h]
	MOV R12, [RCX + 60h]
	MOV R13, [RCX + 68h]
	MOV R14, [RCX + 70h]
	MOV R15, [RCX + 78h]
	
	MOV RCX, [RCX + 10h]; RCX must be preserved until this moment

	RET
load_regs ENDP

stack_boundary_error: 
	MOV RAX, 0DEADC0DEh; TODO(SyntaX): add error handling here later	
	RET

END