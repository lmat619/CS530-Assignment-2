#include <new>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <algorithm>
#include <vector>
#include <fstream>
#include "InstructionTableDictionary.h"
#include "SymbolTableDictionary.h"
#include "LiteralTableDictionary.h"
#include "Assembler.h";

using namespace std;

MachineType machineType = Basic;
int PCArray[500];
string OpCodeArray[500];
string LabelArray[500];
string OperandArray[500];
string LitArray[500];
string ModRecArray[500];
bool userHex[500];
int ModRecordCounter = 0;
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
			
			string lineS(line);
			if (lineS.find(".") != string::npos)
			{
				lineS = lineS.substr(0, lineS.find_first_of(".") - 1);
			}
			strcmp(line, lineS.c_str());

			GetLabel(Label, line);
			GetOpCode(OpCode, line);
			GetOperand(Operand, line);			

			//Check if OpCode is directive
			if(!strcmp(OpCode, "START"))
			{
				PC = StartingAddress = HexToInt(Operand);
			}
			
			//Check if line contains a label
			if(strcmp(Label,""))
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

			if(!strcmp(OpCode, "START"))
			{
				PCArray[IndexCount] = PC;
				OpCodeArray[IndexCount] = OpCode;
				LabelArray[IndexCount] = Label;
				OperandArray[IndexCount] = Operand;
				continue;
			}
			else if (!strcmp(OpCode, "END"))
			{
				PCArray[IndexCount] = PC;
				OpCodeArray[IndexCount] = OpCode;
				LabelArray[IndexCount] = Label;
				OperandArray[IndexCount] = Operand;
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
					userHex[IndexCount] = true;
					//PC += 1;
				}
				else if(Operand[0] == 'C' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					PC += strlen(Operand);
					userHex[IndexCount] = false;
				}
				else
					PC += 1;
			}
			else if (!strcmp(OpCode, "WORD"))
			{
				if(Operand[0] == 'X' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					userHex[IndexCount] = true;
					//PC += 1;
				}
				else if(Operand[0] == 'C' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					userHex[IndexCount] = false;
				}
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
						LitArray[IndexCount] = Operand;
						char* OperandCopy = new char[MAX_OPERAND_SIZE];
						strcpy(OperandCopy, Operand);
						//Get rid of '='
						OperandCopy++;

						map<std::string, Literal>::iterator it = LiteralTable.find(OperandCopy);
						//If it does not exist, add literal to table
						if(it == LiteralTable.end())
						{
							std::string litName(OperandCopy);
							if(OperandCopy[0] == 'C')
							{
								RemoveEndApostrophe(OperandCopy);
								RemoveOperandType(OperandCopy);
								int length = strlen(OperandCopy);
								string op(OperandCopy);
								string val("");
								for(int i = 0; i < op.length(); i++)
									val += IntToHex((int)op[i]);
								LiteralTable.insert(make_pair(litName, Literal(litName, val, length)));
							}
							else if(OperandCopy[0] == 'X')
							{
								RemoveEndApostrophe(OperandCopy);
								RemoveOperandType(OperandCopy);
								int length = (strlen(OperandCopy) / 2);
								string val(OperandCopy);
								LiteralTable.insert(make_pair(litName, Literal(litName, val, length)));
							}
						}
					}
					else
						LitArray[IndexCount] = "";
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

	//Get address values for each literal
	typedef map<std::string, Literal>::iterator it;
	for(it iterator = LiteralTable.begin(); iterator != LiteralTable.end(); iterator++)
	{
		Literal* lit = &(iterator->second);
		PC += lit->Length;
		lit->Address = PC;
	}

	ProgramLength = PC - StartingAddress;
}

