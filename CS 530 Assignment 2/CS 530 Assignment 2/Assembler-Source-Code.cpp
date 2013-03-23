/*
 *  SIC/XE Assembler Project, Released Dec,23 2012.
 *
 *  Copyright (C) 2011 Ahmad Mousa
 *  Copyright (C) 2011 Laith Zuhair
 *  Copyright (C) 2011 Sari Sultan
 *
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *
 *
 *  In addition, as a special exception, the copyright holders give
 *  permission to link the code of portions of this program with the
 *  Students projects under the conditions of non-commercial projects
 *
 *  You must obey the GNU General Public License in all respects
 *  for all of the code used other than Students needs. *  If you modify
 *  file(s) with this exception, you may extend this exception to your
 *  version of the file(s), but you are not obligated to do so. *  If you
 *  do not wish to do so, delete this exception statement from your
 *  version. *  If you delete this exception statement from all source
 *  files in the program, then also delete it here.
 */
// in order for full functionallity we recommend using Visual C++ 2010
// sorry for not using gcc compiler, but the course terms refused using gcc
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <math.h>
using namespace std;

#ifndef NODE_CPP
#define NODE_CPP
class Node			// linked list class to deal with expressions 
{
public:
	Node()
	{
	next= NULL;
	prev =NULL;
	}
	string data;
	Node *next;
	Node *prev;
protected:
	Node *head,*tail;
	int Counter;
};
#endif
#ifndef STACK_H
#define STACK_H

class Stack :public Node		//stack class to deal with expressions by using the infix and postfix concept in data structure
{
public:	
Stack::Stack()
{
	head = NULL;
	Counter = 0;
}
void Stack::push( string value )
{
	Node *nn = new Node;
	nn ->data = value;

	nn->next= head;
	head = nn;

	Counter++;

}
string  Stack::pop()
{
	string val;
	if( isEmpty() )
	{
		cout<<"\n\tTHE STACK IS EMPTY\n";
		return val;
	}

	else
	{
		Node *del = head;

		head= head->next;
		val = del->data;
		delete del;

		Counter--;
		return val;
	}
}
string Stack::topEl()
{
	if( isEmpty() )
	{
		return "";
		
	}

	else
	{

		return (head->data);
	}
}
bool Stack::isEmpty()
{
	return head == NULL;
}
};

