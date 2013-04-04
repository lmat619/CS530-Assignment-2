//
// Dmitry Brant
// Assignment 4, CIS 335, Prof. Jackie Woldering
// EXTRA CREDIT #1, EXTRA CREDIT #2
//
//-------------------------------------
//#include <vcl.h>    //Borland's collection of useful classes
//I will use Borland's string class, and their TList class, which
//is a hashed linked list
#pragma hdrstop

// Although I like the syntax of C++, I always feel like going back
// to the good old C library routines. So I'll mix and match a little.
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <list>

//my typedefs for representing symbols and literals...

typedef struct{
	char name[7];
	int address;
}SIC_Symbol;

typedef struct{
	char name[12];
	int offsetFromLTORG;
	int LTORG_Num;
}SIC_Literal;

//initialize opcode table
struct SIC_Opcode{ char name[7]; unsigned char code; int format; };
const struct SIC_Opcode SIC_Opcodes [] = {
	{ "ADD", 0x18, 3 },       { "ADDR", 0x90, 2 },    { "AND", 0x40, 3 },
	{ "CLEAR", 0xB4, 2 },     { "COMP", 0x28, 3 },    { "COMPR", 0xA0, 2 },
	{ "DIV", 0x24, 3 },       { "DIVR", 0x9C, 2 },    { "HIO", 0xF4, 1 },
	{ "J", 0x3C, 3 },         { "JEQ", 0x30, 3 },     { "JGT", 0x34, 3 },
	{ "JLT", 0x38, 3 },       { "JSUB", 0x48, 3 },    { "LDA", 0x0, 3 },
	{ "LDB", 0x68, 3 },       { "LDCH", 0x50, 3 },    { "LDL", 0x8, 3 },
	{ "LDS", 0x6C, 3 },       { "LDT", 0x74, 3 },     { "LDX", 0x4, 3 },
	{ "LPS", 0xD0, 3 },       { "MUL", 0x20, 3 },     { "MULR", 0x98, 2 },
	{ "OR", 0x44, 3 },        { "RD", 0xD8, 3 },      { "RMO", 0xAC, 2 },
	{ "RSUB", 0x4C, 3 },      { "SHIFTL", 0xA4, 2 },  { "SHIFTR", 0xA8, 2 },
	{ "SIO", 0xF0, 1 },       { "SSK", 0xEC, 3 },     { "STA", 0x0C, 3 },
	{ "STB", 0x78, 3 },       { "STCH", 0x54, 3 },    { "STI", 0xD4, 3 },
	{ "STL", 0x14, 3 },       { "STS", 0x7C, 3 },     { "STSW", 0xE8, 3 },
	{ "STT", 0x84, 3 },       { "STX", 0x10, 3 },     { "SUB", 0x1C, 3 },
	{ "SUBR", 0x94, 2 },      { "SVC", 0xB0, 2 },     { "TD", 0xE0, 3 },
	{ "TIO", 0xF8, 1 },       { "TIX", 0x2C, 3 },     { "TIXR", 0xB8, 2 },
	{ "WD", 0xDC, 3 },
	//oh, what the hell, let's put in floating point instructions, too
	{ "ADDF", 0x58, 3 },      { "COMPF", 0x88, 3 },   { "DIVF", 0x64, 3 },
	{ "FIX", 0xC4, 1 },       { "FLOAT", 0xC0, 1 },   { "LDF", 0x70, 3 },
	{ "MULF", 0x60, 3 },      { "NORM", 0xC8, 1 },    { "STF", 0x80, 3 },
	{ "SUBF", 0x5C, 3 }
};
#define NUM_OPCODES         (sizeof(SIC_Opcodes) / sizeof(SIC_Opcode))
//-----------------------------------

//initialize register table
struct SIC_Reg{ char name[7]; unsigned char code; };
const struct SIC_Reg SIC_Regs [] = {
	{ "A", 0x0 },
	{ "X", 0x1 },
	{ "L", 0x2 },
	{ "PC", 0x8 },
	{ "SW", 0x9 },
	{ "B", 0x3 },
	{ "S", 0x4 },
	{ "T", 0x5 },
	{ "F", 0x6 }
};
#define NUM_REGS         (sizeof(SIC_Regs) / sizeof(SIC_Reg))
//-----------------------------------

//bit masks for formats 3 and 4

#define BIT_E_3       0x1000
#define BIT_P_3       0x2000
#define BIT_B_3       0x4000
#define BIT_X_3       0x8000
#define BIT_I_3       0x10000
#define BIT_N_3       0x20000

