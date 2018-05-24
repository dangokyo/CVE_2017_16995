#include<stdio.h>
#include<stdlib.h>

//OPCODE class
#define BPF_ALU 0x4
#define BPF_ALU64 0x7
#define BPF_JMP 0x5
#define BPF_STX 0x3
#define BPF_ST 0x2
#define BPF_LDX 0x1
#define BPF_LD 0x0


//ALU operation
#define BPF_ADD 0x00
#define BPF_SUB 0x10
#define BPF_AND 0x50
#define BPF_OR  0x40
#define BPF_LSH 0x60
#define BPF_RSH 0x70
#define BPF_XOR 0xa0
#define BPF_MUL 0x20
#define BPF_MOV 0xb0
#define BPF_DIV 0x30
#define BPF_MOD 0x90

//JMP Type
#define BPF_CALL 0x80
#define BPF_JA 0x00
#define BPF_JEQ 0x10
#define BPF_JNE 0x50
#define BPF_JGT 0x20
#define BPF_JGE 0x30
#define BPF_JSGT 0x60
#define BPF_JSGE 0x70
#define BPF_JSET 0x40
#define BPF_EXIT 0x90

//LD/ST OP
#define BPF_MEM 0x60
#define BPF_XADD 0xc0
#define BPF_ABS 0x20
#define BPF_IND 0x40
#define BPF_IMM 0x00

//LD/ST data length
#define BPF_B 0x00
#define BPF_H 0x08
#define BPF_W 0x10
#define BPF_DW 0x18

#define BPF_OPCLASS(code) (code & 0x7)
#define ALUOP(code) (code & 0xf0)
#define JUMPType(code) (code & 0xf0)
#define LDSTOP(code) (code & 0xe0)
#define LDSTSIZE(code) (code & 0x18)

char *prog  = 	"\xb4\x09\x00\x00\xff\xff\xff\xff"
		"\x55\x09\x02\x00\xff\xff\xff\xff"
		"\xb7\x00\x00\x00\x00\x00\x00\x00"
		"\x95\x00\x00\x00\x00\x00\x00\x00"
		"\x18\x19\x00\x00\x03\x00\x00\x00"
		"\x00\x00\x00\x00\x00\x00\x00\x00"
		"\xbf\x91\x00\x00\x00\x00\x00\x00"
		"\xbf\xa2\x00\x00\x00\x00\x00\x00"
		"\x07\x02\x00\x00\xfc\xff\xff\xff"
		"\x62\x0a\xfc\xff\x00\x00\x00\x00"
		"\x85\x00\x00\x00\x01\x00\x00\x00"
		"\x55\x00\x01\x00\x00\x00\x00\x00"
		"\x95\x00\x00\x00\x00\x00\x00\x00"
		"\x79\x06\x00\x00\x00\x00\x00\x00"
		"\xbf\x91\x00\x00\x00\x00\x00\x00"
		"\xbf\xa2\x00\x00\x00\x00\x00\x00"
		"\x07\x02\x00\x00\xfc\xff\xff\xff"
		"\x62\x0a\xfc\xff\x01\x00\x00\x00"
		"\x85\x00\x00\x00\x01\x00\x00\x00"
		"\x55\x00\x01\x00\x00\x00\x00\x00"
		"\x95\x00\x00\x00\x00\x00\x00\x00"
		"\x79\x07\x00\x00\x00\x00\x00\x00"
		"\xbf\x91\x00\x00\x00\x00\x00\x00"
		"\xbf\xa2\x00\x00\x00\x00\x00\x00"
		"\x07\x02\x00\x00\xfc\xff\xff\xff"
		"\x62\x0a\xfc\xff\x02\x00\x00\x00"
		"\x85\x00\x00\x00\x01\x00\x00\x00"
		"\x55\x00\x01\x00\x00\x00\x00\x00"
		"\x95\x00\x00\x00\x00\x00\x00\x00"
		"\x79\x08\x00\x00\x00\x00\x00\x00"
		"\xbf\x02\x00\x00\x00\x00\x00\x00"
		"\xb7\x00\x00\x00\x00\x00\x00\x00"
		"\x55\x06\x03\x00\x00\x00\x00\x00"
		"\x79\x73\x00\x00\x00\x00\x00\x00"
		"\x7b\x32\x00\x00\x00\x00\x00\x00"
		"\x95\x00\x00\x00\x00\x00\x00\x00"
		"\x55\x06\x02\x00\x01\x00\x00\x00"
		"\x7b\xa2\x00\x00\x00\x00\x00\x00"
		"\x95\x00\x00\x00\x00\x00\x00\x00"
		"\x7b\x87\x00\x00\x00\x00\x00\x00"
		"\x95\x00\x00\x00\x00\x00\x00\x00";

