#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

using namespace std;

struct Symbol
{
	std::string Name;
	int Address;

	Symbol::Symbol()
	{ }

	Symbol::Symbol(const string name, int address)
	{
		std::string sym(name);
		this->Name = sym;
		this->Address = address;
	}
};

map<std::string, Symbol> SymbolTable;