void Pass2()
{
	string output("");
	string textRecord("");
	int textRecordStartAddress = StartingAddress;
	int currentLineIndex;
	for (currentLineIndex = 0; currentLineIndex <= IndexCount; currentLineIndex++)
	{
		int currentPC = PCArray[currentLineIndex];
		string currentOpCode = OpCodeArray[currentLineIndex];
		string currentLabel = LabelArray[currentLineIndex];
		string currentOperand = OperandArray[currentLineIndex];
		string currentLiteral = LitArray[currentLineIndex];
		bool currentUserHex = userHex[currentLineIndex];
		if(currentOpCode == "START")
		{
			output += "H";
			while (currentLabel.length() < 6)
				currentLabel = "0" + currentLabel;
			while(currentOperand.length() < 6)
				currentOperand = "0" + currentOperand;
			string progLength = IntToHex(ProgramLength);
			while(progLength.length() < 6)
				progLength = "0" + progLength;

			output  += currentLabel + currentOperand + progLength + "\n";
			continue;
		}
		if(currentOpCode == "END")
		{
			if (textRecord != "")
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
				textRecord = "";
			}
			//do stuff
			//E^startingaddressinhex
			string starting = IntToHex(StartingAddress);
			while (starting.length() < 6)
			{
				starting = "0" + starting;
			}
			output += "E";
			output += starting;
			output += "\n";

			//write mods
			for (int i = 0; i < ModRecordCounter; i++)
			{
				output += ModRecArray[i] + "\n";
			}
			break;
		}
		string objectCode = GenerateObjectCode(currentPC, currentOpCode, currentLabel, currentOperand, currentLiteral, currentUserHex);

		if((textRecord.length() + objectCode.length()) > 60 || currentOpCode == "RESW" || currentOpCode == "RESB")
		{
			if (textRecord != "")
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
			}
			//start new text record			
			textRecord = objectCode;
			//textRecordStartAddress = currentPC;
			textRecordStartAddress = PCArray[currentLineIndex + 1];
		}
		else
		{
			textRecord += objectCode;
		}
	}
	//cout << output;
	PrintToFile(output);
}

