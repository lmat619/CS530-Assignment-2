#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "InstructionTableDictionary.h"
#include "Assembler.h";

using namespace std;

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
	filePath = "C:\\Users\\Louie\\Desktop\\CS 530 Assignment 2 Help\\assembleTest.txt";
	LoadDictionary();
	Pass1(filePath);

	system("PAUSE");
}