#define BIT_E_4       0x100000
#define BIT_P_4       0x200000
#define BIT_B_4       0x400000
#define BIT_X_4       0x800000
#define BIT_I_4       0x1000000
#define BIT_N_4       0x2000000
//-----------------------------------

using namespace std;    //only necessary for our exceptions

// a few globals (sorry)
int currentLine, currentPass, filelength, progSize;
int progBaseAddr, progStartAddr;
bool progBaseAddrValid;
char *buffer=NULL;
//TList* SYMTAB;          //symbol table
//TList* MODTAB;          //modification records
//TList* LITTAB;          //literal table
//TList* LTORGS;          //table of addresses where LTORGs occur
std::list<SIC_Literal>* LITTAB;
std::list<SIC_Symbol>* SYMTAB;
std::list<void*>* MODTAB;
std::list<void*>* LTORGS;
//---------------------------------------------------------------------------

//prototypes
//----------------------
void Pass1(char* fName);
void Pass2(char* fName);
SIC_Symbol* GetSymbol(char* str);
int GetOpcodeNum(char* str);
int GetRegisterNum(char* str);
int GetInstructionlength(char *instruction, char* operand);
void AddSymbol(char* name, long address, char* instruction, char* operand);
string ProcessInstruction(int LOCCTR, char* instruction, char* operand);
void DumpLiterals(int *LOCCTR, int *LTORG_Num, int *textAddr, string &asmLine, string &listing, string &output);
//---------------------------------------------------


int main(int argc, char* argv[]){
	//identify ourselves
	printf("SIC/XE Assembler Version 1.0\n");
	printf("Copyright Dmitry Brant, Cleveland State University, June 2003\n\n");
	//check command line for input file
	string fileName;
	if(argc > 1) 
		fileName = string(argv[1]);
	if(!fileName.length()){
		printf("Usage: sicasm <inputfile>\n");
		return(0);
	}
	string assemble("Assembling file: ");
	printf((assemble.append(fileName).c_str()));

	try{

		//here we go
		Pass1(fileName.c_str());
		Pass2(fileName.c_str());
		printf("Done.\n");

	}catch(exception &e){
		printf("Error: %s", e.what());
		printf(" (Line %d, Pass %d)\n", currentLine, currentPass);
	}
	if(buffer) delete [] buffer;
	//for(int i=0; i<SYMTAB->size(); i++) delete ((SIC_Symbol*)SYMTAB->Items[i]);
	//for(int i=0; i<LITTAB->size(); i++) delete ((SIC_Literal*)LITTAB->Items[i]);
	delete LTORGS; delete LITTAB;
	delete MODTAB; delete SYMTAB;
	return(0);
}
//---------------------------------------------------------------------------


