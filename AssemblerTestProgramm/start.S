.section .text
.global _start
_start:
	lui x2, 0x3FF // checked, but my imm is 3ff000
	#x2: 3FF000 // to be checked!!!!!

	#                    Arithmetische Befehle

	addi x0, x2, 122 // checked
	#x0 = 0

	addi x3, x0, -4 // checked
	#x3 = FFFFFFFC

	addi x4, x3, 0x41 // checked
	#x4 = 3D

	#checked till here

	add x5, x3, x4 // checked
	#x5 = 39

	sub x6, x4, x5 // checked
	#x6 = 4

	sub x3, x3, x6 // checked
	#x3 = FFFFFFF8

	#                    Logische Befehle

	ori x7, x0, 0x43A // checked

	ori x8, x0, 0x339 // checked

	or x9, x7, x8 // checked
	#x9 = 73B

	and x10, x8, x7 // checked
	#x10 = 38

	xor x11, x8, x7 // checked
	#x11 = 703 
	
	#                    Shifts

	addi x12, x0, 4 // checked
	
	slli x12, x12, 6 // checked
	#x12 = 100
	
	srli x13, x12, 3 // checked
	#x13 = 20
	
	srli x14, x3, 2 // checked
	#x14 = 3FFFFFFE

	srai x15, x3, 2 // checked, but => # MAYBE here the ERROR : imm=0x402, shamt=0x2
	#x15 = FFFFFFFE
	
	sll x16, x12, x6 // checked 
	#x16 = 1000

	srl x17, x12, x6 // checked 
	#x17 = 10
	
	addi x18, x0, -122 // checked
	
	sra x18, x18, x12 // checked 
	#x18 = FFFFFF86
	
	#                   Data Transfer

	sw x18, 4(x0) // checked 

	sh x11, 16(x0) // checked 

	lh x19, 4(x0) // checked  // WRONG BY ME
	#x19 = FFFFFF86

	lhu x20, 4(x0) // checked 
	#x20 = FF86

	lb x21, 4(x0) // checked
	#x21 = FFFFFF86

	lbu x22, 4(x0) // checked
	#x22 = 86
	
	lh x23, 16(x0) // checked 
	#x23 =703

	lb x24, 16(x0) // checked 
	#x24 = 3

	lui x25, 5  // checked  # MAYBE HERE THE ERROR
	#x25 = 5000
	
	addi x26, x0,  'H' is 0x48 //checked   # ??? no H for me

	sb x26, 0(x25) // checked

	addi x26, x0, 'U' is 0x55 // checked 

	sb x26, 0(x25) // checked

	addi x26, x0, '\n' is 0x10//

	sb x26, 0(x25) // checked PC = 144

	# Es sollte "HU" auf der Standardausgabe erscheinen
	
	#                   Verzweigungen

	addi x27, x0, 6 // checked  PC=148

	bne_ruecksprung:
	addi x27, x27, -1 // checked PC=152
	addi x28, x28, 1 // checked PC=156

	bne x27, x0, bne_ruecksprung // checked PC=148 bis PC=160
	#x28 = 6

	addi x27, x0, 6 // checked PC=164

	blt_ruecksprung:
	addi x27, x27, -1 //checked
	addi x29, x29, 1 // checked

	blt x0, x27, blt_ruecksprung // checked PC=164 bis PC=176
	#x29 = 6	

	addi x27, x0, -5 // checked PC=180

	bge_ruecksprung:
	addi x27, x27, 1 // checked PC=184
	addi x30, x30, 1 // checked PC=188

	bge x0, x27, bge_ruecksprung // checked PC=192
	#x30 = 6 // NOT CORRECT BY ME

	addi x27, x0, 0 //  checked PC=196

    bgeu_ruecksprung:
    addi x27, x27, -1 // checked
    addi x31, x31, 1 // checked

    bgeu x27, x3, bgeu_ruecksprung // checked PC=196 bis PC=208
    #x31 = 9

    addi x27, x0, 0xe3 // checked 

    addi x1, x0, 0 // checked 

   	bltu_ruecksprung:
    addi x27, x27, -1 // checked 
    addi x1, x1, 1 // checked 

    bltu x27, x3, bltu_ruecksprung //  checked PC=216 bis 228
    #x1 = E4

