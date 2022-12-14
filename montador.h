// montador.h: interface for the montador class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONTADOR_H__B980415D_A6C4_4593_9675_3725164F695F__INCLUDED_)
#define AFX_MONTADOR_H__B980415D_A6C4_4593_9675_3725164F695F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <iostream>
using namespace std;



#define numReg 6
#define separadores " :;"

#define SZ_TAB_SIMB 1000

#include "Globais.h"


typedef struct 
{
   char nome[30];
   int valor;
   bool constante;
}Simbolos;



class montador  
{
		
    private:
	char nomeArqSaida[30];
	char nomeArqEntrada[90];
	FILE * arqEntrada;
	FILE * arqSaida;	
	
	int pos; //contador de posi??o
	char tabMnemonicos[numInstr][5];		
	
	//Tabela dos valores dos r?tulos, vari?veis e constantes
	Simbolos tabSimbolos[1000];
	int indexTab;
	

	//Lista de registradores usados
	char reg[numReg][3];

	//preenche tabela de simbolos
	bool passo1();
	
	//gera arquivo montado
	bool passo2();	
	//retorna tamanho da instru??o
	int sizeofInstr(char * instrucao);	

	//retorna indice do simbolo na tabela
	int indexOfSimbolo(const char * s);

	//retorna o modo de endere?amento
	modo_ender modo(char * op1, char * op2);
	
	//identifica se ? instru??o
	int isInstr(const char * s);	
	bool isReg(char * s);
	bool isNumber(char * s);
	bool isMem(char * s);
	bool isLabel(const char * s);
	bool isJmp(const char * s);


	void addSimbolo(const char * s, const int valor);
	void erro(const char * s);

    //passo2
	
	//codifica opcodes
	bool EncodeOpcode(char * instrucao);	
	//grava informa??o no disco
	void grava(const short int val, bool isWord);	
	void grava(const short int val);

	int getOpcode(const char * s, int m);
	int getValReg(const char * s);
	bool is16BitReg(const char *s);

    public:
	inline char* getArqSaida()
	{
		return nomeArqSaida;
	}
	montador();
	virtual ~montador();
	virtual void montar();
	void setArquivoFonte(char * s);


};

#endif // !defined(AFX_MONTADOR_H__B980415D_A6C4_4593_9675_3725164F695F__INCLUDED_)