void Pass1(const char* fName)
{
	currentPass = 1;
	FILE* inputFile = fopen(fName, "r");
	if(!inputFile)
		throw runtime_error("Failed to open input file.");
	fseek(inputFile, 0, SEEK_END);
	filelength = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);

	int bufPtr=0;
	buffer = new char[filelength+1];
	buffer[filelength] = 0;

	if(!fread(buffer, 1, filelength, inputFile))
		throw runtime_error("Failed to read from file.");
	fclose(inputFile);

	//we'll be parsing the buffer line by line,
	//then each line individually
	char thisLine[1024];  // 1 KB per line? More than enough.
	int linePtr;
	currentLine = 0;

	int LOCCTR = 0;
	int spaceReservedAfterLTORG=0;
	int LTORG_Num = 0;

	while(bufPtr < filelength)
	{
		//read in a line from the buffer
		thisLine[0] = ' '; thisLine[1] = 0;
		linePtr = 1;
		char c;
		while(1)
		{
			c = buffer[bufPtr++];
			if((c == 0xA) || (bufPtr > filelength)) 
				break;
			if(c >= ' ') 
				thisLine[linePtr++] = c;
		}
		thisLine[linePtr] = ' ';
		thisLine[linePtr+1] = 0;   //finish line
		currentLine++;

		if(thisLine[1] == 0) continue;
		string strLine(thisLine);

		char* token = strtok(thisLine, " ");
		if(!token) continue;

		//is this the START directive?
		if(StrPos(strLine.c_str(), " START "))
		{
			//extract starting address
			while(strcmp(token, "START")) 
				token = strtok(NULL, " ");
			token = strtok(NULL, " ");
			string foo = ("0x" + string(token));
			sscanf(foo.c_str(), "%x", &progStartAddr);
			LOCCTR = progStartAddr;
			continue;
		}

		//is this the END directive?
		if(StrPos(strLine.c_str(), " END ")){ continue; }

		//is this the BASE directive?
		if(StrPos(strLine.c_str(), " BASE ")){ continue; }

		//is this the NOBASE directive?
		if(StrPos(strLine.c_str(), " NOBASE ")){ continue; }

		//is this a comment line?
		if(token[0] == '.') continue;

		//is this the LTORG directive?
		if(StrPos(strLine.c_str(), " LTORG "))
		{
			LTORGS->Add((void*)LOCCTR);
			LOCCTR += spaceReservedAfterLTORG;
			spaceReservedAfterLTORG = 0;
			LTORG_Num++;
			continue;
		}

		//does this line contain a literal?
		if(StrPos(strLine.c_str(), "=C'") || StrPos(strLine.c_str(), "=X'"))
		{
			//get the token that has it
			while(!StrPos(token, "=")) 
				token = strtok(NULL, " ");
			string litTok = string(token);
			//insert this literal into the table
			//but first, check if it already exists
			bool exists=false;
			for(int i=0; i<LITTAB->size(); i++)
			{
				SIC_Literal* L = (SIC_Literal*)LITTAB->Items[i];
				if((!strcmp(L->name, token)) && (L->LTORG_Num == LTORG_Num))
					exists = true;
			}
			if(!exists)
			{
				SIC_Literal* lit = new SIC_Literal;
				strcpy(lit->name, token);
				lit->offsetFromLTORG = spaceReservedAfterLTORG;
				lit->LTORG_Num = LTORG_Num;
				LITTAB->Add(lit);
				token++;
				//what is the byte length of this literal?
				if(token[0] == 'X')
				{
					token += 2;  //go past the letter and the '
					string val(token);
					val = val.substr(1, val.length()-1);
					if(val.length() > 2) 
						spaceReservedAfterLTORG += 3;
					else spaceReservedAfterLTORG += 1;
				}
				else if(token[0] == 'C')
				{
					token += 2;  //go past the letter and the '
					string val(token);
					val = val.substr(1, val.length()-1);
					if(val.length() > 1) 
						spaceReservedAfterLTORG += 3;
					else 
						spaceReservedAfterLTORG += 3;
				}
			}
			//reset line tonekizer
			strcpy(thisLine, strLine.c_str());
			token = strtok(thisLine, " ");
		}

		char *instruction;
		char *operand;

		//is the first token an opcode?
		if(GetOpcodeNum(token) == -1)
		{
			//it's a symbol
			//but does it already exist?
			if(GetSymbol(token))
				throw runtime_error("Duplicate symbol.");
			//it's a new symbol
			//find out the instruction associated with it
			instruction = strtok(NULL, " ");
			operand = strtok(NULL, " ");
			if(!instruction)
				throw runtime_error("Unexpected end of line.");
			if(instruction[0] == 0)
				throw runtime_error("Unexpected end of line.");
			//insert symbol
			AddSymbol(token, LOCCTR, instruction, operand);
		}
		else
		{
			//it is an opcode!
			instruction = token;
			operand = strtok(NULL, " ");
		}
		LOCCTR += GetInstructionlength(instruction, operand);
	}
	//add an obligatory entry in LTORG table
	LTORGS->Add((void*)LOCCTR);
	//was there more LTORG space reserved for the end?
	LOCCTR += spaceReservedAfterLTORG;
	progSize = LOCCTR - progStartAddr;
}
//---------------------------------------------------------------------------


