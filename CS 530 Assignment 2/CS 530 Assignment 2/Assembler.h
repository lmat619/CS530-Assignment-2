#include <iostream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

void Pass1(std::string Path);
void Pass2(std::string Path);
void GetLabel(char* dest, char* line);
void GetOpCode(char* dest, char* line);
void GetOperand(char* dest, char* line);
int stringToInt(char* string);
void TrimEnd(char *string);
void RemoveEndApostrophe(char *string);
void RemoveOperandType(char *string);