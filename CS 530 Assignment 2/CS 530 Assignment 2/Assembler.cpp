#include <new>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "InstructionTableDictionary.h"
#include "SymbolTableDictionary.h"
#include "Assembler.h";

using namespace std;

#define READ "r"
#define MAX_RECORD_SIZE 68
#define MAX_LABEL_SIZE 8
#define MAX_OPCODE_SIZE 8
#define MAX_OPERAND_SIZE 50

int PCArray[500];
string OpCodeArray[500];
string LabelArray[500];
string OperandArray[500];
string LitArray[500];
int PC = 0;
int StartingAddress = 0;
int IndexCount = -1;
int ProgramLength;
char* error = new char[100];

void Pass1(std::string Path)
{
	FILE *localFP;
	if ((localFP = fopen(Path.c_str(), READ)) == NULL)
	{
		error  = "File does not exist: ";
		//strcat(error, Path.c_str());
		//throw runtime_error(error);
	}
	char line[MAX_RECORD_SIZE];
	char* Label = new char[MAX_LABEL_SIZE];
	char* OpCode = new char[MAX_OPCODE_SIZE];
	char* Operand = new char[MAX_OPERAND_SIZE];

	//Read each line in the file
	while(fgets(line, sizeof(line), localFP) != NULL)
	{
		IndexCount++;
		//Check if line is a comment
		if(line [0] != '.')
		{
			GetLabel(Label, line);
			GetOpCode(OpCode, line);
			GetOperand(Operand, line);

			//Check if OpCode is directive
			if(OpCode == "START")
			{
				PC = StartingAddress = stringToInt(Operand);
			}
			else if (OpCode == "END")
			{
				break;
			}
			else if (OpCode == "RESB")
			{
				int value = stringToInt(Operand);
				PC += value;
			}
			else if (OpCode == "RESW")
			{
				TrimEnd(Operand);
				int value = stringToInt(Operand);
				PC += (value*3);
			}
			else if (OpCode == "BYTE")
			{
				if(Operand[0] == 'X' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					PC += (strlen(Operand) / 2);
					//PC += 1;
				}
				else if(Operand[0] == 'C' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					PC += strlen(Operand);
				}
				else
					PC += 1;
			}
			else if (OpCode == "WORD")
			{
				PC += 3;
			}
			else if (OpCode == "EQU")
			{
				if(Operand[0] == 'X' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					PC += (strlen(Operand) / 2);
					//Add Symbol (done below)
				}
				else if(Operand[0] == 'C' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					PC += strlen(Operand);
					//Add Symbol (done below)
				}
				else if(Operand[0] == '*')
				{
					//Add Symbol (done below)
				}
				else
				{
					PC += stringToInt(Operand);
					//Add Symbol (done below)
				}
			}
			else
			{
				Mnemonic currentMnemonic;
				//Check if OpCode exists in table
				map<std::string, Mnemonic>::iterator it = InstructionTable.find(OpCode);
				//If it exists, process line
				if(it != InstructionTable.end())
				{
					currentMnemonic = it->second;
					if(OpCode[0] == '+')
						PC += 4;
					else if(currentMnemonic.isFormat1)
						PC += 1;
					else if(currentMnemonic.isFormat0 || currentMnemonic.isFormat3)
						PC += 3;
					else if(currentMnemonic.isFormat2)
						PC += 2;
				}
				else
				{
					error = "OpCode not found: ";
					strcat(error, OpCode);
					throw runtime_error(error);
				}
			}

			//Check if line contains a label
			if(Label != "")
			{
				map<std::string, Symbol>::iterator it = SymbolTable.find(Label);
				//If it does not exist, add symbol to table
				if(it == SymbolTable.end())
				{
					std::string label(Label);
					SymbolTable.insert(make_pair(label, Symbol(label, PC)));
				}
				else
				{
					error = "Duplicate labels found: ";
					//strcat(error, Label);
					//throw runtime_error(error);
				}
			}

			PCArray[IndexCount] = PC;
			OpCodeArray[IndexCount] = OpCode;
			LabelArray[IndexCount] = Label;
			OperandArray[IndexCount] = Operand;
		}
		else
		{
			//line is a comment
		}
	}
	ProgramLength = PC - StartingAddress;
}

void GetLabel(char* dest, char* line)
{
	int i;
	for(i = 0; i < 8; i++)
		dest[i] = line[i];
	dest[i] = '\0';
	TrimEnd(dest);
}
void GetOpCode(char* dest, char* line)
{
	int i;
	for(i = 0; i < 8; i++)
		dest[i] = line[i+9];
	dest[i] = '\0';
	TrimEnd(dest);
}
void GetOperand(char* dest, char* line)
{
	int i;
	for(i = 0; i < 50; i++)
	{
		dest[i] = line[i+18];
		if(dest[i] == '\0')
			break;
	}
	dest[i] = '\0';
	TrimEnd(dest);
}

int stringToInt(char* string)
{
	return (int)strtol(string, NULL, 16);
}

void TrimEnd(char *string)
{
	char *ptr = string;
	int len = strlen(string);
	ptr = ptr + (len - 1);
	for(int i = len; i > 0 ; i--)
	{
		if(*ptr == ' ' || *ptr == '\t')
		{
			*ptr = '\0';
			ptr--;
		}
		else
			break;
	}
}

void RemoveEndApostrophe(char* string)
{
	char *ptr = string;
	ptr = ptr + (strlen(string) - 1);
	*ptr = '\0';
}

void RemoveOperandType(char *string)
{
	char *ptr = string;
	ptr += 2;
	strcpy(string, ptr);
}