#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>

enum opcode_decode
{
	R = 0x33,	  // done
	I = 0x13,	  // done
	S = 0x23,	  // done
	L = 0x03,	  // done
	B = 0x63,	  // done
	JALR = 0x67,  // done
	JAL = 0x6F,	  // done
	AUIPC = 0x17, // done
	LUI = 0x37	  // done
};

typedef struct
{
	size_t data_mem_size_;
	uint32_t regfile_[32];
	uint32_t pc_;
	uint8_t *instr_mem_;
	uint8_t *data_mem_;
} CPU;

void CPU_open_instruction_mem(CPU *cpu, const char *filename);
void CPU_load_data_mem(CPU *cpu, const char *filename);

CPU *CPU_init(const char *path_to_inst_mem, const char *path_to_data_mem)
{
	CPU *cpu = (CPU *)malloc(sizeof(CPU));
	cpu->data_mem_size_ = 0x400000;
	cpu->pc_ = 0x0;
	CPU_open_instruction_mem(cpu, path_to_inst_mem);
	CPU_load_data_mem(cpu, path_to_data_mem);
	return cpu;
}

void CPU_open_instruction_mem(CPU *cpu, const char *filename)
{
	uint32_t instr_mem_size;
	FILE *input_file = fopen(filename, "r");
	if (!input_file)
	{
		printf("no input\n");
		exit(EXIT_FAILURE);
	}
	struct stat sb;
	if (stat(filename, &sb) == -1)
	{
		printf("error stat\n");
		perror("stat");
		exit(EXIT_FAILURE);
	}
	printf("size of instruction memory: %d Byte\n\n", sb.st_size);
	instr_mem_size = sb.st_size;
	cpu->instr_mem_ = malloc(instr_mem_size);
	fread(cpu->instr_mem_, sb.st_size, 1, input_file);
	fclose(input_file);
	return;
}

void CPU_load_data_mem(CPU *cpu, const char *filename)
{
	FILE *input_file = fopen(filename, "r");
	if (!input_file)
	{
		printf("no input\n");
		exit(EXIT_FAILURE);
	}
	struct stat sb;
	if (stat(filename, &sb) == -1)
	{
		printf("error stat\n");
		perror("stat");
		exit(EXIT_FAILURE);
	}
	printf("read data for data memory: %d Byte\n\n", sb.st_size);

	cpu->data_mem_ = malloc(cpu->data_mem_size_);
	fread(cpu->data_mem_, sb.st_size, 1, input_file);
	fclose(input_file);
	return;
}

extern int add(int x, int y);

#define APP_START (0x00000000)
#define APP_LEN (0x200)
#define APP_ENTRY (0x00000000)

#define SIZE_SIEB 2000

uint8_t ui8 = 0x4;
uint16_t ui16 = 0x5678;
uint32_t ui32 = 0x9ABCD;

int8_t i8 = 0x1234;
int16_t i16 = 0x5678;
int32_t i32 = 0x9ABCD;

int test_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