int curInst = 0;

int getOffset()
{
	int v1, v2, ans;
	v1 = (int)(prog[curInst*8 + 2]);
	v2 = (int)(prog[curInst*8 + 3]);

	ans = (v1) | (v2<<8);
	return ans;
}

int getDstReg()
{
	int regInfo = (int)(prog[curInst*8 + 1]);
	int dstReg = regInfo & 0xf;
	return dstReg;
}

int getSrcReg()
{
	int regInfo = (int)(prog[curInst*8 + 1]);
	int srcReg = (regInfo >> 4) & 0xf;
	return srcReg;
}

unsigned int get32ImmediateValue()
{
	int v1, v2, v3, v4;
	unsigned int ans;
	v1 = (int)(prog[curInst*8 + 4]);
	v2 = (int)(prog[curInst*8 + 5]);
	v3 = (int)(prog[curInst*8 + 6]);
	v4 = (int)(prog[curInst*8 + 7]);

	ans = (v1) | (v2 << 8) | (v3 << 16) | (v4 << 24);
	return ans;
}

int printJMPKInfo()
{
	int jumpOffset = getOffset();
	int dstReg = getDstReg();
	int imm = get32ImmediateValue();
	printf("dstReg[%d] compare with imm (0x%x), Jump Offset: %d", dstReg, imm, jumpOffset);
	return 0;
}

int printJMPXInfo()
{
	int jumpOffset = getOffset();
	int dstReg = getDstReg();
	int srcReg = getSrcReg();
	printf(" dstReg[%d] compare with srcReg[%d], Jump Offset: %d", dstReg, srcReg, jumpOffset);
	return 0;
}

int processJMPInst(int opcode)
{
	int offset = getOffset();
	unsigned int imm = get32ImmediateValue();
	if(opcode & 0x8) //X
	{
		switch(JUMPType(opcode))
		{
			case BPF_CALL:
				printf("|BPF_CALL|BPF_X: ");
				printf("offset 0x%x", offset);
				break;
			case BPF_JA:
				printf("|BPF_JA|BPF_X: ");
				printJMPXInfo();
				break;
			case BPF_JEQ:
				printf("|BPF_JEQ|BPF_X: ");
				printJMPXInfo();
				break;
			case BPF_JNE:
				printf("|BPF_JNE|BPF_X: ");
				printJMPXInfo();
				break;
			case BPF_JGT:
				printf("|BPF_JGT|BPF_X: ");
				printJMPXInfo();
				break;
			case BPF_JGE:
				printf("|BPF_JGE|BPF_X: ");
				printJMPXInfo();
				break;
			case BPF_JSGT:
				printf("|BPF_JSGT|BPF_X: ");
				printJMPXInfo();
				break;
			case BPF_JSGE:
				printf("|BPF_JSGE|BPF_X: ");
				printJMPXInfo();
				break;
			case BPF_JSET:
				printf("|BPF_JSET|BPF_X: ");
				printJMPXInfo();
				break;
			case BPF_EXIT:
				printf("|BPF_EXIT|BPF_X");
				break;
			default:
				break;
		}

	}
	else //K
	{
		switch(JUMPType(opcode))
		{
			case BPF_CALL:
				printf("|BPF_CALL|BPF_K: ");
				printf("offset 0x%x", offset);
				break;
			case BPF_JA:
				printf("|BPF_JA|BPF_K: ");
				printJMPKInfo();
				break;
			case BPF_JEQ:
				printf("|BPF_JEQ|BPF_K: ");
				printJMPKInfo();
				break;
			case BPF_JNE:
				printf("|BPF_JNE|BPF_K: ");
				printJMPKInfo();
				break;
			case BPF_JGT:
				printf("|BPF_JGT|BPF_K: ");
				printJMPKInfo();
				break;
			case BPF_JGE:
				printf("|BPF_JGE|BPF_K: ");
				printJMPKInfo();
				break;
			case BPF_JSGT:
				printf("|BPF_JSGT|BPF_K: ");
				printJMPKInfo();
				break;
			case BPF_JSGE:
				printf("|BPF_JSGE|BPF_K: ");
				printJMPKInfo();
				break;
			case BPF_JSET:
				printf("|BPF_JSET|BPF_K: ");
				printJMPKInfo();
				break;
			case BPF_EXIT:
				printf("|BPF_EXIT|BPF_K");
				break;
			default:
				break;
		}
	}
	return 0;
}