//My God, man! I tried to save him! I was desperate to save him...
//He was the last, best hope in the universe for peace.
void Pass2(char* fName)
{
	currentPass = 2;
	if(!buffer) 
		return;
	string output;
	string listing;
	int bufPtr=0;

	//we'll be parsing the buffer line by line,
	//then each line individually
	char thisLine[1024];  // 1 KB per line? More than enough.
	string asmLine;
	int linePtr;
	currentLine = 0;
	progBaseAddrValid = false;

	string progName;
	int LOCCTR = 0;
	int textAddr = 0;
	int LTORG_Num = 0;

	while(bufPtr < filelength)
	{
		//read in a line from the buffer
		thisLine[0] = ' '; thisLine[1] = 0;
		linePtr = 1;
		char c;
		while(1)
		{
			c = buffer[bufPtr++];
			if((c == 0xA) || (bufPtr > filelength)) 
				break;
			if(c >= ' ') 
				thisLine[linePtr++] = c;
		}
		thisLine[linePtr] = ' ';
		thisLine[linePtr+1] = 0;   //finish line
		currentLine++;

		if(thisLine[1] == 0) 
			continue;
		string strLine(thisLine);

		char* token = strtok(thisLine, " ");
		if(!token) 
			continue;

		//is this the START directive?
		if(StrPos(strLine.c_str(), " START "))
		{
			//extract starting address
			while(strcmp(token, "START"))
			{
				progName = string(token);
				token = strtok(NULL, " ");
			}
			token = strtok(NULL, " ");
			string foo = ("0x" + string(token));
			sscanf(foo.c_str(), "%x", &LOCCTR);
			progStartAddr = LOCCTR;
			textAddr = progStartAddr;
			//write out the header record
			output += "H";
			if(progName.length() < 6) 
				progName += Ansistring::stringOfChar(' ', 6-progName.length());
			output += progName;
			output += IntToHex((int)LOCCTR, 6);
			output += IntToHex((int)progSize, 6);
			output += "\n"; //header complete
			//we still want it in the listing
			listing += string(currentLine) + Ansistring::stringOfChar(' ', 8 - string(currentLine).length()) + IntToHex((int)LOCCTR, 4) + "    ";
			listing += strLine + "\r\n";
			continue;
		}

		//is this the END directive?
		if(StrPos(strLine.c_str(), " END "))
		{
			//extract first executable instruction
			while(strcmp(token, "END")) token = strtok(NULL, " ");
			token = strtok(NULL, " ");
			int firstExec = atoi(token);
			bool firstExecValid = (token != NULL);
			//we still want it in the listing
			listing += string(currentLine) + Ansistring::stringOfChar(' ', 8 - string(currentLine).length()) + "        ";
			listing += strLine + "\r\n";
			//dump any other literals
			DumpLiterals(&LOCCTR, &LTORG_Num, &textAddr, asmLine, listing, output);
			//is there a remaining text record?
			if(asmLine.length())
			{
				//end this text record
				output += "T";
				output += IntToHex((int)textAddr, 6);
				output += IntToHex((int)asmLine.length()/2, 2);
				output += asmLine + "\n";
			}
			//a good time to dump modification records
			for(int i=0; i<MODTAB->size(); i++)
			{
				unsigned int k = (int)MODTAB->Items[i];
				output += "M";
				output += IntToHex((int)k, 8) + "\n";
			}
			output += "E";
			if(firstExecValid) output += IntToHex((int)firstExec, 6);
			else output += IntToHex((int)progStartAddr, 6);
			//we're bloody done!
			break;
		}

		//is this the BASE directive?
		if(StrPos(strLine.c_str(), " BASE ")){
			while(strcmp(token, "BASE")) token = strtok(NULL, " ");
			token = strtok(NULL, " ");
			SIC_Symbol* sym = GetSymbol(token);
			if(sym)
				progBaseAddr = sym->address;
			else
				progBaseAddr = atoi(token);
			progBaseAddrValid = true;
			//we still want it in the listing
			listing += string(currentLine) + Ansistring::stringOfChar(' ', 8 - string(currentLine).length()) + "        ";
			listing += strLine + "\r\n";
			continue;
		}

		//is this the NOBASE directive?
		if(StrPos(strLine.c_str(), " NOBASE ")){
			//we still want it in the listing
			listing += string(currentLine) + Ansistring::stringOfChar(' ', 8 - string(currentLine).length()) + "        ";
			listing += strLine + "\r\n";
			progBaseAddrValid = false;
			continue;
		}

		//is this a comment line?
		if(token[0] == '.')
		{
			//we still want it in the listing
			listing += string(currentLine) + Ansistring::stringOfChar(' ', 8 - string(currentLine).length()) + "        ";
			listing += strLine + "\r\n";
			continue;
		}

		//sweet mother of god... we're dumping the literals
		if(StrPos(strLine.c_str(), " LTORG "))
		{
			//put it in the listing
			listing += string(currentLine) + Ansistring::stringOfChar(' ', 8 - string(currentLine).length()) + "        ";
			listing += strLine + "\r\n";
			DumpLiterals(&LOCCTR, &LTORG_Num, &textAddr, asmLine, listing, output);
			continue;
		}

		char *instruction;
		char *operand;

		//is the first token an opcode?
		if(GetOpcodeNum(token) == -1)
		{
			//it's a symbol
			//we'll skip past it, but first make sure it exists
			if(!GetSymbol(token))
				throw runtime_error("Unrecognized symbol.");
			instruction = strtok(NULL, " ");
			operand = strtok(NULL, " ");
			if(instruction[0] == 0)
				throw runtime_error("Unexpected end of line.");
		}
		else
		{
			//it is an opcode!
			instruction = token;
			operand = strtok(NULL, " ");
		}

		//do instruction
		string instr = ProcessInstruction(LOCCTR, instruction, operand);

		listing += string(currentLine) + Ansistring::stringOfChar(' ', 8 - string(currentLine).length()) + IntToHex((int)LOCCTR, 4) + "    ";
		listing += strLine + Ansistring::stringOfChar(' ', 32 - strLine.length()) + instr + "\r\n";

		if((!instr.length()) && asmLine.length())
		{   //it was a RESB or RESW
			//end this text record
			output += "T";
			output += IntToHex((int)textAddr, 6);
			output += IntToHex((int)asmLine.length()/2, 2);
			output += asmLine + "\n";
			//start new text record
			asmLine = "";
			textAddr = LOCCTR;
		}
		else
		{
			if((asmLine.length() + instr.length()) > 60)
			{
				//end this text record
				output += "T";
				output += IntToHex((int)textAddr, 6);
				output += IntToHex((int)asmLine.length()/2, 2);
				output += asmLine + "\n";
				//start new text record
				asmLine = instr;
				textAddr = LOCCTR;
			}
			else
			{
				asmLine += instr;
			}
		}

		LOCCTR += GetInstructionlength(instruction, operand);

	} //while

	//dump symbols into listing
	listing += "\r\n\r\nSymbol          Address:\r\n-----------------------\r\n";
	for(int i=0; i<SYMTAB->size(); i++)
	{
		listing += string(((SIC_Symbol*)SYMTAB->Items[i])->name);
		listing += Ansistring::stringOfChar(' ', 16 - strlen(((SIC_Symbol*)SYMTAB->Items[i])->name));
		listing += IntToHex((int)((SIC_Symbol*)SYMTAB->Items[i])->address, 4);
		listing += "\r\n";
	}

	//dump literals into listing
	listing += "\r\nLiteral         Address:\r\n-----------------------\r\n";
	for(int i=0; i<LITTAB->size(); i++)
	{
		listing += string(((SIC_Literal*)LITTAB->Items[i])->name);
		listing += Ansistring::stringOfChar(' ', 16 - strlen(((SIC_Literal*)LITTAB->Items[i])->name));
		listing += IntToHex((int)((SIC_Literal*)LITTAB->Items[i])->offsetFromLTORG + (int)LTORGS->Items[(int)((SIC_Literal*)LITTAB->Items[i])->LTORG_Num], 4);
		listing += "\r\n";
	}

	//dump the sucker to a file
	string lstFileName = string(fName);
	lstFileName = ChangeFileExt(lstFileName, ".lst");
	string outFileName = ChangeFileExt(lstFileName, ".obj");

	printf("Generating listing file: %s\n", lstFileName.c_str());
	if(FileExists(lstFileName))
		DeleteFile(lstFileName);
	int file = FileCreate(lstFileName);
	if(file == -1)
		throw runtime_error("Could not open listing file for output.");
	FileWrite(file, listing.c_str(), listing.length());
	FileClose(file);

	printf("Generating object file: %s\n", outFileName.c_str());
	if(FileExists(outFileName))
		DeleteFile(outFileName);
	file = FileCreate(outFileName);
	if(file == -1)
		throw runtime_error("Could not open object file for output.");
	FileWrite(file, output.c_str(), output.length());
	FileClose(file);
}
//---------------------------------------------------------------------------

