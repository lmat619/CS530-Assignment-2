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
WordByteType userHex[500];
int ModRecordCounter = 0;
int PC = 0;
int StartingAddress = 0;
int IndexCount = -1;
int ProgramLength;
int reserveByteCount = 0;
int useModRecords = 0;
int badLineError = 0;
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
		badLineError = 0;
		//Check if line is a comment
		if(line [0] != '.' && line[0] != 10 && line[0] != '\0')
		{
			TrimEnd(line);
			string lineS(line);
			if (lineS == "")
			{
				continue;
			}
			if (lineS.find(".") != string::npos)
			{				
				lineS = lineS.substr(0, lineS.find_first_of("."));
			}
			strcpy(line, lineS.c_str());
			while (line[0] == '\r')
			{
				char *ptr = line;
				ptr++;
				strcpy(line, ptr);
			}
			Label[0] = 0;
			OpCode[0] = 0;
			Operand[0] = 0;
			GetLabel(Label, line);
			GetOpCode(OpCode, line);
			GetOperand(Operand, line);			

			if (!strcmp(OpCode, "") || badLineError == 1) 
			{
				continue;
			}
			IndexCount++;
			//Check if OpCode is directive
			if(!strcmp(OpCode, "START"))
			{
				PC = StartingAddress = HexToInt(Operand);
				if (StartingAddress != 0)
					useModRecords = 0;
				else
					useModRecords = 1;
			}

			//Check if line contains a label
			if(strcmp(Label,"") && strcmp(Label, " "))
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
				LitArray[IndexCount] = "";
				continue;
			}
			else if (!strcmp(OpCode, "END"))
			{
				PCArray[IndexCount] = PC;
				OpCodeArray[IndexCount] = OpCode;
				LabelArray[IndexCount] = Label;
				OperandArray[IndexCount] = Operand;
				LitArray[IndexCount] = "";
				break;
			}
			else if (!strcmp(OpCode, "RESB"))
			{
				int value = atoi(Operand);
				PC += value;
			}
			else if (!strcmp(OpCode, "RESW"))
			{
				TrimEnd(Operand);
				int value = atoi(Operand);
				PC += (value*3);
			}
			else if (!strcmp(OpCode,  "BYTE"))
			{
				if(Operand[0] == 'X' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					PC += (strlen(Operand) / 2);
					userHex[IndexCount] = Hex;
					//PC += 1;
				}
				else if(Operand[0] == 'C' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					PC += strlen(Operand);
					userHex[IndexCount] = Char;
				}
				else
				{
					userHex[IndexCount] = Base10;
					PC += 1;
				}
			}
			else if (!strcmp(OpCode, "WORD"))
			{
				if(Operand[0] == 'X' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					userHex[IndexCount] = Hex;
					//PC += 1;
				}
				else if(Operand[0] == 'C' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					userHex[IndexCount] = Char;
				}
				else
					userHex[IndexCount] = Base10;
				PC += 3;
			}
			else if (!strcmp(OpCode, "EQU"))
			{
				if(Operand[0] == 'X' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					userHex[IndexCount] = Hex;
					PC += (strlen(Operand) / 2);

					//Add Symbol (done below)
				}
				else if(Operand[0] == 'C' && Operand[1] == '\'')
				{
					RemoveEndApostrophe(Operand);
					RemoveOperandType(Operand);
					userHex[IndexCount] = Char;
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
				char* InsTest = new char[strlen(OpCode)];
				strcpy(InsTest, OpCode);
				std::string MnemonicTest(InsTest);
				if(InsTest[0] == '+')
					MnemonicTest = ++InsTest;
				map<std::string, Mnemonic>::iterator it = InstructionTable.find(MnemonicTest);
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
					{
						if(Operand[0] == '#' || Operand[0] == '@')
							machineType = XE;
						PC += 3;
					}
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
			OpCodeArray[IndexCount] = string(OpCode);
			LabelArray[IndexCount] = string(Label);
			OperandArray[IndexCount] = string(Operand);
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

	for (int currentLineIndex = 0; currentLineIndex <= IndexCount; currentLineIndex++)
	{
		if (OpCodeArray[currentLineIndex] == "EQU")
		{
			if (userHex[IndexCount] = Char)
			{

			}
			else if (userHex[IndexCount] = Hex)
			{

			}
			else if (OperandArray[IndexCount] != "*")
			{
				if (OperandArray[IndexCount].find_first_of("-") || OperandArray[IndexCount].find_first_of("+") ||
					OperandArray[IndexCount].find_first_of("/") || OperandArray[IndexCount].find_first_of("*") )
				{

				}
			}
			//write math
			//get label address
		}
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
		WordByteType currentUserHex = userHex[currentLineIndex];
		int lastPC = 0;
		if (currentLineIndex != 0)
			lastPC = PCArray[currentLineIndex - 1];

		if(currentOpCode == "START")
		{
			output += "H";
			while (currentLabel.length() < 6)
				currentLabel += " ";
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
			//write mods
			for (int i = 0; i < ModRecordCounter; i++)
			{
				output += ModRecArray[i] + "\n";
			}
			//E^startingaddressinhex
			string starting = IntToHex(StartingAddress);
			while (starting.length() < 6)
			{
				starting = "0" + starting;
			}
			output += "E";
			output += starting;
			output += "\n";			
			break;
		}
		string objectCode = GenerateObjectCode(currentPC, currentOpCode, currentLabel, currentOperand, currentLiteral, currentUserHex, lastPC);

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
			textRecordStartAddress = PCArray[currentLineIndex - 1] + reserveByteCount;
		}
		else
		{
			textRecord += objectCode;
		}
	}
	PrintToFile(output);
}