int processALUXInst()
{
	int dstReg, srcReg;
	//int regInfo = (int)(prog[curInst*8 + 1]);
	dstReg = getDstReg();
	srcReg = getSrcReg();;
	printf("dstRegNum: %d, srcRegNum: %d", dstReg, srcReg);
	return 0;
}

int processALUKInst()
{
	int dstReg;
	unsigned int imm;
	//int regInfo = (int)(prog[curInst*8 + 1]);
	dstReg = getDstReg();
	imm = get32ImmediateValue();
	printf("dstRegNum: %d, IMM: %d", dstReg, imm);
	return 0;
}

int processALUInst(int opcode)
{
	if(opcode & 0x8) //X
	{
		switch(ALUOP(opcode))
		{
			case BPF_ADD:
				printf("|BPF_ADD|BPF_X:");
				break;
			case BPF_SUB:
				printf("|BPF_SUB|BPF_X:");
				break;
			case BPF_AND:
				printf("|BPF_AND|BPF_X:");
				break;
			case BPF_OR:
				printf("|BPF_OR|BPF_X:");
				break;
			case BPF_LSH:
				printf("|BPF_LSH|BPF_X:");
				break;
			case BPF_RSH:
				printf("|BPF_RSH|BPF_X:");
				break;
			case BPF_XOR:
				printf("|BPF_XOR|BPF_X:");
				break;
			case BPF_MUL:
				printf("|BPF_MUL|BPF_X:");
				break;
			case BPF_MOV:
				printf("|BPF_MOV|BPF_X:");
				break;
			case BPF_DIV:
				printf("|BPF_DIV|BPF_X:");
				break;
			case BPF_MOD:
				printf("|BPF_MOD|BPF_X:");
				break;
			default:
				printf("Unknown operation");
				break;
		}
		processALUXInst();
	}
	else   //K
	{
		switch(ALUOP(opcode))
		{
			case BPF_ADD:
				printf("|BPF_ADD|BPF_K:");
				break;
			case BPF_SUB:
				printf("|BPF_SUB|BPF_K:");
				break;
			case BPF_AND:
				printf("|BPF_AND|BPF_K:");
				break;
			case BPF_OR:
				printf("|BPF_OR|BPF_K:");
				break;
			case BPF_LSH:
				printf("|BPF_LSH|BPF_K:");
				break;
			case BPF_RSH:
				printf("|BPF_RSH|BPF_K:");
				break;
			case BPF_XOR:
				printf("|BPF_XOR|BPF_K:");
				break;
			case BPF_MUL:
				printf("|BPF_MUL|BPF_K:");
				break;
			case BPF_MOV:
				printf("|BPF_MOV|BPF_K:");
				break;
			case BPF_DIV:
				printf("|BPF_DIV|BPF_K:");
				break;
			case BPF_MOD:
				printf("|BPF_MOD|BPF_K:");
				break;
			default:
				printf("Unknown operation");
				break;
		}
		processALUKInst();
	}
	return 0;
}