//this ought to be fun...
void DumpLiterals(int *LOCCTR, int *LTORG_Num, int *textAddr, string &asmLine, string &listing, string &output)
{
	for(int i=0; i<LITTAB->size(); i++)
	{
		SIC_Literal* lit = (SIC_Literal*)LITTAB->Items[i];
		//does this literal belong here?
		if(lit->LTORG_Num != *LTORG_Num) continue;

		currentLine++;
		//make up something for the listing
		listing += string(currentLine) + Ansistring::stringOfChar(' ', 8 - string(currentLine).length()) + IntToHex((int)*LOCCTR, 4) + "     ";
		string strLine = "*      " + string(lit->name);
		listing += strLine;

		string instruction = "BYTE";
		string operand = string((char*)(lit->name+1));

		string instr = ProcessInstruction(*LOCCTR, instruction.c_str(), operand.c_str());
		listing += Ansistring::stringOfChar(' ', 31 - strLine.length()) + instr + "\r\n";

		if((asmLine.length() + instr.length()) > 60)
		{
			//end this text record
			output += "T";
			output += IntToHex((int)*textAddr, 6);
			output += IntToHex((int)asmLine.length()/2, 2);
			output += asmLine + "\n";
			//start new text record
			asmLine = instr;
			*textAddr = *LOCCTR;
		}
		else
		{
			asmLine += instr;
		}

		*LOCCTR = *LOCCTR + GetInstructionlength(instruction.c_str(), operand.c_str());
	}
	(*LTORG_Num)++;
}
//---------------------------------------------------------------------------