/**
* Instruction fetch Instruction decode, Execute, Memory access, Write back
*/
void CPU_execute(CPU *cpu)
{
	uint32_t instruction;
	uint8_t opcode;
	int counter = cpu->pc_;
	printf("cpu->pc_ before switch: %x\n", cpu->pc_);
	printf("counter before switch: %x\n", counter);
	instruction = *(uint32_t *)(cpu->instr_mem_ + (counter & 0xFFFFF));
	opcode = instruction & 0x7f;
	printf("Instruction before switch: %x\n", instruction);
	printf("opcode before switch: %d\n", opcode);

	switch (opcode)
	{
	case LUI:; //checked
		printf("------LUI-----\n");
		uint8_t rd_LUI = (uint8_t)(instruction >> 7) & 0x1f;
		printf("rd: %d\n", rd_LUI);
		uint32_t imm_LUI_tmp = (instruction >> 12);
		uint32_t imm_LUI = imm_LUI_tmp << 12;
		printf("imm: 0x%x\n", imm_LUI);
		printf("imm: %d\n", imm_LUI);

		if (rd_LUI == 0)
		{
			cpu->pc_ = counter + 4;
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		else
		{
			cpu->regfile_[rd_LUI] = imm_LUI;
			cpu->pc_ = counter + 4;
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}

	case AUIPC:; //checked
		printf("------AUIPC-----\n");
		uint8_t rd_AUIPC = (uint8_t)(instruction >> 7) & 0x1f;
		printf("rd: %d\n", rd_AUIPC);
		uint32_t imm_AUIPC_tmp = (instruction >> 12);
		uint32_t imm_AUIPC = imm_AUIPC_tmp << 12;
		printf("imm: %d\n", imm_AUIPC);
		printf("imm: 0x%x\n", imm_AUIPC);

		if (rd_AUIPC == 0)
		{
			cpu->pc_ = counter + 4;
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		else
		{
			cpu->regfile_[rd_AUIPC] = cpu->pc_ + imm_AUIPC;
			cpu->pc_ = counter + 4;
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}

	case JAL:; //checked
		printf("------JAL-----\n");
		uint8_t rd_JAL = (uint8_t)(instruction >> 7) & 0x1f;
		printf("rd: %d\n", rd_JAL);

		uint32_t imm_first10 = (instruction >> 21) & 0x3ff;
		uint32_t imm_11 = (instruction >> 20) & 0x1;
		uint32_t imm_first11 = imm_first10 | (imm_11 << 10);

		uint32_t imm12_19 = (instruction >> 12) & 0xff;
		uint32_t imm_first19 = (imm12_19 << 11) | imm_first11;

		uint32_t imm_first31;

		uint32_t imm_20 = (instruction >> 31) & 0x1;
		// if (imm_20 == 0x0)
		// {
		// 	imm_first31 = (uint32_t)imm_first19;
		// }
		// else if (imm_20 == 0x1)
		// {
		// 	imm_first31 = imm_first19 | 0x7ff80000;
		// }
		if (imm_20 == 0x1)
		{
			imm_first31 = imm_first19 | 0xfff80000;
		}
		else
		{
			imm_first31 = imm_first19;
		}
		// Final step
		uint32_t imm_final = imm_first31 << 1;
		printf("imm: %d\n", imm_final);
		printf("imm: %x\n", imm_final);

		if (rd_JAL == 0)
		{
			cpu->pc_ = counter + (int32_t)imm_final;
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		else
		{
			cpu->regfile_[rd_JAL] = cpu->pc_ + 4;
			cpu->pc_ = counter + (int32_t)imm_final;
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}

	case JALR:; //checked
		printf("------JALR-----\n");
		uint8_t rd_JALR = (uint8_t)(instruction >> 7) & 0x1f;
		printf("rd: %d\n", rd_JAL);
		uint8_t funct3_JALR = (uint8_t)(instruction >> 12) & 0x7;
		printf("funct3: %d\n", funct3_JALR);
		uint8_t rs1_JALR = (uint8_t)(instruction >> 15) & 0x1f;
		printf("rs1: %d\n", rs1_JALR);
		uint32_t imm_JALR_first_12 = (instruction >> 20) & 0xfff;
		uint8_t imm_JALR_11 = (instruction >> 31) & 0x1;
		uint32_t imm_JALR;
		if (imm_JALR_11 == 0x1)
		{
			imm_JALR = imm_JALR_first_12 | 0xfffff000;
		}
		else
		{
			imm_JALR = imm_JALR_first_12;
		}

		//imm_JALR = imm_JALR_first_12;

		printf("imm: %d\n", imm_JALR);

		if (rd_JALR == 0)
		{
			cpu->pc_ = (cpu->regfile_[rs1_JALR]) + ((int32_t)imm_JALR);
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		else
		{
			cpu->regfile_[rd_JALR] = counter + 4;
			cpu->pc_ = (cpu->regfile_[rs1_JALR]) + ((int32_t)imm_JALR);
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}

	case R:;
		printf("------R-----\n");
		uint8_t rd_R = (uint8_t)(instruction >> 7) & 0x1f;
		//printf("rd: %d\n", rd_R);
		uint8_t funct3_R = (uint8_t)(instruction >> 12) & 0x7;
		//printf("funct3: %d\n", funct3_R);
		uint8_t rs1_R = (uint8_t)(instruction >> 15) & 0x1f;
		//printf("rs1: %d\n", rs1_R);
		uint8_t rs2_R = (uint8_t)(instruction >> 20) & 0x1f;
		//printf("rs2: %d\n", rs2_R);
		uint8_t funct7_R = (uint8_t)(instruction >> 25) & 0x7f;
		//printf("funct7: %d\n", funct7_R);

		// ADD
		if (funct3_R == 0x0 && funct7_R == 0x0)
		{
			printf("------ADD-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);

			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = cpu->regfile_[rs1_R] + cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// SUB
		if (funct3_R == 0x0 && funct7_R == 0x20)
		{
			printf("------SUB-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);
			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = cpu->regfile_[rs1_R] - cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("PC: %d\n", cpu->pc_);
				break;
			}
		}
		// SLL
		if (funct3_R == 0x1 && funct7_R == 0x0)
		{
			printf("------SLL-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);

			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = cpu->regfile_[rs1_R] << cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// SLT signed comparison
		if (funct3_R == 0x2 && funct7_R == 0x0)
		{
			printf("------SLT-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);
			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = (int32_t)cpu->regfile_[rs1_R] < (int32_t)cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// SLTU
		if (funct3_R == 0x3 && funct7_R == 0x0)
		{
			printf("------SLTU-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);
			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = cpu->regfile_[rs1_R] < cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// XOR
		if (funct3_R == 0x4 && funct7_R == 0x0)
		{
			printf("------XOR-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);

			printf("cpu->regfile_[rs1_R]: %x\n", cpu->regfile_[rs1_R]);
			printf("cpu->regfile_[rs2_R]: %x\n", cpu->regfile_[rs2_R]);
			printf("cpu->regfile_[%d] = %x\n", rd_R, cpu->regfile_[rs1_R] ^ cpu->regfile_[rs2_R]);

			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = cpu->regfile_[rs1_R] ^ cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// SRL
		if (funct3_R == 0x5 && funct7_R == 0x0)
		{
			printf("------SRL-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);

			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = cpu->regfile_[rs1_R] >> cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// SRA
		if (funct3_R == 0x5 && funct7_R == 0x20)
		{
			printf("------SRA-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);

			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = (int32_t)cpu->regfile_[rs1_R] >> (int32_t)cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// OR
		if (funct3_R == 0x6 && funct7_R == 0x0)
		{
			printf("------OR-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);

			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = cpu->regfile_[rs1_R] | cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// AND
		if (funct3_R == 0x7 && funct7_R == 0x0)
		{
			printf("------AND-----\n");
			printf("rd: %d\n", rd_R);
			printf("funct3: %d\n", funct3_R);
			printf("rs1: %d\n", rs1_R);
			printf("rs2: %d\n", rs2_R);
			printf("funct7: %d\n", funct7_R);

			if (rd_R == 0)
			{
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_R] = cpu->regfile_[rs1_R] & cpu->regfile_[rs2_R];
				cpu->pc_ = counter + 4;
				printf("cpu->pc_: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}

	case I:; //checked for ADDI
		printf("------I-----\n");
		uint8_t rd_I = (uint8_t)(instruction >> 7) & 0x1f;
		uint8_t funct3_I = (uint8_t)(instruction >> 12) & 0x7;
		uint8_t rs1 = (uint8_t)(instruction >> 15) & 0x1f;

		if (funct3_I == 0x0 || funct3_I == 0x2 || funct3_I == 0x3 || funct3_I == 0x4 || funct3_I == 0x6 || funct3_I == 0x7)
		{
			uint32_t imm_I_first12 = (instruction >> 20) & 0xfff;
			uint32_t imm_I;
			uint8_t imm_I_11 = (instruction >> 31) & 0x1;
			if (imm_I_11 == 0x1)
			{
				imm_I = imm_I_first12 | 0xfffff800;
			}
			else
			{
				imm_I = imm_I_first12;
			}
			//imm_I = imm_I_first12;

			//ADDI //checked
			if (funct3_I == 0x0)
			{
				printf("------ADDI-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);
				printf("imm: %x\n", imm_I);

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					cpu->regfile_[rd_I] = cpu->regfile_[rs1] + imm_I;
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
			}
			//SLTI signed comparison
			if (funct3_I == 0x2)
			{
				printf("------SLTI-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);
				printf("imm: %x\n", imm_I);

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					cpu->regfile_[rd_I] = (signed)cpu->regfile_[rs1] < (signed)imm_I;
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
			}
			//SLTIU
			if (funct3_I == 0x3)
			{
				printf("------SLTIU-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);
				printf("imm: %x\n", imm_I);

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					cpu->regfile_[rd_I] = cpu->regfile_[rs1] < imm_I;
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
			}
			//XORI
			if (funct3_I == 0x4)
			{
				printf("------XORI-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					cpu->regfile_[rd_I] = cpu->regfile_[rs1] ^ imm_I;
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
			}
			//ORI
			if (funct3_I == 0x6)
			{
				printf("------ORI-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);
				printf("imm: %x\n", imm_I);

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					cpu->regfile_[rd_I] = cpu->regfile_[rs1] | imm_I;
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					break;
				}
			}
			//ANDI
			if (funct3_I == 0x7)
			{
				printf("------ANDI-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					cpu->regfile_[rd_I] = cpu->regfile_[rs1] & imm_I;
					cpu->pc_ = counter + 4;
					printf("cpu->pc_ %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
			}
		}

		if (funct3_I == 0x1 || funct3_I == 0x5)
		{
			uint32_t imm_I_first12 = (instruction >> 20) & 0xfff;
			uint32_t imm_I;
			uint8_t imm_I_11 = (instruction >> 31) & 0x1;
			if (imm_I_11 == 0x1)
			{
				imm_I = imm_I_first12 | 0xfffff000;
			}
			else
			{
				imm_I = imm_I_first12;
			}
			uint32_t shamt_I = (instruction >> 20) & 0x1f;
			uint32_t funct7_I = (instruction >> 25) & 0x7f;

			//SLLI
			if (funct3_I == 0x1 && funct7_I == 0x0)
			{
				printf("------SLLI-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);
				printf("imm: %x\n", imm_I);

				printf("shamt: %d\n", shamt_I);
				printf("funct7: %d\n", funct7_I);
				//cpu->regfile_[rd_I] = cpu->regfile_[rs1] << imm_I;

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					cpu->regfile_[rd_I] = cpu->regfile_[rs1] << shamt_I;
					cpu->pc_ = counter + 4;
					printf("PC after I SLLI: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
			}
			//SRLI
			if (funct3_I == 0x5 && funct7_I == 0x0)
			{
				printf("------SRLI-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);
				printf("imm: %x\n", imm_I);
				printf("shamt: %d\n", shamt_I);
				printf("funct7: %d\n", funct7_I);
				//cpu->regfile_[rd_I] = cpu->regfile_[rs1] >> imm_I;

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					cpu->regfile_[rd_I] = cpu->regfile_[rs1] >> shamt_I;
					cpu->pc_ = counter + 4;
					printf("PC after I SRLI: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
			}
			//SRAI
			if (funct3_I == 0x5 && funct7_I == 0x20)
			{
				printf("------SRAI-----\n");
				printf("rd: %d\n", rd_I);
				printf("funct3: %d\n", funct3_I);
				printf("rs1: %d\n", rs1);
				printf("imm: %d\n", imm_I);
				printf("imm: %x\n", imm_I);
				printf("shamt: %d\n", shamt_I);
				printf("funct7: %d\n", funct7_I);
				//cpu->regfile_[rd_I] = cpu->regfile_[rs1] >> imm_I;

				if (rd_I == 0)
				{
					cpu->pc_ = counter + 4;
					printf("PC: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
				else
				{
					//cpu->regfile_[rd_I] = cpu->regfile_[rs1] >> shamt_I;
					uint32_t sign_bit = (cpu->regfile_[rs1] >> 31) & 0x1;

					if (sign_bit == 0x1)
					{
						uint32_t result_tmp = cpu->regfile_[rs1] >> shamt_I;
						uint32_t tmp = 0xffffffff >> shamt_I;
						uint32_t tmp2 = tmp ^ 0xffffffff;
						cpu->regfile_[rd_I] = tmp2 | result_tmp;
					}
					else
					{
						cpu->regfile_[rd_I] = cpu->regfile_[rs1] >> shamt_I;
					}

					printf("---------------------------------------------------------------------------------\n");
		
					cpu->pc_ = counter + 4;
					printf("PC after I SLLI: %d\n", cpu->pc_);
					printf("------------------------------\n");
					break;
				}
			}
		}

	case S:; // checked the basics, checked for SW
		printf("------S-----\n");

		uint8_t funct3_S = (uint8_t)(instruction >> 12) & 0x7;
		uint8_t rs1_S = (uint8_t)(instruction >> 15) & 0x1f;
		uint8_t rs2_S = (uint8_t)(instruction >> 20) & 0x1f;

		uint32_t imm_S_first5 = (instruction >> 7) & 0x1f;
		uint32_t imm_last7_S = (instruction >> 25) & 0x7f;
		uint32_t imm_first12_S = (imm_last7_S << 5) | imm_S_first5;
		uint32_t imm_final_S;
		uint8_t imm_11_S = (instruction >> 31) & 0x1;
		printf("imm_11_S: %x\n", imm_11_S);

		imm_final_S = imm_first12_S;

		// if (imm_11_S == 0x1)
		// {
		// 	imm_final_S = imm_first12_S | 0xfffff000;
		// }
		// else
		// {
		// 	imm_final_S = imm_first12_S;
		// }

		//SB
		if (funct3_S == 0x0)
		{
			printf("------SB-----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_S);
			printf("rs2: %d\n", rs2_S);
			printf("imm: %d\n", imm_final_S);
			printf("imm: %x\n", imm_final_S);

			cpu->data_mem_[cpu->regfile_[rs1_S] + (int32_t)imm_final_S] = (uint8_t)cpu->regfile_[rs2_S];
			cpu->pc_ = counter + 4;
			printf("cpu->pc_ after S SB: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}

		//SH
		if (funct3_S == 0x1)
		{
			printf("------SH-----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_S);
			printf("rs2: %d\n", rs2_S);
			printf("imm: %d\n", imm_final_S);
			printf("imm: %x\n", imm_final_S);

			*(uint16_t *)(cpu->data_mem_ + cpu->regfile_[rs1_S] + imm_final_S) = (uint16_t)cpu->regfile_[rs2_S];
			cpu->pc_ = counter + 4;
			printf("PC after S SH: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		//SW
		if (funct3_S == 0x2)
		{
			printf("------SW-----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_S);
			printf("rs2: %d\n", rs2_S);
			printf("imm: %d\n", imm_final_S);
			printf("imm: %x\n", imm_final_S);

			printf("imm: %x\n", imm_final_S);

			*(uint32_t *)(cpu->data_mem_ + cpu->regfile_[rs1_S] + imm_final_S) = (uint32_t)cpu->regfile_[rs2_S];
			cpu->pc_ = counter + 4;
			printf("PC after SW: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}

	case B:; // CHECK AGAIN
		printf("------B-----\n");
		uint8_t funct3_B = (uint8_t)(instruction >> 12) & 0x7;
		uint8_t rs1_B = (uint8_t)(instruction >> 15) & 0x1f;
		uint8_t rs2_B = (uint8_t)(instruction >> 20) & 0x1f;

		uint32_t imm_first4_B = (instruction >> 8) & 0xf;
		uint32_t imm_5_10_B = (instruction >> 25) & 0x3f;
		uint32_t imm_1_10_B = (imm_5_10_B << 4) | imm_first4_B;

		uint32_t imm11_B = (instruction >> 7) & 0x1;
		uint32_t imm_first11_B;
		if (imm11_B == 0x1)
		{
			imm_first11_B = imm_1_10_B | 0x400;
		}
		else
		{
			imm_first11_B = imm_1_10_B;
		}
		uint32_t imm12_B = (instruction >> 31) & 0x1;
		uint32_t imm_first12_B;
		if (imm12_B == 0x1)
		{
			imm_first12_B = imm_first11_B | 0xfffff800; //(11111111111111111111100000000000)
		}
		else
		{
			imm_first12_B = imm_first11_B;
		}

		uint32_t imm_final_B = imm_first12_B << 0x1;
		//printf("imm: %d\n", imm_final_B);

		// BEQ
		if (funct3_B == 0x0)
		{
			printf("------BEQ-----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_B);
			printf("rs2: %d\n", rs2_B);
			printf("imm: %d\n", imm_final_B);
			printf("imm: %x\n", imm_final_B);

			if (cpu->regfile_[rs1_B] == cpu->regfile_[rs2_B])
			{
				cpu->pc_ = counter + (int32_t)imm_final_B;
			}
			else
			{
				cpu->pc_ = counter + 4;
			}
			printf("PC: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		// BNE
		if (funct3_B == 0x1)
		{
			printf("------BNE-----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_B);
			printf("rs2: %d\n", rs2_B);
			printf("imm: %d\n", imm_final_B);
			printf("imm: %x\n", imm_final_B);

			if (cpu->regfile_[rs1_B] != cpu->regfile_[rs2_B])
			{
				cpu->pc_ = counter + (int32_t)imm_final_B;
			}
			else
			{
				cpu->pc_ = counter + 4;
			}
			printf("cpu->pc_: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		// BLT signed comparison
		if (funct3_B == 0x4)
		{
			printf("------BLT-----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_B);
			printf("rs2: %d\n", rs2_B);
			printf("imm: %d\n", imm_final_B);

			printf("---------------------ALTYNAI BLT---------------------------------------------------------------------------------------\n");


			printf("cpu->regfile_[rs1_B]: %d\n", cpu->regfile_[rs1_B]);

			printf("cpu->regfile_[rs2_B]: %d\n", cpu->regfile_[rs2_B]);



			if ((signed)cpu->regfile_[rs1_B] < (signed)cpu->regfile_[rs2_B])
			{
				printf("cpu->regfile_[rs1_B] < cpu->regfile_[rs2_B]");
				cpu->pc_ = counter + (int32_t)imm_final_B;
			}
			else
			{
				printf("cpu->regfile_[rs1_B] >= cpu->regfile_[rs2_B]");
				cpu->pc_ = counter + 4;
			}
			printf("PC: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		// BGE signed comparison
		if (funct3_B == 0x5)
		{
			printf("------BGE-----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_B);
			printf("rs2: %d\n", rs2_B);
			printf("imm: %d\n", imm_final_B);
			printf("imm: %x\n", imm_final_B);

			printf("---------------------ALTYNAI BGE---------------------------------------------------------------------------------------\n");


			printf("cpu->regfile_[rs1_B]: %d\n", cpu->regfile_[rs1_B]);

			printf("cpu->regfile_[rs2_B]: %d\n", cpu->regfile_[rs2_B]);

			if ((signed)cpu->regfile_[rs1_B] >= (signed)cpu->regfile_[rs2_B])
			{
				printf("cpu->regfile_[rs1_B] >= cpu->regfile_[rs2_B]");

				cpu->pc_ = counter + (int32_t)imm_final_B;
			}
			else
			{
				printf("cpu->regfile_[rs1_B] < cpu->regfile_[rs2_B]");

				cpu->pc_ = counter + 4;
			}
			printf("PC: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		// BLTU
		if (funct3_B == 0x6)
		{
			printf("------BLTU----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_B);
			printf("rs2: %d\n", rs2_B);
			printf("imm: %d\n", imm_final_B);
			printf("imm: %x\n", imm_final_B);

			if (cpu->regfile_[rs1_B] < cpu->regfile_[rs2_B])
			{
				cpu->pc_ = counter + (int32_t)imm_final_B;
			}
			else
			{
				cpu->pc_ = counter + 4;
			}
			printf("PC: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}
		// BGEU
		if (funct3_B == 0x7)
		{
			printf("------BGEU-----\n");
			printf("funct3: %d\n", funct3_S);
			printf("rs1: %d\n", rs1_B);
			printf("rs2: %d\n", rs2_B);
			printf("imm: %d\n", imm_final_B);
			printf("imm: %x\n", imm_final_B);

			if (cpu->regfile_[rs1_B] >= cpu->regfile_[rs2_B])
			{
				cpu->pc_ = counter + (int32_t)imm_final_B;
			}
			else
			{
				cpu->pc_ = counter + 4;
			}
			printf("PC: %d\n", cpu->pc_);
			printf("------------------------------\n");
			break;
		}

	case L:;
		printf("------L----\n");
		uint8_t rd_L = (uint8_t)(instruction >> 7) & 0x1f;
		//printf("rd: %d\n", rd_L);
		uint8_t funct3_L = (uint8_t)(instruction >> 12) & 0x7;
		//printf("funct3: %d\n", funct3_L);
		uint8_t rs1_L = (uint8_t)(instruction >> 15) & 0x1f;
		//printf("rs1: %d\n", rs1_L);

		uint32_t imm_L_first12 = (instruction >> 20) & 0xfff;
		uint32_t imm_L;
		// uint8_t imm_L_11 = (instruction >> 31) & 0x1;
		// if (imm_L_11 == 0x1)
		// {
		// 	imm_L = imm_L_first12 | 0xfffff000;
		// }
		// else
		// {
		// 	imm_L = imm_L_first12;
		// }
		imm_L = imm_L_first12;
		//printf("imm: %d\n", imm_L);

		// LB
		if (funct3_L == 0x0)
		{
			printf("------LB----\n");
			printf("rd: %d\n", rd_L);
			printf("funct3: %d\n", funct3_L);
			printf("rs1: %d\n", rs1_L);
			printf("imm: %d\n", imm_L);
			printf("imm: %x\n", imm_L);

			if (rd_L == 0)
			{
				cpu->pc_ = counter + 4;
				printf("PC: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				//cpu->regfile_[rd_L] = cpu->data_mem_[cpu->regfile_[rs1_L] + imm_L];
				uint32_t result_tmp = cpu->data_mem_[cpu->regfile_[rs1_L] + imm_L];
				if (((result_tmp >> 7) & 0x1) == 0x1)
				{
					cpu->regfile_[rd_L] = result_tmp | 0xffffff00;
				}
				else
				{
					cpu->regfile_[rd_L] = cpu->data_mem_[cpu->regfile_[rs1_L] + imm_L];
				}
				cpu->pc_ = counter + 4;
				printf("PC: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// LH
		if (funct3_L == 0x1)
		{
			printf("------LH----\n");
			printf("rd: %d\n", rd_L);
			printf("funct3: %d\n", funct3_L);
			printf("rs1: %d\n", rs1_L);
			printf("imm: %d\n", imm_L);
			printf("imm: %x\n", imm_L);

			if (rd_L == 0)
			{
				cpu->pc_ = counter + 4;
				printf("PC: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				//cpu->regfile_[rd_L] = *(uint16_t *)(cpu->regfile_[rs1_L] + imm_L + cpu->data_mem_);
				uint32_t result_tmp = *(uint16_t *)(cpu->regfile_[rs1_L] + imm_L + cpu->data_mem_);
				if (((result_tmp >> 15) & 0x1) == 0x1)
				{
					cpu->regfile_[rd_L] = result_tmp | 0xffff0000;
				}
				else
				{
					cpu->regfile_[rd_L] = *(uint16_t *)(cpu->regfile_[rs1_L] + imm_L + cpu->data_mem_);
				}
				cpu->pc_ = counter + 4;
				printf("PC: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// LW
		if (funct3_L == 0x2)
		{
			printf("------LW----\n");
			printf("rd: %d\n", rd_L);
			printf("funct3: %d\n", funct3_L);
			printf("rs1: %d\n", rs1_L);
			printf("imm: %d\n", imm_L);
			printf("imm: %x\n", imm_L);

			if (rd_L == 0)
			{
				cpu->pc_ = counter + 4;
				printf("PC: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_L] = *(uint32_t *)(cpu->regfile_[rs1_L] + imm_L + cpu->data_mem_);
				cpu->pc_ = counter + 4;
				printf("PC after L LW: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// LBU
		if (funct3_L == 0x4)
		{
			printf("------LBU----\n");
			printf("rd: %d\n", rd_L);
			printf("funct3: %d\n", funct3_L);
			printf("rs1: %d\n", rs1_L);
			printf("imm: %d\n", imm_L);
			printf("imm: %x\n", imm_L);

			if (rd_L == 0)
			{
				cpu->pc_ = counter + 4;
				printf("PC: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_L] = cpu->data_mem_[cpu->regfile_[rs1_L] + imm_L];
				cpu->pc_ = counter + 4;
				printf("PC after L LBU: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}
		// LHU
		if (funct3_L == 0x5)
		{
			printf("------LHU----\n");
			printf("rd: %d\n", rd_L);
			printf("funct3: %d\n", funct3_L);
			printf("rs1: %d\n", rs1_L);
			printf("imm: %d\n", imm_L);
			printf("imm: %x\n", imm_L);

			if (rd_L == 0)
			{
				cpu->pc_ = counter + 4;
				printf("PC: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
			else
			{
				cpu->regfile_[rd_L] = *(uint16_t *)(cpu->regfile_[rs1_L] + imm_L + cpu->data_mem_);
				cpu->pc_ = counter + 4;
				printf("PC after LHU: %d\n", cpu->pc_);
				printf("------------------------------\n");
				break;
			}
		}

	default:
		printf("run into DEFAULT by PC: %d\n", cpu->pc_);
		break;
	}

	instruction = *(uint32_t *)(cpu->instr_mem_ + (cpu->pc_ & 0xFFFFF));
	printf("Instruction after break: %x\n", instruction);
	opcode = instruction & 0x7f;
	printf("opcode after break: %d\n", opcode);

	printf("Test Addition: %d\n", ui16 + i8);
	printf("Test xor: %d\n", ui16 ^ i32);
	printf("Test and: %d\n", ui16 & i8);
	printf("Test or: %d\n", ui16 | i32);
	printf("Test shift <<: %d\n",ui32 << ui8);
	printf("Test shift >>: %d\n", ui32 >> ui8);
	printf("Test <: %d\n", ui16 < i8);
	printf("Test >: %d\n", ui16 > i8);
	printf("Test == %d\n", ui16 == i8);
	printf("Test != %d\n", ui16 != i8);
	for(int i = 0; i < 18; i++) {
	printf("test_array[%d]: %d\n",i,test_array[i]);
	}
	printf("%\nEnde!\n ");
}

int main(int argc, char *argv[])
{
	printf("C Praktikum\nHU Risc-V  Emulator 2022\n");

	CPU *cpu_inst;

	cpu_inst = CPU_init(argv[1], argv[2]);
	for (uint32_t i = 0; i < 1000000; i++)
	{ // run 70000 cycles
		CPU_execute(cpu_inst);
	}

	printf("\n-----------------------RISC-V program terminate------------------------\nRegfile values:\n");

	//output Regfile
	for (uint32_t i = 0; i <= 31; i++)
	{
		printf("%d: %X\n", i, cpu_inst->regfile_[i]);
	}
	fflush(stdout);

	return 0;
}