int processALU64Inst(int opcode)
{
	if(opcode & 0x8) //X
	{
		switch(ALUOP(opcode))
		{
			case BPF_ADD:
				printf("|BPF_ADD|BPF_X:");
				break;
			case BPF_SUB:
				printf("|BPF_SUB|BPF_X:");
				break;
			case BPF_AND:
				printf("|BPF_AND|BPF_X:");
				break;
			case BPF_OR:
				printf("|BPF_OR|BPF_X:");
				break;
			case BPF_LSH:
				printf("|BPF_LSH|BPF_X:");
				break;
			case BPF_RSH:
				printf("|BPF_RSH|BPF_X:");
				break;
			case BPF_XOR:
				printf("|BPF_XOR|BPF_X:");
				break;
			case BPF_MUL:
				printf("|BPF_MUL|BPF_X:");
				break;
			case BPF_MOV:
				printf("|BPF_MOV|BPF_X:");
				break;
			case BPF_DIV:
				printf("|BPF_DIV|BPF_X:");
				break;
			case BPF_MOD:
				printf("|BPF_MOD|BPF_X:");
				break;
			default:
				printf("Unknown operation");
				break;
		}
		processALUXInst();
	}
	else   //K
	{
		switch(ALUOP(opcode))
		{
			case BPF_ADD:
				printf("|BPF_ADD|BPF_K:");
				break;
			case BPF_SUB:
				printf("|BPF_SUB|BPF_K:");
				break;
			case BPF_AND:
				printf("|BPF_AND|BPF_K:");
				break;
			case BPF_OR:
				printf("|BPF_OR|BPF_K:");
				break;
			case BPF_LSH:
				printf("|BPF_LSH|BPF_K:");
				break;
			case BPF_RSH:
				printf("|BPF_RSH|BPF_K:");
				break;
			case BPF_XOR:
				printf("|BPF_XOR|BPF_K:");
				break;
			case BPF_MUL:
				printf("|BPF_MUL|BPF_K:");
				break;
			case BPF_MOV:
				printf("|BPF_MOV|BPF_K:");
				break;
			case BPF_DIV:
				printf("|BPF_DIV|BPF_K:");
				break;
			case BPF_MOD:
				printf("|BPF_MOD|BPF_K:");
				break;
			default:
				printf("Unknown operation");
				break;
		}
		processALUKInst();

	}
	return 0;
}

int printDataSize(int opcode)
{
	switch(LDSTSIZE(opcode))
	{
		case BPF_B:
			printf("|BPF_B:");
			break;
		case BPF_H:
			printf("|BPF_H:");
			break;
		case BPF_W:
			printf("|BPF_W:");
			break;
		case BPF_DW:
			printf("|BPF_DW:");
			break;
		default:
			break;
	}
	return 0;
}

int printLD_IMM_DW_Syntax(int opcode) 
{
	if(LDSTSIZE(opcode) != 0x18)
	{
		printf("Invalid LD_IMM_DW instruction");
		exit(0);
	}
	unsigned long imm1 = get32ImmediateValue();
	int dstReg = getDstReg();
	curInst++;
	unsigned long imm2 = get32ImmediateValue();
	unsigned long value = (imm2 << 32) + imm1;
	printf("dstReg[%d] = 0x%lx", dstReg, value);
	return 0;
}

int processLDInst(int opcode)
{
	switch(LDSTOP(opcode))
	{
		case BPF_ABS:
			printf("|BPF_ABS");
			printDataSize(opcode);
			break;
		case BPF_IND:
			printf("|BPF_IND");
			printDataSize(opcode);
			break;
		case BPF_IMM:
			printf("|BPF_IMM");
			printDataSize(opcode);
			printLD_IMM_DW_Syntax(opcode);
			break;
		default:
			break;
	}
	return 0;
}


