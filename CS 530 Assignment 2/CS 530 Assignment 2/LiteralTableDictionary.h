#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

using namespace std;

struct Literal
{
	std::string Name;
	int Address;
	string Value;
	int Length;

	Literal::Literal()
	{ }

	Literal::Literal(const string name, const string value, int length)
	{
		std::string sym(name);
		std::string val(value);
		this->Name = sym;
		this->Value = val;
		this->Length = length;
	}
};

map<std::string, Literal> LiteralTable;