#endif
string CodeArray[1000];		// which readed from input file and saved into this array [assemply code holding]
string PcArray[1000];		// which we save the pc in it, for every assembly instruction.
string OPCODE[60];			//mapping for every mnemonic[lda,sta,..etc]+ (opcode <==> Format) 
int FORMAT[60];				//format for ==== == = = = = = = = == = = = = = = =
string Mnemonic[60];		//saves all assembly mnemonic
string MachCode[1000];		// saves the machine code for every instruction 
int MachCounter=0;			// the index of machine code array
string AssemCode[1000];		// save the assembly instruction with the literals 
int AssemCounter=0;			//the index of the previous array
string ModRec[50];			//it saves all the modification records 
int ModCounter =0;			//the index of the previous array
string Labels[100];			//this holds the labels from the input file
string PcLabels [100];		//location counter for the labels
int LabMode [100];			//to differentiate between normal labels and label with EQU directive
int LabelCounter=0;			//index of the labels array
string LitName[100];		//this holds the literals name from the input file [all in pass1]
string LitAdd[100];			//this holds the literals location counter  [pass1]
string LitVal[100];			//this holds the literal value  [pass1]
int LitLen[100];			//this hold the literal lenght in byets [pass1]
int LitCounter=0;			//location counter for literals
int LitCount=0;				//to point to the first literal which doesn't have location counter
string ObProg[100];
int ObProgCounter=0;
string Base;				// holds the base value of the code [pass 2 when base directive occur
ofstream IntermediateFile;  //the Intermediate file 
ofstream ObjectPorogram;    //The Oject File Program 
ifstream InputFile ;		//object ifstream read files
string s;					// line reading from file
string PC;					// variable to store location counter
int PcIndex;				// to store index of location counter array after cheecking it
int Counter=0;				// number of byte for the whole machine
int PcCounter=0;			
//=================================================== Flags
bool Format4Flag =0;		//to indicate format 4 instructions
//================================================== SupportiveFunctions
void ShowAll();				// to print the values on output file
void OPCODETABLE();			//m3ba jahez
void StrFilter(string );	//filtering files 
string Fill( string );		// to fill the specific value to be 3 bytes (it filled with 0's)
string Fill_F( string );	// to fill the specific value to be 3 byter ( it filled with F's)
string SignEX( string );	//	to extend the value by the most sigificant bit replication
string IntToStr (int);		//	int to string converter 
int HexToInt (string);		//	hex to integer converter 
string IntToHex (int);		//integer to hex converter 
int StrToInt(string ) ;		//sting to int converter
void DelSpace (string &);	//delete for space from the input file TABS
void Del_(string &);		//del _ 
string HexToBin( string ) ;	//hex to binary converter
string BinToHex( string ) ;	//binary to hex converter
string ToUpper( string );	//lower case to upper case
void TwoComp(string &);		//conver to two's complement 
//==================================================== Assembler Functions
void Pass1(string);					// to assign every assembly instruction with its appropriate address
void Pass2(string,string,int);		// to give the machine code of the instructions 
int AddLables(string,string,int);	// to add labels in the label array
int AddLit(string,string);			// to literal in the literal array
string Expression(string );			// to deal with expresions
string RegNum(string);				//to get the register number for every register 
void Object_program();				// to get the object program of the code
//=================================================== MAIN function
void main ()
{
	int PcTrack=1;				//keep tracking of the next location counter
	string P_C="";				//save the location counter of the next instruction
	OPCODETABLE();				//hold the opcode of the instructions
	InputFile.open("input.txt");	//open the assembly code file [input file]
	if( InputFile.is_open())	//check if the file is opened without any difficulties 
	{
		StrFilter(s);			//to filter the machine code and out it in codearray
	}
	else 
	{
		cout<<" Unable to open file "; //execption handling if the file couldn't be opened 
		goto L1;
	}
	PC ="000000";				// initialize the location counter with 0 value because it relocatable program

	for( int i=0 ; i<Counter;i++)//this is for pass1 [giving every instruction its own address]
	{
		Pass1(CodeArray[i]);	//call the pass1 for the specific instruction
	}
	PcArray[PcCounter++]=PC;	
	for( int i=0 ; i<AssemCounter;i++) //for giving the machine code for every instruction
	{
		    PcTrack=i+1;				
			P_C=PcArray[PcTrack++];
			while(P_C == "")
			{
				P_C=PcArray[PcTrack++];
			}
			Pass2(AssemCode[i],P_C,i);
	}
	cout<< "\nThe Array Instruction is :-\n";
	        cout<<"\n\n |=============|========================================|================|\n"
				    <<" |       PC    |              Instruction               |  Macheine Code |\n"
				    <<" |=============|========================================|================|\n";
	for( int i=0 ; i<AssemCounter;i++)
	{	
		string out= AssemCode[i];
		for ( int j=0 ; j<out.length();j++)
				{
					
						if (out.find("	")!= string::npos)
					{
						int pos= out.find("	");
						out=out.replace(pos,1," ");
					}
					
		}
				cout<<" |   "<<setiosflags(ios::left)<<setw(9)<<PcArray[i]<<" |  "<<setw(32)<<out<<"\t|  "<<setw(13)<<MachCode[i]<<" |\n"
				    <<" |=============|========================================|================|\n";
	}	
	cout<< "\n\nThe Labels are :-\n";
	for( int i=0 ; i<LabelCounter;i++)
		{
		cout<<PcLabels[i]<<'\t'<<Labels[i]<<endl;
		}

	Object_program();		//to print the object program
   
	IntermediateFile.open("Intermediate File.txt");
	ObjectPorogram.open("Object Program File.txt");
	
	
	if( IntermediateFile.is_open() && ObjectPorogram.is_open() )
	{
		ShowAll();
	}

	else 
	{
		cout<<" Unable to open One of the output files ";
		goto L1;
	}
	
	L1:cout<<"Press any key to continue... ";
    cin.get();
}
//================================================== Function implementations
void ShowAll()
{
	IntermediateFile<<"\n\n |=============|========================================|================|\n"
				        <<" |      PC     |              Instruction               |  Obeject Code  |\n"
				        <<" |=============|========================================|================|\n";
	for( int i=0 ; i<AssemCounter;i++)
	{	
		string out= AssemCode[i];
		for ( int j=0 ; j<out.length();j++)
				{
					
						if (out.find("	")!= string::npos)
					{
						int pos= out.find("	");
						out=out.replace(pos,1," ");
					}
					
		}
				IntermediateFile<<" |   "<<setiosflags(ios::left)<<setw(9)<<PcArray[i]<<" |  "<<setw(32)<<out<<"\t|  "<<setw(13)<<MachCode[i]<<" |\n"
				    <<" |=============|========================================|================|\n";
	}	


	IntermediateFile.close();

	for( int i=0 ; i<ObProgCounter;i++)
	{
		ObjectPorogram << ObProg[i]<<endl;
	}

	ObjectPorogram.close();

}
void Pass1(string instruction)
{
	int pos,pos1=10000000,pos2=10000000,offset=1;
	int LitFlag=0;
	string Parts[10];bool FParts=0;
	int Part_count=0;
	
	if(instruction[0]==' '|| instruction[0]=='\t')
		FParts=1;
	else 
		FParts=0;

	string part = instruction;
	string inst= instruction;

	while (inst!="")
	{
		pos1=pos2=10000000;
		if (inst.find(" ")!= string::npos)
			pos1= inst.find(" ");
		if(inst.find("\t")!= string::npos)
		    pos2=inst.find("\t");

		if (pos1<pos2)
			pos=pos1;
		else if(pos1>pos2)
			pos=pos2;
		else 
		{
			Parts[Part_count++]=inst;
			break;
		}

		part=inst.substr(0,pos);
		inst= inst.substr(pos+1);
		DelSpace(part);DelSpace(part);
		if ( part !="")
		{
			Parts[Part_count++]=part;
			continue;
		}
	}

	
	for (int i=0 ; i<Part_count;i++)
	{
		int format;
		bool Fmnemonic=0;
		part=Parts[i];

		Format4Flag= 0;
		if ( part[0] == '+')
		{
			Format4Flag= true;
			part = part.substr(1);
		}

		
		for(int i=0;i<59;i++)
		{
			if (Mnemonic[i]== part)
			{
				PC=Fill(PC);
				PcArray[PcCounter++]=PC;
				format=FORMAT[i];
				if (Format4Flag)
					PC= IntToHex(HexToInt(PC)+4);
				else
					PC= IntToHex(HexToInt(PC)+format);

				Fmnemonic=1;

				break;
			}
		}

		if (Fmnemonic)
		{
			if ( instruction.find("=")!=string::npos)
			{
				pos=instruction.find("=");
				inst= instruction.substr(pos);
				DelSpace(inst);
				if (instruction.find("X'")!= string::npos)
			{
				pos1 = instruction.find_first_of("=");
				pos2 = instruction.find_last_of("'");
				inst= instruction.substr(pos1,pos2-pos1+1);

				int check = AddLit(instruction,inst);

				if ( check == -1 )
				{
					pos1 = instruction.find_first_of("'");
			        pos2 = instruction.find_last_of("'");
			        inst= instruction.substr(pos1+1,pos2-pos1-1);
					if (part=="LDCH"||part=="STCH"||part=="WD"||part=="RD"||part=="TD")
					{
						LitVal[LitCounter] =inst;
						  int len = ceil(inst.length()/2.0);
						  LitLen[LitCounter++] =len;
					}
					else
					{
						if (inst.length()==1)
							inst="0"+inst;
						string Val =inst+"000000";
						Val= Val.substr(0,6);
						LitVal[LitCounter] =Val;
						  int len = ceil(Val.length()/2.0);
						  LitLen[LitCounter++] =len;
					}

				}

			}
		    else if(instruction.find("C'")!= string::npos)
			{
				pos1 = instruction.find_first_of("=");
				pos2 = instruction.find_last_of("'");
				inst= instruction.substr(pos1,pos2-pos1+1);

				int check = AddLit(instruction,inst);

				if ( check == -1 )
				{
					pos1 = instruction.find_first_of("'");
			        pos2 = instruction.find_last_of("'");
			        inst= instruction.substr(pos1+1,pos2-pos1-1);

			       if (part=="LDCH"||part=="STCH"||part=="WD"||part=="RD"||part=="TD")
					{
						string Value,Val;
						int hex;char c;
						for(int i=0;i<inst.length();i++)
						{
							Val= inst.substr(i,1);
							hex=Val[0];
							Val =IntToHex(hex);
							if(Val.length()==1)
								Val="0"+Val;
							Value=Value+Val;

				
						}
						LitVal[LitCounter] =Value;
						  int len = ceil(Value.length()/2.0);
						  LitLen[LitCounter++] =len;
					}
					else
					{
						string Value,Val;
						int hex;
						for(int i=0;i<inst.length();i++)
						{
							Val= inst.substr(i,1);
							hex=Val[0];
							Val =IntToHex(hex);
							if(Val.length()==1)
								Val="0"+Val;
							Value=Value+Val;

				
						}
						Value=Value+"000000";
						Value= Value.substr(0,6);
						LitVal[LitCounter] =Value;
						  int len = ceil(Value.length()/2.0);
						  LitLen[LitCounter++] =len;
					}


				}
			}
				else if(inst == "=*")
			{

				int check = AddLit(instruction,inst);

				if ( check == -1 )
				{
					
			        inst= instruction.substr(1);

			       if (part=="LDCH"||part=="STCH"||part=="WD"||part=="RD"||part=="TD")
					{
						string Value,Val;
						if(Format4Flag==1)
						Val=Fill(IntToHex(HexToInt(PC)-4));
						else
							Val=Fill(IntToHex(HexToInt(PC)-3));
						Value=Val.substr(Val.length()-2,2);
						LitVal[LitCounter] =Value;
						  int len = ceil(Val.length()/2.0);
						  LitLen[LitCounter++] =len;
					}
					else
					{
						string Value,Val;
						if(Format4Flag==1)
						Val=Fill(IntToHex(HexToInt(PC)-4));
						else
							Val=Fill(IntToHex(HexToInt(PC)-3));

						Value= Val.substr(Val.length()-6,6);
						Value= Value.substr(4,2)+Value.substr(2,2)+Value.substr(0,2);
						LitVal[LitCounter] =Value;
						  int len = ceil(Value.length()/2.0);
						  LitLen[LitCounter++] =len;
					}
				}
			
			}
			else
			{
				int check = AddLit(instruction,inst);

				if ( check == -1 )
				{

			       if (part=="LDCH"||part=="STCH"||part=="WD"||part=="RD"||part=="TD")
					{
						string Value,Val;
						Val=IntToHex(StrToInt(Expression(inst)));
						Val=Fill(Val);
						Value=Val.substr(Val.length()-2,2);
						LitVal[LitCounter] =Value;
						  int len = ceil(Val.length()/2.0);
						  LitLen[LitCounter++] =len;
					}
					else
					{
						string Value,Val;
						Val=IntToHex(StrToInt(Expression(inst)));
						Val=Fill(Val);
						Value=Val.substr(Val.length()-6,6);
						LitVal[LitCounter] =Value;
						  int len = ceil(Val.length()/2.0);
						  LitLen[LitCounter++] =len;
					}


				}
			
			}
				
			}
			break;
		}

		if(Fmnemonic != 1)
		{
			if(part=="START" || part=="BASE" )
		{
			PcArray[PcCounter++]="";
			break;
		}
		else
			if (part=="END" || part=="LTORG")
			{
					PcArray[PcCounter++]= "";

					if (LitCount<LitCounter)
						AssemCode[AssemCounter++]=instruction;

				for (;LitCount<LitCounter;LitCount++)
				{
					PC=Fill(PC);
					LitAdd[LitCount]= PC;

					if(LitAdd[LitCount]=="=*")
						ModRec[ModCounter++]="M^"+Fill(PC)+"^06";

					PcArray[PcCounter++]= PC;
					AssemCode[AssemCounter++]="* "+ LitName[LitCount];

					PC= IntToHex(HexToInt(PC)+LitLen[LitCount]);
					LitFlag=1;
					
				}

				break;
				

			}
		else
			if (part=="ORG")
			{
			    pos=instruction.find("ORG");
				pos=pos+3;
				inst= instruction.substr(pos);
				DelSpace(inst);DelSpace(inst);
				PC=IntToHex(StrToInt(Expression(inst)));
			}
		else if(part=="BYTE")
		{
			if (instruction.find("X'")!= string::npos)
			{
				pos1 = instruction.find_first_of("'");
			    pos2 = instruction.find_last_of("'");
			    part= instruction.substr(pos1+1,pos2-pos1-1);

				int length = ceil(part.length()/2.0);

				PC=Fill(PC);
				PcArray[PcCounter++]=PC;
				PC= IntToHex(HexToInt(PC) + 1);
			}
		    else if(instruction.find("C'")!= string::npos)
			{
				pos1 = instruction.find_first_of("'");
				pos2 = instruction.find_last_of("'");
				 part= instruction.substr(pos1+1,pos2-pos1-1);

				int length = part.length();

				PC=Fill(PC);
				PcArray[PcCounter++]=PC;
				PC= IntToHex(HexToInt(PC) + length);
			}
			else
			{
				PcArray[PcCounter++]=PC;
				PC= IntToHex(HexToInt(PC) + 1);
			}

			break;


		}
		else if(part=="WORD")
		{
			if (instruction.find("X'")!= string::npos)
			{
				pos1 = instruction.find_first_of("'");
				pos2 = instruction.find_last_of("'");
				 part= instruction.substr(pos1+1,pos2-pos1-1);

				int length = ceil(part.length()/2.0);
				if (length<3)
					length=3;

				PC=Fill(PC);
				PcArray[PcCounter++]=PC;
				PC= IntToHex(HexToInt(PC) + 3);
			}
		    else if(instruction.find("C'")!= string::npos)
			{
				pos1 = instruction.find_first_of("'");
				pos2 = instruction.find_last_of("'");
				part= instruction.substr(pos1+1,pos2-pos1-1);

				int length =  part.length();

				if (length<3)
					length=3;

				PC=Fill(PC);
				PcArray[PcCounter++]=PC;
				PC= IntToHex(HexToInt(PC) + length);
			}
			else
			{
				PC=Fill(PC);
				PcArray[PcCounter++]=PC;
				PC= IntToHex( HexToInt(PC) + 3);
			}

			break;

		}

		else if(part=="RESB")
		{

				pos = instruction.find("RESB");
				pos=pos+4;
				part= instruction.substr(pos);

				DelSpace(part);   DelSpace(part);

				int val = StrToInt(part);
	
				PC=Fill(PC);
				PcArray[PcCounter++]=PC;
				PC= IntToHex( HexToInt(PC) + val);

				break;

		}
		else if(part=="RESW")
		{

				pos = instruction.find("RESW");
				pos=pos+4;
				part= instruction.substr(pos);

				DelSpace(part);   DelSpace(part);

				int val = StrToInt(part)*3;
	
				PC=Fill(PC);
				PcArray[PcCounter++]=PC;
				PC= IntToHex( HexToInt(PC) + val);
				break;

		}
		else
		{
			if (i == 0)
			{
				
				if (instruction.find("EQU")!= string::npos)
				{
					
					PcArray[PcCounter++]="";
					string pc="";
					pos = instruction.find("EQU");
					pos=pos+3;
					inst =instruction.substr(pos);

					DelSpace(inst);DelSpace(inst);
					if (inst.find("X'")!= string::npos)
					{
						pos1 = inst.find_first_of("'");
						pos2 = inst.find_last_of("'");
						pc= inst.substr(pos1+1,pos2-pos1-1);
						pc = Fill(pc);
						AddLables(part,pc,0);
					}
					else if(inst.find("C'")!= string::npos)
					{
						pos1 = instruction.find_first_of("'");
						pos2 = instruction.find_last_of("'");
						pc= instruction.substr(pos1+1,pos2-pos1-1);
						pc = Fill(pc);
						AddLables(part,pc,0);
					}
					else if(inst=="*")
					{
						AddLables(part,PC,0);
					}
					else
					{
						pc = IntToHex(StrToInt(Expression(inst)));
						AddLables(part,pc,0);
					}

					break;
				}
				else
				{
					AddLables(part,PC,1);
				}
			}
		}
}

}
if (LitFlag==0)
	AssemCode[AssemCounter++]=instruction;
}
void Pass2(string instruction,string Pc,int index)
{
	bool ImmAdd=0;
	bool IndAdd=0;
	bool PcRelative=0;
	bool BaseRelative=0;
	bool IndexAdd=0;

	string MCode;
	string Opcode,nixbpe,Add,Disp,R1,R2;
	int pos,pos1=10000000,pos2=10000000,offset=1;

	string Parts[10];bool FParts=0;
	int Part_count=0;
	
	if(instruction[0]==' '|| instruction[0]=='\t')
		FParts=1;
	else 
		FParts=0;

	string part = instruction;
	string inst= instruction;

	while (inst!="")
	{
		pos1=pos2=10000000;
		if (inst.find(" ")!= string::npos)
			pos1= inst.find(" ");
		if(inst.find("\t")!= string::npos)
		    pos2=inst.find("\t");

		if (pos1<pos2)
			pos=pos1;
		else if(pos1>pos2)
			pos=pos2;
		else 
		{
			Parts[Part_count++]=inst;
			break;
		}

		part=inst.substr(0,pos);
		inst= inst.substr(pos+1);
		DelSpace(part);DelSpace(part);
		if ( part !="")
		{
			Parts[Part_count++]=part;
			continue;
		}
	}

	for (int i=0 ; i<Part_count;i++)
	{
		int format;
		bool Fmnemonic=0;
		part=Parts[i];

		Format4Flag= 0;
		if ( part[0] == '+')
		{
			Format4Flag= true;
			part = part.substr(1);
			pos=instruction.find("+");
			pos= pos+part.length()+1;
			inst =instruction.substr(pos);
			if((inst.find("#")!=string::npos))
			{
				int Index=-1;
				pos=inst.find("#");
				inst=inst.substr(pos+1);
				DelSpace(inst);
				for (int i=0;i<LabelCounter;i++)
				{
					if (Labels[i] == inst)
					{
						Index=i;
						break;
					}
				}
				if(Index!=-1 && LabMode[Index]==1)
				{
					string Loc = PcArray[index];
					ModRec[ModCounter++]="M^"+Fill(IntToHex(HexToInt(Loc)+1))+"^05";
				}
			}
			else
				{
					string Loc = PcArray[index];
					ModRec[ModCounter++]="M^"+Fill(IntToHex(HexToInt(Loc)+1))+"^05";
				}
				
		}

		for(int i=0;i<59;i++)
		{
			if (Mnemonic[i]== part)
			{
				format=FORMAT[i];
				Opcode=OPCODE[i];
				Fmnemonic=1;

				break;
			}
		}

		if (Fmnemonic)
		{

			if(format==1)
			{
				MachCode[MachCounter++]=Opcode;
			}
			else if(format==2)
			{
				int offset=1;
				if (instruction.find(",")!=string::npos)
				{
					pos = instruction.find(",");
					pos1=instruction.find(part);
					pos1=pos1+part.length();
					R1=instruction.substr(pos1,pos-pos1);
					DelSpace(R1);
					R2=instruction.substr(pos+1);
					DelSpace(R2);

					R1=RegNum(R1);
					R2=RegNum(R2);
				}
				else
				{
					R1=Parts[i+1];
					DelSpace(R1);
					R1=RegNum(R1);
					R2="0";
				}

				MachCode[MachCounter++]=Opcode+R1+R2;

			}
			else if(format==3)
			{
				if (part == "RSUB")
				{
					MachCode[MachCounter++]="4F0000";
					break;
				}
				if ( instruction.find("=")!=string::npos)
			{
				pos1 = instruction.find_first_of("=");
				pos2 = instruction.find_last_of("'");
				part= instruction.substr(pos1,pos2-pos1+1);

			   int Index ;

				for(int i=0;i<LitCounter;i++)
				{
					if (LitName[i]== part)
					{
						Index=i;
						break;
					}
				}

				if(Format4Flag)
				{
					nixbpe="110001";
					Add = LitAdd[Index];
					Add=SignEX(Add);
					Add=Add.substr(Add.length()-5);

					Opcode=HexToBin(Opcode);
					Opcode=Opcode.substr(0,6);
					string Op_nixbpe=BinToHex(Opcode+nixbpe);
					MachCode[MachCounter++]=Op_nixbpe+Add;
				}
				else
				{
					int CheckDisp= HexToInt(LitAdd[Index])-HexToInt(Pc);

				  if ( (CheckDisp <2047)&&(CheckDisp >-2048))
					{
						nixbpe="110010";
						Opcode=HexToBin(Opcode);
						Opcode=Opcode.substr(0,6);
						string Op_nixbpe=BinToHex(Opcode+nixbpe);

						Disp= IntToHex(CheckDisp);
						Disp=Fill(Disp);
						Disp=Disp.substr(Disp.length()-3);
						MachCode[MachCounter++]=Op_nixbpe+Disp;
					}
					else
					{
						nixbpe=nixbpe+"110100";
						Opcode=HexToBin(Opcode);
						Opcode=Opcode.substr(0,6);
						string Op_nixbpe=BinToHex(Opcode+nixbpe);

						Disp= IntToHex(HexToInt(LitAdd[Index])- HexToInt(Base));
						Disp=Fill(Disp);
						Disp=Disp.substr(Disp.length()-3);
						MachCode[MachCounter++]=Op_nixbpe+Disp;
					}
				}
	
			}
				else
				{
				if ( instruction.find(",")!=string::npos)
				{
					pos=instruction.find(",");
					pos1=instruction.find(Parts[i]);
					pos1=pos1+Parts[i].length();
					IndexAdd=1;
					nixbpe="1";
					inst=instruction.substr(pos1,pos-pos1);
				}
				else
				{
					pos1=instruction.find(Parts[i]);
					pos1=pos1+Parts[i].length();
					nixbpe="0";
					inst=instruction.substr(pos1);
				}

				if (instruction.find("@")!=string::npos)
				{
					pos=instruction.find("@");
					IndAdd=1;
					nixbpe="10"+nixbpe;
					inst=instruction.substr(pos+1);
				}
				else if ( instruction.find("#")!=string::npos)
				{
					pos=instruction.find("#");
					ImmAdd=1;
					nixbpe="01"+nixbpe;
					inst=instruction.substr(pos+1);
				}
				else
				{
					nixbpe="11"+nixbpe;
				}

				DelSpace(inst);


				if(Format4Flag)
				{
					nixbpe=nixbpe+"001";
					Add = IntToHex(StrToInt(Expression(inst)));
					Add=SignEX(Add);
					Add=Add.substr(Add.length()-5);

					Opcode=HexToBin(Opcode);
					Opcode=Opcode.substr(0,6);
					string Op_nixbpe=BinToHex(Opcode+nixbpe);
					MachCode[MachCounter++]=Op_nixbpe+Add;
				}
				else
				{

					int CheckDisp= StrToInt(Expression(inst))-HexToInt(Pc);
		
					int Index=-1 ;
					for (int i=0;i<LabelCounter;i++)
					{
						if (Labels[i] == inst)
						{
							Index=i;
							break;
						}
					}

					

					if ( (CheckDisp <2047)&&(CheckDisp >-2048))
					{
						if(ImmAdd)
						{
						if(Index==-1)
						{
							nixbpe=nixbpe+"000";
							Disp= IntToHex(StrToInt(Expression(inst)));
						}
						else
						{
							if(LabMode[Index]==0)
							{
								nixbpe=nixbpe+"000";
								Disp= IntToHex(StrToInt(Expression(inst)));
							}
							else
							{
								nixbpe=nixbpe+"010";
								Disp= IntToHex(StrToInt(Expression(inst))- HexToInt(Pc));
							}
						}
						goto J1;
						}
						nixbpe=nixbpe+"010";
						Disp= IntToHex(StrToInt(Expression(inst))- HexToInt(Pc));

					J1: Opcode=HexToBin(Opcode);
						Opcode=Opcode.substr(0,6);
						string Op_nixbpe=BinToHex(Opcode+nixbpe);

						Disp=Fill(Disp);
						Disp=Disp.substr(Disp.length()-3);
						MachCode[MachCounter++]=Op_nixbpe+Disp;
					}
					else
					{
						if (ImmAdd)
						{
						if(Index==-1)
						{
							nixbpe=nixbpe+"000";
							Disp= IntToHex(StrToInt(Expression(inst)));
						}
						else
						{
							if(LabMode[Index]==false)
							{
								nixbpe=nixbpe+"000";
								Disp= IntToHex(StrToInt(Expression(inst)));
							}
							else
							{
								nixbpe=nixbpe+"100";
								Disp= IntToHex(StrToInt(Expression(inst))- HexToInt(Base));
							}
						}
						goto J2;
						}
						
						nixbpe=nixbpe+"100";
					J2: Disp= IntToHex(StrToInt(Expression(inst))- HexToInt(Base));
						Opcode=HexToBin(Opcode);
						Opcode=Opcode.substr(0,6);
						string Op_nixbpe=BinToHex(Opcode+nixbpe);

						
						Disp=Fill(Disp);
						Disp=Disp.substr(Disp.length()-3);
						MachCode[MachCounter++]=Op_nixbpe+Disp;
					}

					
				}
				}


			}
			break;
		}
		if (Fmnemonic!=1)
		{
			if(part=="*"&& i==0)
			{
				 int Index ;
				 part=Parts[i+1];

				for(int i=0;i<LitCounter;i++)
				{
					if (LitName[i]== part)
					{
						Index=i;
						break;
					}
				}
				MachCode[MachCounter++]=LitVal[Index];
				break;
			}
			else 
				if (part=="BASE")
			{
			    pos=instruction.find("BASE");
				pos=pos+4;
				inst= instruction.substr(pos);
				DelSpace(inst);DelSpace(inst);
				if(inst=="*")
				{
					Base= Fill(PcArray[index]);
				}
				else
					Base=IntToHex(StrToInt(Expression(inst)));

				MachCode[MachCounter++]="";
				break;
			}
			else 
				if (Parts[i+1]=="BYTE")
			{
				if (instruction.find("X'")!= string::npos)
			{
				pos1 = instruction.find_first_of("'");
			    pos2 = instruction.find_last_of("'");
			    part= instruction.substr(pos1+1,pos2-pos1-1);
				DelSpace(part);
				MachCode[MachCounter++]=part;
				
			}
		    else if(instruction.find("C'")!= string::npos)
			{
				pos1 = instruction.find_first_of("'");
				pos2 = instruction.find_last_of("'");
				inst= instruction.substr(pos1+1,pos2-pos1-1);

				string Value,Val;
						int hex;
						for(int i=0;i<inst.length();i++)
						{
							Val= inst.substr(i,1);
							hex=Val[0];
							Val =IntToHex(hex);
							if(Val.length()==1)
								Val="0"+Val;
							Value=Value+Val;

				
						}
						MachCode[MachCounter++]=Value;
			}
			else
			{
				string Val = Parts[i+2];
				Val= IntToHex(StrToInt(Val));
				
			   if(Val.length()==1)
				  Val="0"+Val;

				MachCode[MachCounter++]=Val;
			}

			break;
			}
				else
						if (Parts[i+1]=="WORD")
			{
				if (instruction.find("X'")!= string::npos)
			{
				pos1 = instruction.find_first_of("'");
			    pos2 = instruction.find_last_of("'");
			    part= instruction.substr(pos1+1,pos2-pos1-1);
				DelSpace(part);
				part=part+"000000";
				part=part.substr(0,6);
				MachCode[MachCounter++]=part;
				
			}
		    else if(instruction.find("C'")!= string::npos)
			{
				pos1 = instruction.find_first_of("'");
				pos2 = instruction.find_last_of("'");
				inst= instruction.substr(pos1+1,pos2-pos1-1);

				string Value,Val;
						int hex;
						for(int i=0;i<inst.length();i++)
						{
							Val= inst.substr(i,1);
							hex=Val[0];
							Val =IntToHex(hex);
							if(Val.length()==1)
								Val="0"+Val;
							Value=Value+Val;

				
						}

						Value=Value+"000000";
						Value=Value.substr(0,6);
						MachCode[MachCounter++]=Value;
			}
			else
			{
				string Val = Parts[i+2];
				Val= IntToHex(StrToInt(Val));
				
			   if(Val.length()==1)
				  Val="0"+Val;

			   Val=Val+"000000";
			   Val=Val.substr(0,6);
				MachCode[MachCounter++]=Val;
			}

			break;
			}
			else
			{
				if(i==(Part_count-1))
				{
				MachCode[MachCounter++]="";
				break;
				}
			}

		}
	}
}
void Object_program()
{
	string Mach;
	string code;
	string Pc;
	string T;
	bool Flag=0,End=1;
	int Len;
	int Len1;
	int LEN;
	string AddTrack="0";
	cout<<"\nThe Opject Program is :\n";
	string StartName= Labels[0]+"      ";
	StartName=StartName.substr(0,6);
	cout<<"H^"<<StartName<<"^"<<"000000^"<<PcArray[MachCounter]<<endl;
	ObProg[ObProgCounter++]= "H^"+StartName+"^"+"000000^"+PcArray[MachCounter];
	for( int PcTrack = 0; (PcTrack<MachCounter)&&End;)
	{
			code="";
			LEN=0;Len=0;Len1=0;
			if(Flag)
			{
				PcTrack=PcTrack-1;
				Flag=0;
			}
			Pc=PcArray[PcTrack];
			Mach=MachCode[PcTrack];
			while(Mach == "")
			{
				Mach=MachCode[PcTrack];
				Pc=PcArray[PcTrack];
				PcTrack=PcTrack+1;
					Flag=1;
			}
			if(Flag)
				{
					PcTrack=PcTrack-1;
					Flag=0;

				}
			T="T^"+Pc;
			while(Len <30)
			{
				Flag = 0;
				Pc=PcArray[PcTrack];
				Mach=MachCode[PcTrack];
				while(Mach == "")
				{
					Mach=MachCode[PcTrack];
					Pc=PcArray[PcTrack];
					PcTrack=PcTrack+1;
					Flag=1;
				}
				
				if(Flag==0)
					PcTrack=PcTrack+1;

				int count = PcTrack;
				string temp1;
				AddTrack=PcArray[PcTrack];
				temp1=MachCode[PcTrack];

				while(temp1 == "")
				{
					count=count+1;
					AddTrack=PcArray[count];
					temp1=MachCode[count];

					if (count>=MachCounter)
					{
						End=0;
					break;
					
				}
					
				}

				Len1=Len1+(HexToInt(AddTrack)- HexToInt(Pc));
				Len=Len+(Mach.length()/2);
					

				if (Len>30)
				{
					Flag=1;
					break;
				}

				code=code+"^"+Mach;
				LEN=LEN+(Mach.length()/2);
				if (Len1>30)
				{
					Flag=0;
					break;
				}

				if (End==0)
					break;
			}
			string Length = IntToHex(LEN);
			if (Length.length()==1)
				Length="0"+Length;
			T=T+"^"+Length+code;
			cout<<T<<endl;
			ObProg[ObProgCounter++]=T;


	}

	for (int count =0;count<ModCounter;count++)
	{
		cout<<ModRec[count]<<endl;
	}
	cout<<"E^000000"<<endl;

	ObProg[ObProgCounter++]= "E^000000";
}
int AddLables(string Label,string pc,int mode)
{
	int IdIndex=-1 ;
	for (int i=0;i<LabelCounter;i++)
	{
		if (Labels[i] == Label)
				{
					IdIndex=i;
					break;
				}
	}

	if(IdIndex==-1)
		{
			Labels[LabelCounter]=Label;
			LabMode[LabelCounter]=mode;
			pc=Fill(pc);
			PcLabels[LabelCounter++]=pc;
		}
	return IdIndex;
}
int AddLit(string inst ,string Lit)
{
	int IdIndex=-1 ;
	for (int i=0;i<LitCounter;i++)
	{
		if (LitName[i] == Lit)
		{
					IdIndex=i;
					break;
		}
	}

	if(IdIndex==-1)
		{
			LitName[LitCounter] =Lit;
		}
	return IdIndex;
}
string Expression(string instruction)
	{

			Stack s; int c=0;
			string temp;
			string str[100];
			string expression;
			string ExpParts[50];
			int ExpParts_count=0;

			for(int i=0; i<instruction.length();i++)
	{
		if(instruction[i]=='*'||instruction[i]=='+'||instruction[i]=='/'||instruction[i]=='-'||instruction[i]=='^'||instruction[i]=='%'||instruction[i]=='('||instruction[i]==')')
		{
			if (i==0)
			{
			ExpParts[ExpParts_count++] =instruction.substr(0,1);
			instruction=instruction.substr(1);
			i=-1;
			}
			else
			{
				int IdIndex=-1 ;
				string inst;
				inst=instruction.substr(0,i);
				for ( int count=0;count<LabelCounter;count++)
				{
					if (Labels[count] == inst)
					{
						IdIndex=count;
						break;
					}
				}

				if (IdIndex==-1)
				{
					ExpParts[ExpParts_count++] =instruction.substr(0,i);
					instruction=instruction.substr(i);
					i=-1;
				}
				else
				{
					ExpParts[ExpParts_count++] = IntToStr(HexToInt(PcLabels[IdIndex]));
					instruction=instruction.substr(i);
					i=-1;
				}
			}
		}
		if((instruction.find("*")== string::npos) && (instruction.find("+")== string::npos)&& (instruction.find("^")== string::npos)&& (instruction.find("/")== string::npos)&& (instruction.find("-")== string::npos)&& (instruction.find("^")== string::npos)&& (instruction.find("(")== string::npos)&& (instruction.find(")")== string::npos))
		{
			if(i!=-1)
			{
				int IdIndex=-1 ;
				for ( int count=0;count<LabelCounter;count++)
				{
					string a =Labels[count];
					if (Labels[count] == instruction)
					{
						IdIndex=count;
						break;
					}
				}

				if (IdIndex==-1)
				{
					ExpParts[ExpParts_count++] =instruction;
				}
				else
				{
					ExpParts[ExpParts_count++] = IntToStr(HexToInt(PcLabels[IdIndex]));
				}
			
				break;
			}
		}
	}


			for (int i=0; i<ExpParts_count ;i++ )
			{
				/*if(ExpParts[i] == ' ')
				continue;*/
				if ( ExpParts[i][0]>=48 && ExpParts[i][0]<=58)
				{
					str[c++]= ExpParts[i];
				}

				else
				{
					if (ExpParts[i]== "(") 
					{
						s.push(ExpParts[i]);

					}

					else if (ExpParts[i]== ")")
					{
						temp = s.pop();
						while (temp != "(" )
						{
							str[c++]= temp;
							temp = s.pop();
						}
					}


					else if (ExpParts[i]== "^")
					{
					
						while ( (s.topEl() == "^") && (s.topEl()!="(") )
						{
							temp = s.pop();
							str[c++]= temp;
						}
							s.push(ExpParts[i]);

					}

					else if ((ExpParts[i]== "*")||(ExpParts[i]== "/")||(ExpParts[i]== "%"))
					{

						while ( (s.topEl() == "^" || s.topEl() == "*" || s.topEl() == "/"|| s.topEl() == "%") && (s.topEl()!="(") )
						{
							temp = s.pop();
							str[c++]= temp;
						}
							s.push(ExpParts[i]);
					}




					else if ((ExpParts[i]== "+")||(ExpParts[i]== "-"))
					{
						while ( (s.topEl() == "^" || s.topEl() == "*" || s.topEl() == "/" || s.topEl() == "%" || s.topEl() == "+" || s.topEl() == "-") && (s.topEl()!="(") )
						{
							temp = s.pop();
							str[c++]= temp;
						}
							s.push(ExpParts[i]);
					}
				}
			}

			while(!(s.isEmpty()))
			{
				temp = s.pop();
				str[c++]= temp; 
			}

		/*	for (int i=0;i<c;i++)
			{
				cout<<str[i];
			}*/
	

     	Stack s1; int count, x1,x2,res;

	        
			for (int i=0; i<c;i++ )
			{
				if(str[i] == " ")
				continue;
				if ( str[i][0]>=48 && str[i][0]<=58)
				{
					s1.push(str[i]);
				}

				else
				{
					if (str[i]=="^")
					{
						res=1;

						x2 = StrToInt(s1.pop());
						x1 = StrToInt(s1.pop());
					
						for ( count =0; count<x2; count++ )
							res*=x1;
	
							s1.push(IntToStr(res));
					}

					else if (str[i]=="%")
					{
						x2 = StrToInt(s1.pop());
						x1 = StrToInt(s1.pop());

						res= x1 % x2;

						s1.push(IntToStr(res));

					}
						
					else if (str[i]=="*")
					{
						x2 = StrToInt(s1.pop());
						x1 = StrToInt(s1.pop());

						res= x1 * x2;

						s1.push(IntToStr(res));

					}


					else if (str[i]=="/")
					{
						x2 = StrToInt(s1.pop());
						x1 = StrToInt(s1.pop());

						res= x1 / x2;

						s1.push(IntToStr(res));

					}


					else if (str[i]=="+")
					{

						x2 = StrToInt(s1.pop());
						x1 = StrToInt(s1.pop());

						res= x1 + x2;

						s1.push(IntToStr(res));

					}

					else if (str[i]=="-")
					{
						x2 = StrToInt(s1.pop());
						x1 = StrToInt(s1.pop());

						res= x1 - x2;

						s1.push(IntToStr(res));

					}

					
				}
			}

			string FinalVal=s1.pop();

			return FinalVal ;

		}