string GenerateObjectCode(int currentPC, string currentOpCode, string currentLabel, string currentOperand, string currentLiteral, bool currentUserHex)
{
	string objCode("");
	if(currentOpCode == "WORD")
	{
		if (currentUserHex)
		{
			currentOperand += "000000";
			objCode = currentOperand.substr(0, 6);
		}
		else
		{
			string val("");
			for(int i = 0; i < currentOperand.length(); i++)
				val += IntToHex((int)currentOperand[i]);
			val += "000000";
			objCode = val.substr(0, 6);
		}
	}
	else if(currentOpCode == "BYTE")
	{
		if (currentUserHex)
		{
			objCode = currentOperand;
		}
		else
		{
			string val("");
			for(int i = 0; i < currentOperand.length(); i++)
				val += IntToHex((int)currentOperand[i]);
			objCode = val;
		}
	}	
	else if(currentOpCode == "RESW" || currentOpCode == "RESB")
	{ 
		return objCode;
	}
	else
	{
		int xbpe = 0;
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

				//Add the modification records
				if (currentOperand.find("#"))
				{					
					int foundLabel = -1;
					for (int i = 0; i < IndexCount ; i++)
					{
						if (LabelArray[i] == currentOperand.substr(1))
						{
							foundLabel = i;
							break;
						}
					}
					if (foundLabel != -1)
					{
						ModRecArray[ModRecordCounter++] = "M^" + IntToHex(PCArray[foundLabel]) + "^05";
					}
				}
				else
				{
					ModRecArray[ModRecordCounter++] = "M^" + IntToHex(currentPC) + "^05";
				}
				
				char* op1 = new char[MAX_OPCODE_SIZE];
				strcpy(op1, currentMnemonic.Opcode.c_str());
				int OpCode = HexToInt(op1);

				if (currentLiteral != "")
				{
					//nixbpe
					//110001
					OpCode += 3;
					objCode += IntToHex(OpCode);
					objCode += "1";
					map<std::string, Literal>::iterator it = LiteralTable.find(currentLiteral);
					//If it does not exist, add literal to table
					if(it != LiteralTable.end())
					{
						string address = IntToHex(it->second.Address);
						while (address.length() < 5)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}
				else if(currentOperand.find(","))
				{
					//nixbpe
					//111001					
					OpCode += 3;
					objCode += IntToHex(OpCode);
					objCode += "9";
					currentOperand = currentOperand.substr(0, currentOperand.find_first_of(",") - 1);
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address = IntToHex(it->second.Address);
						while (address.length() < 5)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}				
				//get n & i bits
				else if(currentOperand.find("@"))
				{				
					//nixbpe
					//100001
					OpCode += 2;
					objCode += IntToHex(OpCode);
					objCode += "1";
					currentOperand = currentOperand.substr(1);
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address = IntToHex(it->second.Address);
						while (address.length() < 5)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}
				else if(currentOperand.find("#"))
				{
					//nixbpe	
					//010001
					OpCode += 1;
					objCode += IntToHex(OpCode);
					objCode += "1";
					currentOperand = currentOperand.substr(1);
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address = IntToHex(it->second.Address);
						while (address.length() < 5)
						{
							address = "0" + address;
						}
						objCode += address;
					}
					else
					{						
						string address = IntToHex(atoi(currentOperand.c_str()));
						while (address.length() < 5)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}
				else
				{
					//nixbpe
					//110001
					OpCode += 3;
					objCode += IntToHex(OpCode);
					objCode += "1";					
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address = IntToHex(it->second.Address);
						while (address.length() < 5)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}
			}
			else if(currentMnemonic.isFormat1)
			{
				objCode = currentMnemonic.Opcode;
			}
			else if(currentMnemonic.isFormat0 || currentMnemonic.isFormat3)
			{
				if(machineType == Basic)
				{

				}
				else
				{
					int disp = 0;
					char* op1 = new char[MAX_OPCODE_SIZE];
				strcpy(op1, currentMnemonic.Opcode.c_str());
				int OpCode = HexToInt(op1);

				if (currentLiteral != "")
				{
					//nixbpe
					//110010
					OpCode += 3;
					objCode += IntToHex(OpCode);
					objCode += "2";
					map<std::string, Literal>::iterator it = LiteralTable.find(currentLiteral);
					//If it does not exist, add literal to table
					if(it != LiteralTable.end())
					{
						string address = IntToHex(it->second.Address - currentPC);
						while (address.length() < 3)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}
				else if(currentOperand.find(","))
				{
					//nixbpe
					//111010					
					OpCode += 3;
					objCode += IntToHex(OpCode);
					objCode += "10";
					currentOperand = currentOperand.substr(0, currentOperand.find_first_of(",") - 1);
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address = IntToHex(it->second.Address - currentPC);
						while (address.length() < 3)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}				
				//get n & i bits
				else if(currentOperand.find("@"))
				{				
					//nixbpe
					//100010
					OpCode += 2;
					objCode += IntToHex(OpCode);
					objCode += "2";
					currentOperand = currentOperand.substr(1);
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address = IntToHex(it->second.Address - currentPC);
						while (address.length() < 3)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}
				else if(currentOperand.find("#"))
				{
					//nixbpe	
					//010010
					OpCode += 1;
					objCode += IntToHex(OpCode);
					objCode += "2";
					currentOperand = currentOperand.substr(1);
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address = IntToHex(it->second.Address - currentPC);
						while (address.length() < 3)
						{
							address = "0" + address;
						}
						objCode += address;
					}
					else
					{
						string address = IntToHex(atoi(currentOperand.c_str()));
						while (address.length() < 3)
							address = "0" + address;
					}
				}
				else
				{
					//nixbpe
					//110010
					OpCode += 3;
					objCode += IntToHex(OpCode);
					objCode += "2";					
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address = IntToHex(it->second.Address - currentPC);
						while (address.length() < 5)
						{
							address = "0" + address;
						}
						objCode += address;
					}
				}

				}
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

void PrintToFile(string output)
{
	//Export objectFile
	ofstream outFile;
	outFile.open(".\\output");
	outFile << output;
	outFile.close();

	////Export Symbol Table
	outFile.open(".\\symbolTable");
	map<string, Symbol>::iterator symbolIt;
	outFile << "Name\tAddress\n\n";
	for(symbolIt = SymbolTable.begin(); symbolIt != SymbolTable.end(); symbolIt++)
	{
		Symbol symbol = symbolIt->second;
		outFile << symbol.Name << "\t" << IntToHex(symbol.Address) << "\n";
	}
	outFile.close();

	////Export Literal Table
	outFile.open(".\\literalTable");
	map<string, Literal>::iterator literalIt;
	outFile << "Name\tValue\tLength\tAddress\n\n";
	for(literalIt = LiteralTable.begin(); literalIt != LiteralTable.end(); literalIt++)
	{
		Literal lit = literalIt->second;
		outFile << lit.Name << "\t" << lit.Value << "\t" << lit.Length << "\t" << IntToHex(lit.Address) << "\n";
	}
	outFile.close();
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
		if(*ptr == ' ' || *ptr == '\t' || *ptr == '\n')
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

void RemoveOperandType(char* string)
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
