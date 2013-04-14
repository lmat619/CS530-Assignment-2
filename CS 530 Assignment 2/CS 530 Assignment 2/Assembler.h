#include <iostream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

enum MachineType
{
	Basic = 0,
	XE = 1
};

enum WordByteType
{
	Hex = 0,
	Char = 1,
	Base10 = 3
};

#define READ "r"
#define MAX_RECORD_SIZE 68
#define MAX_LABEL_SIZE 8
#define MAX_OPCODE_SIZE 8
#define MAX_OPERAND_SIZE 50

void Pass1(std::string Path);
void Pass2();
string GenerateObjectCode(int currentPC, string currentOpCode, string currentLabel, string currentOperand, string currentLiteral, WordByteType currentUserHex, int lastPC);
std::vector<std::string> SplitCommas(string operand);
void PrintToFile(string output);
int GetRegisterNum(string reg);
void GetLabel(char* dest, char* line);
void GetOpCode(char* dest, char* line);
void GetOperand(char* dest, char* line);
int HexToInt(char* string);
string IntToHex(int num);
void TrimBegin(char *string);
void TrimEnd(char *string);
void RemoveEndApostrophe(char *string);
void RemoveOperandType(char *string);
string PadWithZeros(string str, int strLen, int maxNum);