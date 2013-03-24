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
#define MAXRECORDSIZE 68
#define MAXLABELSIZE 8
#define MAXOPCODESIZE 8
#define MAXOPERANDSIZE 50

int PC = 0;
int StartingAddress = 0;
int ProgramLength;

void Pass1(std::string Path)
{
	FILE *localFP = fopen(Path.c_str(), READ);
	char line[MAXRECORDSIZE];
	char* Label = new char[MAXLABELSIZE];
	char* OpCode = new char[MAXOPCODESIZE];
	char* Operand = new char[MAXOPERANDSIZE];

	//Read each line in the file
	while(fgets(line, sizeof(line), localFP) != NULL)
	{
		//Check if line is a comment
		if(line [0] != '.')
		{
			GetLabel(Label, line);
			GetOpCode(OpCode, line);
			GetOperand(Operand, line);

			//Check if line contains a label
			if(Label != "")
			{
				//Add to symbol table
				std::string label(Label);
				SymbolTable.insert(make_pair(label, Symbol(label, PC)));
			}

			//Check if OpCode is directive
			if(OpCode == "START")
			{
				PC = StartingAddress = StringToInt(Operand);
			}
			else if (OpCode == "END")
			{
				break;
			}
			else if (OpCode == "RESB")
			{
				int value = StringToInt(Operand);
				PC += value;
			}
			else if (OpCode == "RESW")
			{
				TrimEnd(Operand);
				int value = StringToInt(Operand);
				PC += (value*3);
			}
			else if (OpCode == "BYTE")
			{
				if(Operand[0] == 'X' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					PC += 1;
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
				/*if(Operand[0] == 'X' && Operand[1] == '\'')
				{
					TrimEnd(Operand);
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
				}
				else if(Operand[0] == 'C' && Operand[1] == '\'')
				{
					TrimEnd(Operand);
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
				}
				else
					PC += 3;*/
			}
			else if (OpCode == "EQU")
			{
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
			}
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

int StringToInt(char* string)
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