string GenerateObjectCode(int currentPC, string currentOpCode, string currentLabel, string currentOperand, string currentLiteral, WordByteType currentUserHex, int lastPC)
{
	reserveByteCount = 0;
	string objCode("");
	if(currentOpCode == "WORD")
	{
		if (currentUserHex == Hex)
		{
			while (currentOperand.length() < 6)
				currentOperand = "0" + currentOperand;
			objCode = currentOperand;
		}
		else if (currentUserHex == Char)
		{
			string val("");
			for(int i = 0; i < currentOperand.length(); i++)
				val += IntToHex((int)currentOperand[i]);
			while (val.length() < 6)
				val = "0" + val;
			objCode = val;
		}
		else
		{
			string val("");
			val = IntToHex(atoi(currentOperand.c_str()));
			while (val.length() < 6)
				val = "0" + val;
			objCode = val;
		}
	}
	else if(currentOpCode == "BYTE")
	{
		if (currentUserHex == Hex)
		{
			objCode = currentOperand;
		}
		else if (currentUserHex == Char)
		{
			string val("");
			for(int i = 0; i < currentOperand.length(); i++)
				val += IntToHex((int)currentOperand[i]);
			objCode = val;
		}
		else
		{
			string val("");
			val = IntToHex(atoi(currentOperand.c_str()));	
			objCode = val;
		}
	}	
	else if(currentOpCode == "RESW" || currentOpCode == "RESB")
	{ 
		reserveByteCount = atoi(currentOperand.c_str());
		return objCode;
	}
	else
	{
		int xbpe = 0;
		//get from dictionary
		Mnemonic currentMnemonic;
		//Check if OpCode exists in table
		std::string MnemonicTest = currentOpCode;
		if(currentOpCode[0] == '+')
			MnemonicTest = MnemonicTest.substr(1);
		map<std::string, Mnemonic>::iterator it = InstructionTable.find(MnemonicTest);
		//If it exists, process line
		if(it != InstructionTable.end())
		{
			currentMnemonic = it->second;
			if(currentOpCode[0] == '+')
			{
				//format 4
				//Add the modification records
				if (useModRecords)
				{
					if (currentOperand.find("#") != string::npos)
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
							std::string address = IntToHex(lastPC + 1);
							while (address.length() < 6)
								address = "0" + address;
							ModRecArray[ModRecordCounter++] = "M" + address + "05";
						}
					}
					else
					{						

						std::string address = IntToHex(lastPC + 1);
						while (address.length() < 6)
							address = "0" + address;
						ModRecArray[ModRecordCounter++] = "M" + address + "05";						
					}
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
				else if(currentOperand.find(",") != string::npos)
				{
					//nixbpe
					//111001					
					OpCode += 3;
					objCode += IntToHex(OpCode);
					objCode += "9";
					currentOperand = currentOperand.substr(0, currentOperand.find_first_of(","));
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
				else if(currentOperand.find("@") != string::npos)
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
				else if(currentOperand.find("#") != string::npos)
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
				if (currentMnemonic.mnemonic == "RSUB")
				{
					objCode = "4C0000";
				}
				else if(machineType == Basic)
				{
					//ni
					//00
					int isIndexed = 0;
					int add;
					if(currentOperand.find(",") != string::npos)
					{
						currentOperand = currentOperand.substr(0, currentOperand.find_first_of(","));
						isIndexed = 1;
					}
					map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
					//If it does not exist, add literal to table
					if(it != SymbolTable.end())
					{
						string address("");
						add = it->second.Address;
						if (useModRecords)
						{
							std::string address = IntToHex(lastPC + 1);
							while (address.length() < 6)
								address = "0" + address;
							ModRecArray[ModRecordCounter++] = "M" + address + "04";
						}
						address = IntToHex(add);
							while(address.length() < 4)
								address = "0" + address;
						if(isIndexed == 1)
						{
							char hexVal = address[0];
							int firstBit = (hexVal >= 'A') ? (hexVal - 'A' + 10) : (hexVal - '0');
							firstBit += 8;
							string bit = IntToHex(firstBit);
							char newVal = bit[0];
							address[0] = newVal;
						}
						objCode += currentMnemonic.Opcode + address;
					}
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
					else if(currentOperand.find(",") != string::npos)
					{
						//nixbpe
						//111010					
						OpCode += 3;
						objCode += IntToHex(OpCode);
						objCode += "10";
						currentOperand = currentOperand.substr(0, currentOperand.find_first_of(","));
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
					else if(currentOperand.find("@") != string::npos)
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
					else if(currentOperand.find("#") != string::npos)
					{
						//nixbpe	
						//010010
						OpCode += 1;
						objCode += IntToHex(OpCode);
						objCode += "2";
						currentOperand = currentOperand.substr(1);
						map<std::string, Symbol>::iterator it = SymbolTable.find(currentOperand);
						//#Symbol found
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
							objCode += address;
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
	reg1[i] = '\0';
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
	for(i = 0; i < 6; i++)
	{
		if (line[i] != '\0')
			dest[i] = line[i];
	}
	dest[i] = '\0';
	TrimBegin(dest);
	TrimEnd(dest);
}

void GetOpCode(char* dest, char* line)
{
	int i = 0;
	int checkForBadLine = 0;
	for (int j = 0 ; j < 9; j++)
	{
		if (line[j] == '\0')
			checkForBadLine = badLineError = 1; 
	}
	if (checkForBadLine == 0)
	{
		for(i = 0; i < 7; i++)
		{
			if (line[i+8] != '\0')
				dest[i] = line[i+8];
		}
	}
	dest[i] = '\0';
	TrimBegin(dest);
	TrimEnd(dest);
}

void GetOperand(char* dest, char* line)
{
	int i = 0;
	int checkForBadLine = 0;
	for (int j = 0 ; j < 16; j++)
	{
		if (line[j] == '\0')
			checkForBadLine = badLineError = 1;
	}
	if (checkForBadLine == 0)
	{
		for(i = 0; i < 19; i++)
		{
			dest[i] = line[i+16];
			if(dest[i] == '\0')
				break;
		}
	}
	dest[i] = '\0';
	TrimBegin(dest);
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

void TrimBegin(char *string)
{
	char *ptr = string;
	for (int i = 0; i < strlen(string); ptr++)
	{
		if (*ptr != ' ' && *ptr != '\t' && *ptr != '\n')
			break;
	}
	if (*ptr != '\0')
		strcpy(string, ptr);
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
