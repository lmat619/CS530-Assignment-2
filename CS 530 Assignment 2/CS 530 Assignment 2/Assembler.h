#include <iostream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

void Pass1(std::string Path);
void Pass2();
string GenerateObjectCode(int currentPC, string currentOpCode, string currentLabel, string currentOperand, string currentLiteral);
void GetLabel(char* dest, char* line);
void GetOpCode(char* dest, char* line);
void GetOperand(char* dest, char* line);
int HexToInt(char* string);
string IntToHex(int num);
void TrimEnd(char *string);
void RemoveEndApostrophe(char *string);
void RemoveOperandType(char *string);
string PadWithZeros(string str, int strLen, int maxNum);