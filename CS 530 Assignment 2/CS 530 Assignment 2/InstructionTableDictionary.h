#include <iostream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

struct Mnemonic
{
	std::string mnemonic;
	bool isFormat0;
	bool isFormat1;
	bool isFormat2;
	bool isFormat3;
	bool isFormat4;
	std::string Opcode;

	Mnemonic::Mnemonic()
	{ }

	Mnemonic::Mnemonic(const string mnemonic, const string opCode, bool format0, bool format1, bool format2, bool format3, bool format4)
	{
		std::string name(mnemonic);
		std::string code(opCode);
		this->mnemonic = name;
		this->Opcode = code;
		this->isFormat0 = format0;
		this->isFormat1 = format1;
		this->isFormat2 = format2;
		this->isFormat3 = format3;
		this->isFormat4 = format4;
	}
};

map<std::string, Mnemonic> InstructionTable;

void LoadDictionary();