// From hell's heart, I stab at thee...
string ProcessInstruction(int LOCCTR, char* instruction, char* operand)
{
	string retStr("");
	int op = GetOpcodeNum(instruction);
	if(op == -1)
	{
		if(!strcmp(instruction, "WORD"))
		{
			//it's a word, so get its value (simple integer)
			if(operand) 
				if(operand[0] == 0)
					throw runtime_error("Missing operand.");
			//wait... is it a symbol?
			SIC_Symbol* sym = GetSymbol(operand);
			int val;
			if(sym)
				val = sym->address;
			else
				val = atoi(operand);
			retStr = IntToHex((int)val, 6);
		}
		else if(!strcmp(instruction, "BYTE"))
		{
			//it's a collection of bytes
			if(operand) 
				if(operand[0] == 0)
					throw runtime_error("Missing operand.");
			if(operand[0] == 'X')
			{
				operand += 2;  //go past the letter and the '
				string val(operand);
				val = val.substr(1, val.length()-1);
				//if(val.length() > 2) val = Ansistring::stringOfChar('0', 6-val.length()) + val;
				retStr = val;
			}
			else if(operand[0] == 'C')
			{
				operand += 2;  //go past the letter and the '
				string val(operand);
				val = val.substr(1, val.length()-1);
				for(int i=0; i<val.length(); i++)
					retStr += IntToHex(int(val.c_str()[i]), 2);
			}
			else
			{
				throw runtime_error("Unrecognized expression.");
			}
		}
		else if(!strcmp(instruction, "RESB")){ }
		else if(!strcmp(instruction, "RESW")){ }
		else
		{
			throw runtime_error("Unrecognized directive.");
		}
	}
	else
	{
		int format = SIC_Opcodes[op].format;
		if(instruction[0] == '+')
		{
			if(format != 3)
				throw runtime_error("Extended format does not apply to this instruction.");
			format = 4;
		}
		if(format == 1)
		{
			retStr = IntToHex((int)SIC_Opcodes[op].code, 2);
		}
		else if(format == 2)
		{
			string oper(operand);
			retStr = IntToHex((int)SIC_Opcodes[op].code, 2);
			if(StrPos(oper.c_str(), ",")){
				//take apart the operand into two strings
				string o2 = string(((char*)(StrPos(oper.c_str(), ","))+1));
				string o1 = oper.substr(1, oper.length() - o2.length());
				int k=0;
				//is this a register?
				if(GetRegisterNum(o1.c_str()) > -1) k |= (SIC_Regs[GetRegisterNum(o1.c_str())].code << 4);
				else k |= (atoi(o1.c_str()) << 4);
				if(GetRegisterNum(o2.c_str()) > -1) k |= SIC_Regs[GetRegisterNum(o2.c_str())].code;
				else k |= atoi(o2.c_str());
				retStr += IntToHex(k, 2);
			}
			else
			{
				//just one operand
				int k=0;
				//is this a register?
				if(GetRegisterNum(operand) > -1) k |= (SIC_Regs[GetRegisterNum(operand)].code << 4);
				else k |= (atoi(operand) << 4);
				retStr += IntToHex(k, 2);
			}
		}
		else if(format == 3)
		{
			unsigned long k=0;
			k |= (SIC_Opcodes[op].code << 16);
			//set bits n and i to 1 by default
			k |= BIT_N_3;
			k |= BIT_I_3;
			if(operand)
			{
				string oper(operand);
				//is there indexing?
				if(StrPos(operand, ",X"))
				{
					oper = oper.substr(1, oper.length()-2);
					k |= BIT_X_3;
				}

				int address;
				bool constant=false;
				//is it a literal?
				if(operand[0] == '=')
				{
					//find it
					SIC_Literal* lit=NULL;
					for(int i=0; i<LITTAB->size(); i++){
						if(!strcmp(operand, ((SIC_Literal*)LITTAB->Items[i])->name)){ lit = (SIC_Literal*)LITTAB->Items[i]; break; }
					}
					if(!lit)
						throw runtime_error("Invalid literal value.");
					int LTORG_Addr = (int)LTORGS->Items[lit->LTORG_Num];
					address = LTORG_Addr + lit->offsetFromLTORG;
				}
				//do we have an immediate value?
				else if(operand[0] == '#')
				{
					oper = oper.substr(2, oper.length()-1);
					//hmm... is it a symbol?
					SIC_Symbol* sym = GetSymbol(oper.c_str());
					if(sym)
						address = sym->address;
					else
					{
						address = atoi(oper.c_str());
						constant = true;
					}
					//unset the n bit
					k &= (~((unsigned long)BIT_N_3));
				}
				//do we have an indirection?
				else if(operand[0] == '@')
				{
					oper = oper.substr(2, oper.length()-1);
					//hmm... is it a symbol?
					SIC_Symbol* sym = GetSymbol(oper.c_str());
					if(sym)
						address = sym->address;
					else
					{
						address = atoi(oper.c_str());
						constant = true;
					}
					//unset the i bit
					k &= (~((unsigned long)BIT_I_3));
				}
				//ok, so it's just the symbol address
				else
				{
					SIC_Symbol* sym = GetSymbol(oper.c_str());
					if(!sym)
						throw runtime_error("Unrecognized symbol.");
					address = sym->address;
				}

				int disp;
				if(constant)
				{
					if(address > 4095)
						throw runtime_error("Constant is too large.");
					//use direct addressing
					disp = address;
					//unset both b and p
					k &= (~((unsigned long)BIT_P_3));
					k &= (~((unsigned long)BIT_B_3));
				}
				else
				{
					while(1)
					{
						//are we good for program-size()er relative addressing?
						disp = address - (LOCCTR+GetInstructionlength(instruction, operand));
						if((disp >= -2048) && (disp <= 2047)){
							//yes!
							k |= BIT_P_3;
							if(disp < 0)
								disp = 4096 + disp;
							break;
						}
						//how about base-relative addressing?
						if(progBaseAddrValid){
							disp = address - progBaseAddr;
							if((disp >= 0) && (disp <= 4095))
							{
								k |= BIT_B_3;
								break;
							}
						}
						//address must be too big
						throw runtime_error("Displacement out of range.");
					}
				}
				k |= disp;
			}
			retStr = IntToHex((int)k, 6);
		}
		else if(format == 4)
		{
			unsigned long k=0;
			k |= (SIC_Opcodes[op].code << 24);
			k |= BIT_E_4;
			//set bits n and i to 1 by default ?
			k |= BIT_N_4;
			k |= BIT_I_4;
			if(operand)
			{
				string oper(operand);
				//is there indexing?
				if(StrPos(operand, ",X")){
					oper = oper.substr(1, oper.length()-2);
					k |= BIT_X_4;
				}

				int address;
				bool requireMod=true;

				//is it a literal?
				if(operand[0] == '=')
				{
					//find it
					SIC_Literal* lit=NULL;
					for(int i=0; i<LITTAB->size(); i++){
						if(!strcmp(operand, ((SIC_Literal*)LITTAB->Items[i])->name)){ lit = (SIC_Literal*)LITTAB->Items[i]; break; }
					}
					if(!lit)
						throw runtime_error("Invalid literal value.");
					int LTORG_Addr = (int)LTORGS->Items[lit->LTORG_Num];
					address = LTORG_Addr + lit->offsetFromLTORG;
				}
				//do we have an immediate value?
				if(operand[0] == '#')
				{
					oper = oper.substr(2, oper.length()-1);
					//hmm... is it a symbol?
					SIC_Symbol* sym = GetSymbol(oper.c_str());
					if(sym){
						address = sym->address;
					}else{
						address = atoi(oper.c_str());
						requireMod = false;
					}
					//unset the n bit
					k &= (~((unsigned long)BIT_N_4));
				}
				//do we have an indirection?
				else if(operand[0] == '@')
				{
					oper = oper.substr(2, oper.length()-1);
					//this better be a symbol
					SIC_Symbol* sym = GetSymbol(oper.c_str());
					if(!sym)
						throw runtime_error("Unrecognized symbol.");
					address = sym->address;
					//unset the i bit
					k &= (~((unsigned long)BIT_I_4));
				}
				//ok, so it's just the symbol address
				else
				{
					SIC_Symbol* sym = GetSymbol(oper.c_str());
					if(!sym)
						throw runtime_error("Unrecognized symbol.");
					address = sym->address;
				}
				k |= address;
				//add this to our modification table
				if(requireMod)
					MODTAB->Add((void*)(((LOCCTR+1 - progStartAddr) << 8) + 5)); //will always be 5 half-bytes
			}
			retStr = IntToHex((int)k, 8);
		}
	}
	return(retStr);
}
//---------------------------------------------------------------------------