#Regfile Zustand nach der Ausführung:
#x0: 0
#x1: E4
#x2: 3FF000
#x3: FFFFFFF8
#x4: 3D
#x5: 39
#x6: 4
#x7: 43A
#x8: 339
#x9: 73B
#x10: 38
#x11: 703
#x12: 100
#x13: 20
#x14: 3FFFFFFE
#x15: FFFFFFFE
#x16: 1000
#x17: 10
#x18: FFFFFF86
#x19: FFFFFF86
#x20: FF86
#x21: FFFFFF86
#x22: 86
#x23: 703
#x24: 3
#x25: 5000
#x26: A
#x27: FFFFFFFF
#x28: 6
#x29: 6
#x30: 6
#x31: 9

#---------------Wenn die Registerwerte mit ihrer Implementierung übereinstimmen, dann die nächsten beiden Zeilen kommentieren oder löschen um weitere Instruktionen auszuführen.
    # end_of_program:
	# jal x0, end_of_program
#-----------------------------------------------------------------------------------
    #-----------------------------------ab hier alle Register wieder auf 0
    add x1, x0, x0
    add x2, x0, x0
    add x3, x0, x0
    add x4, x0, x0
    add x5, x0, x0
    add x6, x0, x0
    add x7, x0, x0
    add x8, x0, x0
    add x9, x0, x0
    add x10, x0, x0
    add x11, x0, x0
    add x12, x0, x0
    add x13, x0, x0
    add x14, x0, x0
    add x15, x0, x0
    add x16, x0, x0
    add x17, x0, x0
    add x18, x0, x0
    add x19, x0, x0
    add x20, x0, x0
    add x21, x0, x0
    add x22, x0, x0
    add x23, x0, x0
    add x24, x0, x0
    add x25, x0, x0
    add x26, x0, x0
    add x27, x0, x0
    add x28, x0, x0
    add x29, x0, x0
    add x30, x0, x0
    add x31, x0, x0

	#                          Vergleiche
	addi x3, x0, 5
	addi x4, x0, 6
	slt x5, x3, x4
	#x5 = 1
	slt x6, x4, x3
	#x6 = 0
	lui x3, 0xFFFFF
	#x3 = 0xFFFFF000
	sltu x7, x3, x4
	#x7 =0
	sltu x8, x4, x3
	#x8 = 1
	slt x9, x3, x4
	#x9 = 1
	slt x10, x4, x3
	#x10 = 0
	addi x14, x0, 0x20
	#						   auipc
	auipc x11, 0xAFFE
	#x11 = AFFE180
    funct:
    addi x12, x12, 1
    beq x12, x14, ende
    #							jal
    jal x13, funct
    #x13: 190
    ende:
	#x12 = 20
	jal x1, funct2
    inf:
	addi x31, x31, 1
	jal x0, inf
	funct2:
	lui x25, 5
	#x25 = 5000
	addi x27, x0, 62
	addi x26, x0,  0x3F
	#schreibt @ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|} auf die Standardausgabe
	ausgabeRet:
	addi x27, x27, -1
	addi x26, x26, 1
	sb x26, 0(x25)
	bne x27, x0, ausgabeRet
	jalr x0, x1, 0
	#Sprung nach inf anschließend wird x31 mit jeden neuem Zyklus inkrementiert.
	end_of_program_:
	jal end_of_program_

#	Regfile Zustand nach der Ausführung:
#x0: 0
#x1: 198
#x2: 0
#x3: FFFFF000
#x4: 6
#x5: 1
#x6: 0
#x7: 0
#x8: 1
#x9: 1
#x10: 0
#x11: AFFE184
#x12: 20
#x13: 194
#x14: 20
#x15: 0
#x16: 0
#x17: 0
#x18: 0
#x19: 0
#x20: 0
#x21: 0
#x22: 0
#x23: 0
#x24: 0
#x25: 5000
#x26: 7D
#x27: 0
#x28: 0
#x29: 0
#x30: 0
#x31: z. B. C246

