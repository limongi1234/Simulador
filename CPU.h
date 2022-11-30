// CPU.h: interface for the CPU class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPU_H__CDF280FE_4A38_460E_B0B1_0DE01F12ECD8__INCLUDED_)
#define AFX_CPU_H__CDF280FE_4A38_460E_B0B1_0DE01F12ECD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <time.h>
#include "Globais.h"
#include <iostream>
using namespace std;

#define EXEC 1
#define STEP 2
#define SZ_MEM 1000

//opcodes de cada instrução


//Mnemonicos
enum Mnemonico
{
	MOV,

	AND,
	OR,
	XOR,
	NOT,
	CMP,

	ADD,
	MUL,
	DIV,
	SUB,

	JMP,
	JC,
	JNC,
	JE,
	JNE,
	JL,
	JLE,
	JG,
	JGE,

	STC,
	CLC,
	HLT,
	NOP,
	INVALIDO
};


enum RegOp{
	rAX = 0,
	rAL = 1,
	rAH = 2,
	rBX = 3,
	rBL = 4,
	rBH = 5
};


typedef union
{	
	unsigned short int word;	
	struct
	{
		unsigned char LowByte;
		unsigned char HighByte;
	};
	
}Registrador;

typedef struct
{
	unsigned C : 1;
	unsigned O : 1;
	unsigned Z : 1;
	unsigned S : 1;
}Flags;


typedef union
{
	unsigned char byte;
	struct
	{
		unsigned P0 : 1;
		unsigned P1 : 1;
		unsigned P2 : 1;
		unsigned P3 : 1;
		unsigned P4 : 1;
		unsigned P5 : 1;
		unsigned P6 : 1;
		unsigned P7 : 1;
		unsigned P8 : 1;
	};
}Porto;

//funcao auxiliar 
void sleep(unsigned int mseconds);

class CPU  
{
    private:
	Mnemonico opMnemonicoTab[255];
	Registrador AX;
	Registrador BX;
	Flags flags;
	unsigned char IR;
	unsigned short int IP;
	unsigned char Memoria[SZ_MEM];
	modo_ender GetModo(int opcode);
	bool is16BitReg(unsigned char r);
	Mnemonico GetMnemonico(unsigned char opcode);
	unsigned char LeByte(unsigned short int indice);
	unsigned short int LeWord(unsigned short int indice);
	char * MnemToStr(unsigned char m, char * s);
	char * RegToStr(unsigned char r, char *s);
	int modoExec;
	FILE* log;
	
    public:
	void executa();
	void Reset();
	void Imprime();
	void Interface();
	CPU();
	CPU(char * Arquivo);
	virtual ~CPU();
	int delay;
	void help();
	void menu();
};

#endif // !defined(AFX_CPU_H__CDF280FE_4A38_460E_B0B1_0DE01F12ECD8__INCLUDED_)