//Cry havoc! And let slip the dogs of war!
void AddSymbol(char* name, long address, char* instruction, char* operand)
{
	if(strlen(name) > 6)
		throw runtime_error("Symbol is too long.");
	SIC_Symbol *s = new SIC_Symbol;
	strcpy(s->name, name);
	s->address = address;
	SYMTAB->Add(s);
	//don't care if it's format-4 right now
	if(instruction[0] == '+'){ instruction++; }
	//if the instruction is an opcode, then the symbol automatically becomes a label
	if(GetOpcodeNum(instruction) > -1)
		return;

	if(!strcmp(instruction, "WORD"))
	{
		//it's a word, so get its value (simple integer)
		if(!operand)
			throw runtime_error("Missing operand.");
		return;
	}

	if(!strcmp(instruction, "BYTE"))
	{
		//it's a collection of bytes
		if(!operand)
			throw runtime_error("Missing operand.");
		if(operand[0] == 'X'){ }
		else if(operand[0] == 'C'){ }
		else
		{
			throw runtime_error("Unrecognized expression.");
		}
		return;
	}

	if(!strcmp(instruction, "RESB"))
	{
		if(!operand)
			throw runtime_error("Missing operand.");
		return;
	}

	if(!strcmp(instruction, "RESW"))
	{
		if(!operand)
			throw runtime_error("Missing operand.");
		return;
	}

	throw runtime_error("Unsupported directive.");
}
//---------------------------------------------------------------------------

