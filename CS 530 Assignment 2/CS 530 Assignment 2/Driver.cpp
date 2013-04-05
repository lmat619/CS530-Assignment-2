#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include "InstructionTableDictionary.h"
#include "Assembler.h";

using namespace std;

string IntToHex(int num);

int main(int argc, char* argv[])
{
	std::string filePath;
	/*if(argc > 1)
		std::string filePath(argv[1]);
	else
	{
		cout << "Please type in the file path of a SIC Basic or XE source code" << endl;
		getline(cin, filePath);
	}*/

	filePath = ".\\assembleTest.txt";
	LoadDictionary();
	try
	{
		Pass1(filePath);
		Pass2();
	}
	catch (char *error)
	{
		printf("Error: %s", error);
	}

	system("PAUSE");
	exit(0);
}

string IntToHex(int num)
{
	char* hex = new char[10];
	sprintf(hex, "%x", num);
	string hexString(hex);
	std::transform(hexString.begin(), hexString.end(), hexString.begin(), ::toupper);
	return hexString;
}