int printLDXSyntax(int opcode)
{

	int offset = getOffset();
	int dstReg = getDstReg(); 
	int srcReg = getSrcReg();
	printf("dstReg[%d] = ", dstReg);
	switch(LDSTSIZE(opcode))
	{
		case BPF_B:
			printf("*(BYTE*)");
			break;
		case BPF_H:
			printf("*(HALF BYTE*)");
			break;
		case BPF_W:
			printf("*(WORD*)");
			break;
		case BPF_DW:
			printf("*(DOUBLE WORD*)");
			break;
		default:
			break;
	}
	printf("((unsigned long) (srcReg[%d] + %d))",srcReg, offset );
	return 0;
}

int processLDXInst(int opcode)
{
	switch(LDSTOP(opcode))
	{
		case BPF_MEM:
			printf("|BPF_MEM");
			printDataSize(opcode);
			printLDXSyntax(opcode);
			break;
		default:
			break;
	}
	return 0;
}


int printSTInstSyntax(int opcode)
{
	int offset = getOffset();
	int dstReg = getDstReg(); 
	int imm = get32ImmediateValue();
	switch(LDSTSIZE(opcode))
	{
		case BPF_B:
			printf("*(BYTE*)");
			break;
		case BPF_H:
			printf("*(HALF BYTE*)");
			break;
		case BPF_W:
			printf("*(WORD*)");
			break;
		case BPF_DW:
			printf("*(DOUBLE WORD*)");
			break;
		default:
			break;
	}
	printf("[dstReg[%d] + %d] = %d ", dstReg, offset, imm);

	return 0;
}

int processSTInst(int opcode)
{
	switch(LDSTOP(opcode))
	{
		case BPF_MEM:
			printf("|BPF_MEM");
			printDataSize(opcode);
			printSTInstSyntax(opcode);
			break;
		default:
			break;
	}
	return 0;
}

int printSTXMEMSyntax(int opcode)
{
	
	int offset = getOffset();
	int dstReg = getDstReg(); 
	int srcReg = getSrcReg();
	switch(LDSTSIZE(opcode))
	{
		case BPF_B:
			printf("*(BYTE*)");
			break;
		case BPF_H:
			printf("*(HALF BYTE*)");
			break;
		case BPF_W:
			printf("*(WORD*)");
			break;
		case BPF_DW:
			printf("*(DOUBLE WORD*)");
			break;
		default:
			break;
	}
	printf("[dstReg[%d] + %d] = srcReg[%d] ", dstReg, offset, srcReg);
	return 0;
}

int printSTXXADDSyntax(int opcode)
{
	//TODO
	return 0;
}

int processSTXInst(int opcode)
{
	switch(LDSTOP(opcode))
	{
		case BPF_MEM:
			printf("|BPF_MEM");
			printDataSize(opcode);
			printSTXMEMSyntax(opcode);
			break;
		case BPF_XADD:
			printf("|BPF_XADD");
			printDataSize(opcode);
			printSTXXADDSyntax(opcode);
			break;
		default:
			break;
	}
	return 0;
}

int main()
{
	int length = 0x148;
	int i;
	int opcode;
	int count = 0;
	for(curInst; curInst < length/8; curInst++, count++)
	{
		opcode = (int)(prog[curInst*8]) & 0xff;
		printf("[%02d instrtuction(opcode 0x%02x):] ", count, opcode);
		switch(BPF_OPCLASS(opcode) )
		{
			case BPF_ALU:
				printf("BPF_ALU");
				processALUInst(opcode);
				break;
			case BPF_ALU64:
				printf("BPF_ALU64");
				processALU64Inst(opcode);
				break;
			case BPF_JMP:
				printf("BPF_JMP");
				processJMPInst(opcode);
				break;
			case BPF_STX:
				printf("BPF_STX");
				processSTXInst(opcode);
				break;
			case BPF_ST:
				printf("BPF_ST");
				processSTInst(opcode);
				break;
			case BPF_LDX:
				printf("BPF_LDX");
				processLDXInst(opcode);
				break;
			case BPF_LD:
				printf("BPF_LD");
				processLDInst(opcode);
				break;
			default:
				printf("Invalid opcode");
				exit(0);
				break;
		}
		printf("\n");
	}
	return 0;
}