//retrieves a symbol from the symbol list, null if not found
SIC_Symbol* GetSymbol(char* str)
{
	bool found=false;
	SIC_Symbol* s;
	for(int i=0; i<SYMTAB->size(); i++){
		s = (SIC_Symbol*)SYMTAB->Items[i];
		if(!strcmp(str, s->name)){ found=true; break; }
	}
	if(found) return(s); else return(NULL);
}
//---------------------------------------------------------------------------

//retrieves position in opcode array, -1 if not found
int GetOpcodeNum(char* str)
{
	//scan opcode table
	bool found=false;
	int i;
	//don't care if it's a format-4 opcode
	if(str[0] == '+') str++;
	for(i=0; i<NUM_OPCODES; i++)
		if(!strcmp(str, SIC_Opcodes[i].name))
		{ 
			found = true; 
			break; 
		}
		if(found) 
			return(i);
		else 
			return(-1);
}
//---------------------------------------------------------------------------

//retrieves position in register array, -1 if not found
int GetRegisterNum(char* str)
{
	//scan register table
	bool found=false;
	int i;
	for(i=0; i<NUM_REGS; i++)
		if(!strcmp(str, SIC_Regs[i].name)){ found = true; break; }
		if(found) return(i);
		else return(-1);
}
//---------------------------------------------------------------------------

//calculates byte length of given instruction
int GetInstructionlength(char *instruction, char* operand)
{
	bool formatFour=false;
	int size=0;
	if(instruction[0] == '+')
	{ 
		formatFour = true; 
		instruction++; 
	}
	//ok, is it one of the opcodes?
	if(GetOpcodeNum(instruction) > -1)
	{
		size += SIC_Opcodes[GetOpcodeNum(instruction)].format;
		if(formatFour)
		{
			if(size != 3)
				throw runtime_error("Extended format does not apply to this instruction.");
			size++;
		}
	}
	else if(!strcmp(instruction, "WORD"))
	{
		//check that operand exists
		if(!operand)
			throw runtime_error("Missing operand.");
		size += 3;
	}
	else if(!strcmp(instruction, "BYTE"))
	{
		//check that operand exists
		if(!operand)
			throw runtime_error("Missing operand.");
		if(operand[0] == 'X')
		{
			operand += 2;  //go past the letter and the '
			string s(operand);
			s = s.substr(1, s.length()-1);
			size += (s.length() / 2);
		}
		else if(operand[0] == 'C')
		{
			operand += 2;  //go past the letter and the '
			string s(operand);
			s = s.substr(1, s.length()-1);
			size += s.length();
		}
		else
		{
			throw runtime_error("Unrecognized expression.");
		}
	}
	else if(!strcmp(instruction, "RESB"))
	{
		//check that operand exists
		if(!operand)
			throw runtime_error("Missing operand.");
		size += atoi(operand);
	}
	else if(!strcmp(instruction, "RESW"))
	{
		//check that operand exists
		if(!operand)
			throw runtime_error("Missing operand.");
		size += (3 * atoi(operand));
	}
	return(size);
}
//---------------------------------------------------------------------------