string RegNum(string R)
{
	if(R=="A")
		return "0";
	else if(R=="X")
		return "1";
	else if(R=="L")
		return "2";
	else if(R=="B")
		return "3";
	else if(R=="S")
		return "4";
	else if(R=="T")
		return "5";
	else if(R=="F")
		return "6";
	else if(R=="PC")
		return "8";
	else if(R=="SW")
		return "9";
	else return R;
}
void StrFilter(string s)
{
	
		while ( !InputFile.eof())
		{
			getline(InputFile,s);

			if(  s == "")
			continue;
		if(  s == "	")
			continue;
		if(  s == "\t")
			continue;

		if(  (s.find(" ") != string::npos)|| (s.find("\t") != string::npos)||(s.find("	") != string::npos))
		{
			string Temp;
			Temp=s;
			DelSpace(Temp);
			DelSpace(Temp);
			if ( Temp=="")
			continue;
		}

		if(  s.find("//") != string::npos || s.find(";") != string::npos ||s.find(";;") != string::npos || s.find("!_!_!") != string::npos)
		{
			
				if(s.find("!_!_!") >1)
				{
					int pos = s.find("!_!_!");
					s=s.substr(0,pos);
					string Temp=s;
					DelSpace(Temp);
					DelSpace(Temp);
					if (Temp=="")
						continue;
				}
					else
						continue;

		}

//===========================================================================================
///==========================================================================================
					
							CodeArray[Counter++]=s;
						
						
	
		}

}
void OPCODETABLE()
{
	OPCODE[0]="60"; FORMAT[0]=3; Mnemonic[0]="MULF";  OPCODE[1]="98"; FORMAT[1]=2; Mnemonic[1]="MULR";  OPCODE[2]="C8"; FORMAT[2]=1;Mnemonic[2]="NORM";  OPCODE[3]="44"; FORMAT[3]=3; Mnemonic[3]="OR";
	OPCODE[4]="D8"; FORMAT[4]=3; Mnemonic[4]="RD";  OPCODE[5]="AC"; FORMAT[5]=2;  Mnemonic[5]="RMO"; OPCODE[6]="4C"; FORMAT[6]=3; Mnemonic[6]="RSUB"; OPCODE[7]="A4"; FORMAT[7]=2; Mnemonic[7]="SHIFTL";
	OPCODE[8]="A8"; FORMAT[8]=2;  Mnemonic[8]="SHIFTR"; OPCODE[9]="F0"; FORMAT[9]=1; Mnemonic[9]="SIO";  OPCODE[10]="EC"; FORMAT[10]=3; Mnemonic[10]="SSK";OPCODE[11]="0C"; FORMAT[11]=3;Mnemonic[11]="STA";
	OPCODE[12]="78"; FORMAT[12]=3; Mnemonic[12]="STB"; OPCODE[13]="54"; FORMAT[13]=3;Mnemonic[13]="STCH"; OPCODE[14]="80"; FORMAT[14]=3;Mnemonic[14]="STF"; OPCODE[15]="D4"; FORMAT[15]=3;Mnemonic[15]="STI";
	OPCODE[16]="14"; FORMAT[16]=3; Mnemonic[16]="STL"; OPCODE[17]="7C"; FORMAT[17]=3; Mnemonic[17]="STS";OPCODE[18]="E8"; FORMAT[18]=3;Mnemonic[18]="STSW"; OPCODE[19]="84"; FORMAT[19]=3;Mnemonic[19]="STT";
	OPCODE[20]="10"; FORMAT[20]=3;Mnemonic[20]="STX";  OPCODE[21]="1C"; FORMAT[21]=3;Mnemonic[21]="SUB"; OPCODE[22]="5C"; FORMAT[22]=3;Mnemonic[22]="SUBF"; OPCODE[23]="94"; FORMAT[23]=2;Mnemonic[23]="SUBR";
	OPCODE[24]="B0"; FORMAT[24]=2;Mnemonic[24]="SVC";  OPCODE[25]="E0"; FORMAT[25]=3;Mnemonic[25]="TD"; OPCODE[26]="F8"; FORMAT[26]=1;Mnemonic[26]="TIO"; OPCODE[27]="2C"; FORMAT[27]=3;Mnemonic[27]="TIX";
	OPCODE[28]="B8"; FORMAT[28]=2;Mnemonic[28]="TIXR";  OPCODE[29]="DC"; FORMAT[29]=3;Mnemonic[29]="WD"; OPCODE[30]="18"; FORMAT[30]=3;Mnemonic[30]="ADD"; OPCODE[31]="58"; FORMAT[31]=3;Mnemonic[31]="ADDF";
	OPCODE[32]="90"; FORMAT[32]=2;Mnemonic[32]="ADDR";  OPCODE[33]="40"; FORMAT[33]=3;Mnemonic[33]="AND"; OPCODE[34]="B4"; FORMAT[34]=2;Mnemonic[34]="CLEAR"; OPCODE[35]="28"; FORMAT[35]=3;Mnemonic[35]="COMP";
	OPCODE[36]="88"; FORMAT[36]=3;Mnemonic[36]="COMPF"; OPCODE[37]="A0"; FORMAT[37]=2; Mnemonic[37]="COMPR";OPCODE[38]="24"; FORMAT[38]=3;Mnemonic[38]="DIV"; OPCODE[39]="64"; FORMAT[39]=3;Mnemonic[39]="DIVF";
	OPCODE[40]="9C"; FORMAT[40]=2;Mnemonic[40]="DIVR"; OPCODE[41]="C4"; FORMAT[41]=1;Mnemonic[41]="FIX"; OPCODE[42]="C0"; FORMAT[42]=1;Mnemonic[42]="FLOAT"; OPCODE[43]="F4"; FORMAT[43]=1;Mnemonic[43]="HIO";
	OPCODE[44]="3C"; FORMAT[44]=3;Mnemonic[44]="J"; OPCODE[45]="30"; FORMAT[45]=3;Mnemonic[45]="JEQ"; OPCODE[46]="34"; FORMAT[46]=3;Mnemonic[46]="JGT"; OPCODE[47]="38"; FORMAT[47]=3;Mnemonic[47]="JLT";
	OPCODE[48]="48"; FORMAT[48]=3;Mnemonic[48]="JSUB"; OPCODE[49]="00"; FORMAT[49]=3;Mnemonic[49]="LDA"; OPCODE[50]="68"; FORMAT[50]=3;Mnemonic[50]="LDB"; OPCODE[51]="50"; FORMAT[51]=3;Mnemonic[51]="LDCH";
	OPCODE[52]="70"; FORMAT[52]=3;Mnemonic[52]="LDF"; OPCODE[53]="08"; FORMAT[53]=3;Mnemonic[53]="LDL"; OPCODE[54]="6C"; FORMAT[54]=3;Mnemonic[54]="LDS"; OPCODE[55]="74"; FORMAT[55]=3;Mnemonic[55]="LDT";
	OPCODE[56]="04"; FORMAT[56]=3;Mnemonic[56]="LDX"; OPCODE[57]="D0"; FORMAT[57]=3;Mnemonic[57]="LPS"; OPCODE[58]="20"; FORMAT[58]=3;Mnemonic[58]="MUL";
}
void Del_(string &s)
{
	for ( int i=0 ; i < s.length() ; i++)
				{
					int pos;

					if (s.find("_")!= string::npos)
					{
						pos= s.find("_");
						s=s.erase(pos,1);
					}
					
				}
	
}
void  DelSpace(string &s)
{
	int pos;
	for ( int i=0 ; i<s.length();i++)
				{
					if (s.find(" ")!= string::npos)
					{
						pos= s.find(" ");
						s=s.erase(pos,1);
					}
					
				}
	for ( int i=0 ; i<s.length();i++)
				{
					if (s.find("	")!= string::npos)
					{
						pos= s.find("	");
						s=s.erase(pos,1);
					}
					
				}
	for ( int i=0 ; i<s.length();i++)
				{
					if (s.find("\t")!= string::npos)
					{
						pos= s.find("\t");
						s=s.erase(pos,1);
					}
					
				}
}
string IntToHex( int i )
  {
	 std::ostringstream Hex;
      Hex << std::hex << i;
	  //cout<<hex<<showbase<<10;
     string s = Hex.str();
	 s= ToUpper(s);  
	  return s;
  }
