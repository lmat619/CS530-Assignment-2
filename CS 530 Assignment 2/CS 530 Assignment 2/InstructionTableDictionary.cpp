#include <new>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>

#include "InstructionTableDictionary.h"

using namespace std;

void LoadDictionary()
{
	InstructionTable.insert(make_pair("ADD", Mnemonic("ADD", "18", true, false, false, true, true)));
	InstructionTable.insert(make_pair("ADDF", Mnemonic("ADDF", "58", false, false, false, true, true)));
	InstructionTable.insert(make_pair("ADDR", Mnemonic("ADDR", "90", false, false, true, false, false)));
	InstructionTable.insert(make_pair("AND", Mnemonic("AND", "40", true, false, false, true, true)));
	InstructionTable.insert(make_pair("CLEAR", Mnemonic("CLEAR", "B4", false, false, true, false, false)));
	InstructionTable.insert(make_pair("COMP", Mnemonic("COMP", "28", true, false, false, true, true)));
	InstructionTable.insert(make_pair("COMPF", Mnemonic("COMPF", "88", false, false, false, true, true)));
	InstructionTable.insert(make_pair("COMPR", Mnemonic("COMPR", "A0", false, false, true, false, false)));
	InstructionTable.insert(make_pair("DIV", Mnemonic("DIV", "24", true, false, false, true, true)));
	InstructionTable.insert(make_pair("DIVF", Mnemonic("DIVF", "64", false, false, false, true, true)));
	InstructionTable.insert(make_pair("DIVR", Mnemonic("DIVR", "9C", false, false, true, false, false)));
	InstructionTable.insert(make_pair("FIX", Mnemonic("FIX", "C4", false, true, false, false, false)));
	InstructionTable.insert(make_pair("FLOAT", Mnemonic("FLOAT", "C0", false, true, false, false, false)));
	InstructionTable.insert(make_pair("HIO", Mnemonic("HIO", "F4", false, true, false, false, false)));
	InstructionTable.insert(make_pair("J", Mnemonic("J", "3C", true, false, false, true, true)));
	InstructionTable.insert(make_pair("JEQ", Mnemonic("JEQ", "30", true, false, false, true, true)));
	InstructionTable.insert(make_pair("JGT", Mnemonic("JGT", "34", true, false, false, true, true)));
	InstructionTable.insert(make_pair("JLT", Mnemonic("JLT", "38", true, false, false, true, true)));
	InstructionTable.insert(make_pair("JSUB", Mnemonic("JSUB", "48", true, false, false, true, true)));
	InstructionTable.insert(make_pair("LDA", Mnemonic("LDA", "00", true, false, false, true, true)));
	InstructionTable.insert(make_pair("LDB", Mnemonic("LDB", "68", false, false, false, true, true)));
	InstructionTable.insert(make_pair("LDCH", Mnemonic("LDCH", "50", true, false, false, true, true)));
	InstructionTable.insert(make_pair("LDF", Mnemonic("LDF", "70", false, false, false, true, true)));
	InstructionTable.insert(make_pair("LDL", Mnemonic("LDL", "08", true, false, false, true, true)));
	InstructionTable.insert(make_pair("LDS", Mnemonic("LDS", "6C", false, false, false, true, true)));
	InstructionTable.insert(make_pair("LDT", Mnemonic("LDT", "74", false, false, false, true, true)));
	InstructionTable.insert(make_pair("LDX", Mnemonic("LDX", "04", true, false, false, true, true)));
	InstructionTable.insert(make_pair("LPS", Mnemonic("LPS", "D0", false, false, false, true, true)));
	InstructionTable.insert(make_pair("MUL", Mnemonic("MUL", "20", true, false, false, true, true)));
	InstructionTable.insert(make_pair("MULF", Mnemonic("MULF", "60", false, false, false, true, true)));
	InstructionTable.insert(make_pair("MULR", Mnemonic("MULR", "98", false, false, true, false, false)));
	InstructionTable.insert(make_pair("NORM", Mnemonic("NORM", "C8", false, true, false, false, false)));
	InstructionTable.insert(make_pair("OR", Mnemonic("OR", "44", true, false, false, true, true)));
	InstructionTable.insert(make_pair("RD", Mnemonic("RD", "D8", true, false, false, true, true)));
	InstructionTable.insert(make_pair("RMO", Mnemonic("RMO", "AC", false, false, true, false, false)));
	InstructionTable.insert(make_pair("RSUB", Mnemonic("RSUB", "4C", false, false, false, true, true)));
	InstructionTable.insert(make_pair("SHIFTL", Mnemonic("SHIFTL", "A4", false, false, true, false, false)));
	InstructionTable.insert(make_pair("SHIFTR", Mnemonic("SHIFTR", "A8", false, false, true, false, false)));
	InstructionTable.insert(make_pair("SIO", Mnemonic("SIO", "F0", false, true, false, false, false)));
	InstructionTable.insert(make_pair("SSK", Mnemonic("SSK", "EC", false, false, false, true, true)));
	InstructionTable.insert(make_pair("STA", Mnemonic("STA", "0C", true, false, false, true, true)));
	InstructionTable.insert(make_pair("STB", Mnemonic("STB", "78", false, false, false, true, true)));
	InstructionTable.insert(make_pair("STCH", Mnemonic("STCH", "54", true, false, false, true, true)));
	InstructionTable.insert(make_pair("STF", Mnemonic("STF", "80", false, false, false, true, true)));
	InstructionTable.insert(make_pair("STI", Mnemonic("STI", "D4", false, false, false, true, true)));
	InstructionTable.insert(make_pair("STL", Mnemonic("STL", "14", true, false, false, true, true)));
	InstructionTable.insert(make_pair("STS", Mnemonic("STS", "7C", false, false, false, true, true)));
	InstructionTable.insert(make_pair("STSW", Mnemonic("STSW", "E8", true, false, false, true, true)));
	InstructionTable.insert(make_pair("STT", Mnemonic("STT", "84", false, false, false, true, true)));
	InstructionTable.insert(make_pair("STX", Mnemonic("STX", "10", true, false, false, true, true)));
	InstructionTable.insert(make_pair("SUB", Mnemonic("SUB", "1C", true, false, false, true, true)));
	InstructionTable.insert(make_pair("SUBF", Mnemonic("SUBF", "5C", false, false, false, true, true)));
	InstructionTable.insert(make_pair("SUBR", Mnemonic("SUBR", "94", false, false, true, false, false)));
	InstructionTable.insert(make_pair("SVC", Mnemonic("SVC", "B0", false, false, true, false, false)));
	InstructionTable.insert(make_pair("TD", Mnemonic("TD", "E0", true, false, false, true, true)));
	InstructionTable.insert(make_pair("TIO", Mnemonic("TIO", "F8", false, true, false, false, false)));
	InstructionTable.insert(make_pair("TIX", Mnemonic("TIX", "2C", true, false, false, true, true)));
	InstructionTable.insert(make_pair("TIXR", Mnemonic("TIXR", "B8", false, false, true, false, false)));
	InstructionTable.insert(make_pair("WD", Mnemonic("WD", "DC", true, false, false, true, true)));
}
/*

map<string, string> dictionary;

// adding
dictionary.insert(make_pair("foo", "bar"));

// searching
map<string, string>::iterator it = dictionary.find("foo");
if(it != dictionary.end())
cout << "Found! " << it->first << " is " << it->second << "\n";
*/