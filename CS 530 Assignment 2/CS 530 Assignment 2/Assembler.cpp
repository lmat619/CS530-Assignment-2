#include <new>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <algorithm>
#include <vector>
#include "InstructionTableDictionary.h"
#include "SymbolTableDictionary.h"
#include "Assembler.h";

using namespace std;

MachineType machineType = None;
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
			if(!strcmp(OpCode, "START"))
			{
				PC = StartingAddress = HexToInt(Operand);
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
			
			if (!strcmp(OpCode, "END"))
			{
				break;
			}
			else if (!strcmp(OpCode, "RESB"))
			{
				int value = HexToInt(Operand);
				PC += value;
			}
			else if (!strcmp(OpCode, "RESW"))
			{
				TrimEnd(Operand);
				int value = HexToInt(Operand);
				PC += (value*3);
			}
			else if (!strcmp(OpCode,  "BYTE"))
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
			else if (!strcmp(OpCode, "WORD"))
			{
				PC += 3;
			}
			else if (!strcmp(OpCode, "EQU"))
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
					PC += HexToInt(Operand);
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
					{
						PC += 4;
						machineType = XE;
					}
					else if(currentMnemonic.isFormat1)
					{
						PC += 1;
						machineType = XE;
					}
					else if(currentMnemonic.isFormat0 || currentMnemonic.isFormat3)
						PC += 3;
					else if(currentMnemonic.isFormat2)
					{
						PC += 2;
						machineType = XE;
					}

					//Check if Operand is literal
					if(Operand[0] == '=')
					{

					}
				}
				else
				{
					error = "OpCode not found: ";
					strcat(error, OpCode);
					throw runtime_error(error);
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

void Pass2()
{
	string output("");
	string textRecord("");
	int textRecordStartAddress;
	int currentLineIndex;
	for (currentLineIndex = 0; currentLineIndex <= IndexCount; currentLineIndex++)
	{
		int currentPC = PCArray[currentLineIndex];
		string currentOpCode = OpCodeArray[currentLineIndex];
		string currentLabel = LabelArray[currentLineIndex];
		string currentOperand = OperandArray[currentLineIndex];
		string currentLiteral = LitArray[currentLineIndex];

		if(currentOpCode == "START")
		{
			output += "H";
			if (currentLabel.length() < 6)
				currentLabel = PadWithZeros(currentLabel, currentLabel.length(), 6);
			if(currentOperand.length() < 6)
				currentOperand = PadWithZeros(currentOperand, currentOperand.length(), 6);
			string progLength = IntToHex(ProgramLength);
			if(progLength.length() < 6)
				progLength = PadWithZeros(progLength, progLength.length(), 6) + "\n";
			output  += currentLabel + currentOperand + progLength;
			continue;
		}
		if(currentOpCode == "END")
		{
			//do stuff
			break;
		}
		string objectCode = GenerateObjectCode(currentPC, currentOpCode, currentLabel, currentOperand, currentLiteral);
		if((textRecord.length() + objectCode.length()) > 60)
		{
			//end this text record
			output += "T";
			string startAdd = IntToHex((int)textRecordStartAddress);
			string recordLength = IntToHex((int)(textRecord.length()/2));
			if(startAdd.length() < 6)
				startAdd = PadWithZeros(startAdd, startAdd.length(), 6);
			if(recordLength.length() < 2)
				recordLength = PadWithZeros(recordLength, recordLength.length(), 2);
			output += startAdd + recordLength + textRecord + "\n";
			//start new text record
			textRecord = objectCode;
			textRecordStartAddress = currentPC;
		}
		else
		{
			textRecord += objectCode;
		}
	}
}

string GenerateObjectCode(int currentPC, string currentOpCode, string currentLabel, string currentOperand, string currentLiteral)
{
	string objCode("");
	if(currentOpCode == "WORD")
	{
	}
	else if(currentOpCode == "BYTE")
	{
	}
	else if(currentOpCode == "RESW" || currentOpCode == "RESB")
	{
	}
	else
	{
		//get from dictionary
		Mnemonic currentMnemonic;
		//Check if OpCode exists in table
		map<std::string, Mnemonic>::iterator it = InstructionTable.find(currentOpCode);
		//If it exists, process line
		if(it != InstructionTable.end())
		{
			currentMnemonic = it->second;
			if(currentOpCode[0] == '+')
			{
				//format 4
			}
			else if(currentMnemonic.isFormat1)
			{
				objCode = currentMnemonic.Opcode;
			}
			else if(currentMnemonic.isFormat0 || currentMnemonic.isFormat3)
			{
			}
			else if(currentMnemonic.isFormat2)
			{
				objCode = currentMnemonic.Opcode;
				if(currentOperand.find(",") != string::npos)
				{
					std::vector<std::string> registers = SplitCommas(currentOperand);
					int reg1 = GetRegisterNum(registers[0]);
					int reg2 = GetRegisterNum(registers[1]);
					if(reg1 == -1 || reg2 == -1)
					{
						//report error
					}
					string register1 = IntToHex(reg1);
					string register2 = IntToHex(reg2);
					objCode += register1 + register2;
				}
				else
				{
					int reg1 = GetRegisterNum(currentOperand);
					if(reg1 == -1)
					{
						//report error
					}
					string register1 = IntToHex(reg1);
					objCode += register1;
				}
			}
		}
	}
	return objCode;
}

std::vector<std::string> SplitCommas(string operand)
{
	std::vector<std::string> registers(2);
	const char* op = operand.c_str();
	char* reg1 = new char[10];
	int i;
	for(i = 0; op[i] != ','; i++)
		reg1[i] = op[i];
	op += ++i;
	string register1(reg1);
	string register2(op);
	registers[0] = register1;
	registers[1] = register2;
	return registers;
}

int GetRegisterNum(string reg)
{
	if(reg == "A")
		return 0;
	else if(reg == "X")
		return 1;
	else if(reg == "L")
		return 2;
	else if(reg == "B")
		return 3;
	else if(reg == "S")
		return 4;
	else if(reg == "T")
		return 5;
	else if(reg == "F")
		return 6;
	else if(reg == "PC")
		return 8;
	else if(reg == "SW")
		return 9;
	return -1;
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

int HexToInt(char* string)
{
	return (int)strtol(string, NULL, 16);
}

string IntToHex(int num)
{
	char* hex = new char[10];
	sprintf(hex, "%x", num);
	string hexString(hex);
	std::transform(hexString.begin(), hexString.end(), hexString.begin(), ::toupper);
	return hexString;
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

string PadWithZeros(string str, int strLen, int maxNum)
{
	string newString;
	while(strLen < maxNum)
	{
		newString += "0";
		strLen++;
	}
	newString += str;
	return newString;
}