string IntToStr( int n )
  {
  std::ostringstream result;
  result << n;
  return result.str();
}
int HexToInt( string s )
  {
	  int i;
	  stringstream convert ;
	  convert= (stringstream)s;
	  convert>> std::hex >> i;
	  
	  return i;
  }
int StrToInt(string in) 
{    
	int ret_val = 0;    
std::stringstream sstr(in);    
sstr >> ret_val;    
return ret_val;
}
string HexToBin( string hexNumber) 
{ 
   std::string binaryNum; 
    
   for (int i = 0; i <= hexNumber.length() ; i++) 
   { 
      if (hexNumber.substr(i,1) == "0") 
         binaryNum  += "0000"; 
      if (hexNumber.substr(i,1) == "1") 
         binaryNum  += "0001"; 
      if (hexNumber.substr(i,1) == "2") 
         binaryNum  += "0010"; 
      if (hexNumber.substr(i,1) == "3") 
         binaryNum  += "0011"; 
      if (hexNumber.substr(i,1) == "4") 
         binaryNum  += "0100"; 
      if (hexNumber.substr(i,1) == "5") 
         binaryNum  += "0101"; 
      if (hexNumber.substr(i,1) == "6") 
         binaryNum  += "0110"; 
      if (hexNumber.substr(i,1) == "7") 
         binaryNum  += "0111"; 
      if (hexNumber.substr(i,1) == "8") 
         binaryNum  += "1000"; 
      if (hexNumber.substr(i,1) == "9") 
         binaryNum  += "1001"; 
      if (hexNumber.substr(i,1) == "a"||hexNumber.substr(i,1) == "A") 
         binaryNum  += "1010"; 
      if (hexNumber.substr(i,1) == "b"||hexNumber.substr(i,1) == "B") 
         binaryNum  += "1011"; 
      if (hexNumber.substr(i,1) == "c"||hexNumber.substr(i,1) == "C") 
         binaryNum  += "1100"; 
      if (hexNumber.substr(i,1) == "d"||hexNumber.substr(i,1) == "D") 
         binaryNum  += "1101"; 
      if (hexNumber.substr(i,1) == "e"||hexNumber.substr(i,1) == "E") 
         binaryNum  += "1110"; 
      if (hexNumber.substr(i,1) == "f"||hexNumber.substr(i,1) == "F") 
         binaryNum  += "1111"; 
   } 
   return binaryNum; 
} 
string BinToHex( string BinNum) 
{ 
   std::string HexNum; 
    
   for (int i = 0; i <= BinNum.length() ; i=i+4) 
   { 
      if (BinNum.substr(i,4) == "0000") 
         HexNum  += "0"; 
      if (BinNum.substr(i,4) == "0001") 
         HexNum  += "1"; 
      if (BinNum.substr(i,4) == "0010") 
         HexNum  += "2"; 
      if (BinNum.substr(i,4) == "0011") 
         HexNum  += "3"; 
      if (BinNum.substr(i,4) == "0100") 
         HexNum  += "4"; 
      if (BinNum.substr(i,4) == "0101") 
         HexNum  += "5"; 
      if (BinNum.substr(i,4) == "0110") 
         HexNum  += "6"; 
      if (BinNum.substr(i,4) == "0111") 
         HexNum  += "7"; 
      if (BinNum.substr(i,4) == "1000") 
         HexNum  += "8"; 
      if (BinNum.substr(i,4) == "1001") 
         HexNum  += "9"; 
      if (BinNum.substr(i,4) == "1010") 
         HexNum  += "A"; 
      if (BinNum.substr(i,4) == "1011") 
         HexNum  += "B"; 
      if (BinNum.substr(i,4) == "1100") 
         HexNum  += "C"; 
      if (BinNum.substr(i,4) == "1101") 
         HexNum  += "D"; 
      if (BinNum.substr(i,4) == "1110") 
         HexNum  += "E"; 
      if (BinNum.substr(i,4) == "1111") 
         HexNum  += "F"; 
   } 
   return HexNum; 
} 
string ToUpper( string s) 
{ 
	string Uper;
    
   for (int i = 0; i <= s.length() ; i++) 
   {

	   if (s.substr(i,1) == "0") 
         Uper  += "0"; 
	   if (s.substr(i,1) == "1") 
         Uper  += "1"; 
	   if (s.substr(i,1) == "2") 
         Uper  += "2"; 
	   if (s.substr(i,1) == "3") 
         Uper  += "3"; 
	   if (s.substr(i,1) == "4") 
         Uper  += "4"; 
	   if (s.substr(i,1) == "5") 
         Uper  += "5"; 
	   if (s.substr(i,1) == "6") 
         Uper  += "6"; 
	   if (s.substr(i,1) == "7") 
         Uper  += "7"; 
	   if (s.substr(i,1) == "8") 
         Uper  += "8"; 
	   if (s.substr(i,1) == "9") 
         Uper  += "9"; 
	   if (s.substr(i,1) == "a") 
         Uper  += "A"; 
	   if (s.substr(i,1) == "b") 
         Uper  += "B"; 
	   if (s.substr(i,1) == "c") 
         Uper  += "C"; 
	   if (s.substr(i,1) == "d") 
         Uper  += "D"; 
	   if (s.substr(i,1) == "e") 
         Uper  += "E"; 
	   if (s.substr(i,1) == "f") 
         Uper  += "F"; 
	   if (s.substr(i,1) == "A") 
         Uper  += "A";
	   if (s.substr(i,1) == "B") 
         Uper  += "B"; 
	   if (s.substr(i,1) == "C") 
         Uper  += "C"; 
	   if (s.substr(i,1) == "D") 
         Uper  += "D"; 
	   if (s.substr(i,1) == "E") 
         Uper  += "E"; 
	   if (s.substr(i,1) == "F") 
         Uper  += "F"; 
      
   }
   return Uper;
} 
void TwoComp(string &s)
{
	int Comp = HexToInt(s);
	Comp=~Comp+1;

	s= IntToHex(Comp);
	s=s.substr(2,6);
}
string Fill( string s)
{
	if(s.length()==0)
		s="000000";
	else if (s.length()==1)
		s="00000"+s;
	else if (s.length()==2)
		s="0000"+s;
	else if (s.length()==3)
		s="000"+s;
	else if (s.length()==4)
		s="00"+s;
	else if (s.length()==5)
		s="0"+s;
	return s;
}
string Fill_F( string s)
{
	if(s.length()==0)
		s="FFFFFF";
	else if (s.length()==1)
		s="FFFFF"+s;
	else if (s.length()==2)
		s="FFFF"+s;
	else if (s.length()==3)
		s="FFF"+s;
	else if (s.length()==4)
		s="FF"+s;
	else if (s.length()==5)
		s="F"+s;
	return s;
}
string SignEX( string s)
{
	s= HexToBin(s);
	if (s[0]=='0')
	{
	s= BinToHex(s);
	if(s.length()==0)
		s="000000";
	else if (s.length()==1)
		s="00000"+s;
	else if (s.length()==2)
		s="0000"+s;
	else if (s.length()==3)
		s="000"+s;
	else if (s.length()==4)
		s="00"+s;
	else if (s.length()==5)
		s="0"+s;

	}
	else if (s[0]=='1')
	{
		s= BinToHex(s);
	 if (s.length()==1)
		s="FFFFF"+s;
	else if (s.length()==2)
		s="FFFF"+s;
	else if (s.length()==3)
		s="FFF"+s;
	else if (s.length()==4)
		s="FF"+s;
	else if (s.length()==5)
		s="F"+s;
	}

	return s;
}