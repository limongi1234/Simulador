// montador.cpp: implementation of the montador class.
//
//////////////////////////////////////////////////////////////////////

#include "montador.h"
#include <fstream>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

montador::montador()
{
    //inicializa tabela de registradores	
	strcpy(reg[0], "ax");
	strcpy(reg[1], "al");
	strcpy(reg[2], "ah");
	
	strcpy(reg[3], "bx");
	strcpy(reg[4], "bl");
	strcpy(reg[5], "bh");

	//inicializa Tabela de Mnemonicos
	strcpy(tabMnemonicos[0], "mov");

	strcpy(tabMnemonicos[1], "and");
	strcpy(tabMnemonicos[2], "xor");
	strcpy(tabMnemonicos[3], "or");
	strcpy(tabMnemonicos[4], "not");
	strcpy(tabMnemonicos[5], "cmp");
	
	strcpy(tabMnemonicos[6], "add");
	strcpy(tabMnemonicos[7], "sub");
	strcpy(tabMnemonicos[8], "mul");
	strcpy(tabMnemonicos[9], "div");

    strcpy(tabMnemonicos[10], "jmp");
	strcpy(tabMnemonicos[11], "jc");
	strcpy(tabMnemonicos[12], "jnc");
	strcpy(tabMnemonicos[13], "jg");
	strcpy(tabMnemonicos[14], "jge");
	strcpy(tabMnemonicos[15], "jl");
	strcpy(tabMnemonicos[16], "jle");
	strcpy(tabMnemonicos[17], "je");
	strcpy(tabMnemonicos[18], "jne");

	strcpy(tabMnemonicos[19], "clc");
	strcpy(tabMnemonicos[20], "stc");
	strcpy(tabMnemonicos[21], "hlt");
	strcpy(tabMnemonicos[22], "nop");
}

montador::~montador()
{

}

//Verifica se é um registrador
bool montador::isReg(char * s)
{
	int i;
	for(i=0; i<numReg; i++)
	  if(strcmp(s, reg[i]) == 0)
			return true;
	return false;
}

int montador::isInstr(const char * s)
{
	int i;
	char is[30];
	strcpy(is, s);
	char * token = strtok(is, "\n :;");
	if(token == NULL)
		return -1;
	
	for(i = 0; i < numInstr;i ++)
	  if(strcmp(token, tabMnemonicos[i]) == 0)
			return i;
	return -1;
}

bool montador::isNumber(char * s)
{
	
	if(*s != '[')
	{
		//é uma constante
		if(*s <'0' || *s > '9')		
			addSimbolo(s, -1);
		
		//verifica se é um numero válido
		else
		do
		{
			if(*s < '0' || *s > '9')
				return false;
		}while(*(++s) != 0);
      return true;
	}

	return false;
}

bool montador::isMem(char * s)
{
	
	int i;
	char N[8] = "";
	i = indexOfSimbolo(s) ;

	if(i != -1 && !tabSimbolos[i].constante)
		return true;

	//começa com "[" e termina com "]"
	if(*s == '[')
	{
        for(i=1; i<strlen(s); i++)
		{
			if(s[i] == ']')
				break;
			N[i - 1] = s[i];
		
		}
		if(s[i] != ']')
			erro("sintaxe");
		N[i-1] = '\0';

		//pode ser um número ou uma constante
		if(isNumber(N))
				return true;
	}
	return false;
}

bool montador::isLabel(const char * s)
{
	return s[0] == '@';
}

bool montador::isJmp(const char * s)
{
	return s[0] == 'j';
}

modo_ender montador::modo(char * op1, char * op2)
{
	
	//invalido
	if(op1 == NULL && op2 == NULL)
		return invalido;
    //reg para registrador
	if(isReg(op1) && isReg(op2))
		return RegToReg;else
	//memoria para registrador
	if(isReg(op1) && isMem(op2))	
		return MemToReg;else
	//imediato para registrador
	if(isReg(op1) && isNumber(op2))
		return ImdToReg; else	
	//registrador para memoria
	if(isMem(op1) && isReg(op2))	
		return RegToMem;
    //invalido
	return invalido;
}

//retorna o tamanho de uma instrução
int montador::sizeofInstr(char * instrucao)
{
	char seps[] = " :;,\n";	
	char * operador, * operando1, * operando2;

	operador = strtok(instrucao, seps);	
	operando1 = strtok(NULL, seps);
	operando2 = strtok(NULL, seps);

	//se for um jmp, tem tamanho fixo
	if(operador[0] == 'j')
	{
		addSimbolo(operando1, -1);		
		return 3;
	}else
		//instruções de 1 byte
		if(strcmp(operador, "hlt") == 0 || strcmp(operador, "nop") == 0 || strcmp(operador, "clc") == 0 || strcmp(operador, "stc") == 0)
			return 1; 
	else
		//instruções que usam registrador apenas um operando
		if(strcmp(operador, "div") == 0 || strcmp(operador, "mul") == 0 || strcmp(operador, "not") == 0)
			return 2;
		else
	
	switch(modo(operando1, operando2))
	{
	   //modo reg p/ reg
	   case RegToReg:	return 2;		
	  //modo imed p/ reg
	  case ImdToReg:
		if(is16BitReg(operando1))
			return 3;	
		else
			return 2;
	  //modo mem p/ reg
	  case MemToReg:
		 if(is16BitReg(operando1))
			return 3;	
		 else
			return 2;
	  //modo reg p/ mem
	  case RegToMem:return 3;		
	};

	return 0;
}

void montador::addSimbolo(const char * s, const int valor)
{
	//adiciona o símbolo, caso não foi adicionado				
	int i = indexOfSimbolo(s);
	if(i == -1)					
	{					
		strcpy(tabSimbolos[indexTab].nome, s);
		tabSimbolos[indexTab++].valor = valor;
	}
	//senão, atualiza valor				
	else
		if(tabSimbolos[i].valor == -1)
			tabSimbolos[i].valor = valor;
}

//preenche tabela de simbolos
bool montador::passo1()
{	
	char * token;
	int n = 0;
	char linha[60];

	//lê arquivo
	
	if((arqEntrada = fopen(nomeArqEntrada, "r")) == NULL)
		erro("Arquivo invalido");
	

	while(!feof(arqEntrada))
	{					
		
		fgets(linha, 60, arqEntrada);
		if(linha != NULL)
		{

			strtok(linha, "\n:");
			strlwr(linha);
		
			//********************************************************************
			//se não for comentário 
			//********************************************************************
			if(linha[0] != ';')

			//********************************************************************
			//se fim do programa
			//********************************************************************
			if(strcmp(strtok(linha, ";"), "endp") == 0)
				return true;
			else

			//********************************************************************
			//se instrução, incrementa contador de posição de acordo com tamanho da instrução
			//********************************************************************
			if(isInstr(linha) > -1)
				pos += sizeofInstr(linha); 		
			else
				//********************************************************************
				//se for rótulo(começa com @)
				//********************************************************************
				if(isLabel(linha))
				{				
					token = strtok(linha, separadores);
					addSimbolo(token, pos);
				}

				//********************************************************************
				//se for declaração de variável ou constante
				//********************************************************************			
				else
				{
					token = strtok(linha, " :;\n");
					if(token != NULL)
					{
				
						if(indexOfSimbolo(token) == -1)
							strcpy(tabSimbolos[indexTab++].nome, token);
						else
							erro("simbolo redeclarado");				

						token = strtok(NULL, " :;");
					
						if(token == NULL)
							erro("diretiva nao reconhecida");
					

					//é um byte
					if(strcmp(token, "db") == 0)
					{
						token = strtok(NULL, separadores);
						//não cabe na posição
						if(atoi(token) > 0xFF)
							erro("fora dos limites");
						int indexx = (indexTab - 1);
						
						//coloca o endereço na tabela de simbolos
						tabSimbolos[indexx].valor = pos;					
						pos ++;
					}
					else
						//é uma word
						if(strcmp(token, "dw") == 0)
						{
							token = strtok(NULL, separadores);
							//não cabe na posição
							if(atoi(token) > 0xFFFF)
								erro("fora dos limites");
							int indexx=(indexTab - 1);
							
							//coloca o endereço na tabela de simbolos
							tabSimbolos[indexx].valor = pos;
							pos += 2;
						}
						else
							//é uma constante
							if(strcmp(token, "equ") == 0)
							{
								token = strtok(NULL, separadores);
								int indexx = (indexTab - 1);
								//coloca valor da constante
								tabSimbolos[indexx].valor = atoi(token);
								tabSimbolos[indexx].constante = true;
							
							}else
								//********************************************************************			
								//erro(argumento inválido)
								//********************************************************************
								erro("argumento invalido");
					}
				}
				n++;
		}
	}

	fclose(arqEntrada);
	return true;
}

//gera arquivo montado
bool montador::passo2()
{

    int n = 0;
	char linha[60];
	char * token;
	
	arqSaida = fopen(nomeArqSaida, "wb");
	arqEntrada = fopen(nomeArqEntrada, "r");

	while(!feof(arqEntrada))
	{
		
		fgets(linha, 60, arqEntrada);
		strlwr(linha);
		
		if(linha != NULL)
		{
			//********************************************************************
			//se comentário, ignora linha
			//********************************************************************
			cout<<endl<<linha;
			if(linha[0] != ';')
			{

				//********************************************************************
				//se instrução, codifica instrução
				//********************************************************************
				if(isInstr(linha) > -1)
					EncodeOpcode(linha);
				
				else
				//********************************************************************
				//se for declaração de variável ou constante, grava valores
				//********************************************************************			
				{			

					token = strtok(linha, " :;");

					if(indexOfSimbolo(token) != -1)
					{
						token = strtok(NULL, " :;");						
						//oito bits
						if(strcmp(token, "db") == 0)
						{
							token = strtok(NULL, separadores);
							
							grava(atoi(token), false);
						}else
							//16 bits
							if(strcmp(token, "dw") == 0)
							{
								token = strtok(NULL, separadores);
								grava(atoi(token), true);
							}
					}

				
				}
			}
		}
	}

	fclose(arqEntrada);
	fclose(arqSaida);
	return false;

}

void montador::montar()
{
    char * s = nomeArqEntrada; 
	int i;

		//inicializa tabela de simbolos
	for(i = 0; i < SZ_TAB_SIMB; i ++)
	{
		strcpy(tabSimbolos[i].nome, "");
		tabSimbolos[i].valor = 0;
		tabSimbolos[i].constante = false;
	}
	
	//monta o nome do arquivo de saída com a extensão .mem
	i = 0;
	do
		nomeArqSaida[i++] = *s;
	while(*(++s) != '.');

	nomeArqSaida[i] = '\0';
	strcat(nomeArqSaida, ".mem");

	cout << "Arquivo fonte: " << nomeArqEntrada << endl;
	cout << "Arquivo destino: " << nomeArqSaida << endl;

	//inicializa contadores
	indexTab = 0;
	pos = 0;

	//preenche tabela de simbolos
	passo1();
	

	//gera arquivo
	cout << endl;
	passo2();
	cout << endl << "Montagem efetuada com sucesso!" << endl;
	system("pause");
	system("cls");
}


void montador::setArquivoFonte(char * s)
{
	strcpy(nomeArqEntrada, s);
}


int montador::indexOfSimbolo(const char * s)
{
	int i;

	if(s != NULL)
	for(i=0; i<1000; i++)
       if(strcmp(s, tabSimbolos[i].nome) == 0)
			return i;
	return -1;
}

void montador::erro(const char * s)
{
	cout << "ERRO - " << s <<
		endl;
	system("pause");
	exit(1);
}

void montador::grava(const short int val, bool isWord)
{
	static int i = 0;
	unsigned char b1, b2;

	if(isWord)
	{
		//valor deve ser quebrado em dois bytes
		b1 = (val & 0xFF00) >> 8;
		b2 = val % 0x100;

		fwrite(&b1, 1, 1, arqSaida);			
		printf("\t%d: %Xh\n", i ++, b1);
		fwrite(&b2, 1, 1, arqSaida);
		printf("\t%d: %Xh\n", i ++, b2);
	}else
	{
		b1 = (char)val;

		fwrite(&b1, 1, 1, arqSaida);
		printf("\t%d: %Xh\n", i++, b1);		

	}
}

void montador::grava(const short int val)
{
	grava(val, false);
}


bool montador::EncodeOpcode(char * instrucao)
{
	char seps[] = " ;,:\n";	
	char * operador, * operando1, * operando2;
	modo_ender  m;
	int i;
	unsigned char opcode;
	short int b;
	
	//cout<<endl<<"-"<<instrucao;

	//instruções de sem operandos
	operador = strtok(instrucao, seps);	
	operando1 = strtok(NULL, seps);
	operando2 = strtok(NULL, seps);

	if( strcmp(operador, "hlt") == 0 ||
		strcmp(operador, "nop") == 0||
		strcmp(operador, "stc") == 0||
		strcmp(operador, "clc") == 0)
	{
		//grava opcode
		grava(getOpcode(operador, 0));
	}else
		
	//intruçoes que usam um operando
	if( strcmp(operador, "div") == 0 ||
		strcmp(operador, "mul") == 0 ||
		strcmp(operador, "not") == 0)
	{
			//grava opcode
			grava(getOpcode(operador, 0));
			//grava codigo do registrador
			grava(getValReg(operando1));

	}else

	//instruções de salto 
	if(operador[0] == 'j')
	{
		i = indexOfSimbolo(operando1);
		if(i == -1)
			erro("Símbolo não encontrado");	
		
		//grava opcode
		grava(getOpcode(operador, 0));
		//grava endereço
		//o valor gravado é de 16 bits
		if(tabSimbolos[i].valor<256)
			grava(0);		
		grava((int)tabSimbolos[i].valor);

	//demais instruções
	}else
	{
		//decodifica valor do operando 2 quando for constante ou variáveis
		if(operando2 != NULL)
		{
			if((i = indexOfSimbolo(operando2)) != -1)
			   b = tabSimbolos[i].valor;
	        else

			//se estiver entre colchetes
			if(operando2[0] == '[')
			{
				char s[30];	
				char * k;
				strcpy(s, operando2);
				k = strtok(s, "[]");
				strcpy(s, k);
				i = indexOfSimbolo(s);

				//se for número
				if(i == -1 && isNumber(s))
					b = atoi(s);
				else		
				//se for variável ou constante
					b = tabSimbolos[i].valor;
			}else
				//se for número
				b = atoi(operando2);
		}
		
		
		//obtêm modo de endereçamento
		m = modo(operando1, operando2);
		//obtêm opcode
		opcode = getOpcode(operador, m);

		//modo inválido = erro
		if(m == invalido)
			erro("enderecamento invalido");else
		switch(m)
		{
			case RegToReg:
				grava(opcode + getValReg(operando1));
				grava(getValReg(operando2));
				break;

			case RegToMem:

				//se estiver não estiver entre colchetes			
				if((i = indexOfSimbolo(operando1)) != -1)
				   b = tabSimbolos[i].valor;
				else
				//se estiver entre colchetes
				if(operando1[0] == '[')
				{
					char s[30];	
					char * k;
					strcpy(s, operando1);
					k = strtok(s, "[]");
					strcpy(s, k);
					i = indexOfSimbolo(s);

					//se for número
					if(i == -1 && isNumber(s))
						b = atoi(s);
					else		
					//se for variável ou constante
						b = tabSimbolos[i].valor;
				}

				grava(opcode + getValReg(operando2));
				grava(b, is16BitReg(operando2));
				break;

			default:
				grava(opcode + getValReg(operando1));			
				grava(b, is16BitReg(operando1));
				break;
		}
    }


	/*
	opc(5 bits)
	[---]
		 (reg 3 bits)	
		 [-]

	00000000//nop
	00000001//clc
	00000010//stc
	00000011//hlt

	00001 000//mov
	00010 000
	00011 000
	00100 000
	00101 000//and
	00110 000
	00111 000
	01000 000//or
	01001 000
	01010 000
	01011 000//xor
	01100 000
	01101 000
	01110 000//not reg
	
	01111 000//add
	10000 000
	10001 000
	10010 000//sub
	10011 000
	10100 000
	10101000//div
	10111001//mul

	10111010//jmp
	10111011//JMP 
	10111100//jc
	10111101//JnC 
	10111110//Je 
	10111111//Jne
	11000000//Jl
	11001000//jle
	11010000//jg
	11011000//jge

	11100 000//cmp
	11101 000
	11110 000

	11111000//
*/

	return 0;
}

int montador::getOpcode(const char * s, int m)
{

	if(strcmp(s, "nop") == 0) return nop;
	if(strcmp(s, "clc") == 0) return clc;
	if(strcmp(s, "stc") == 0) return stc;
	if(strcmp(s, "hlt") == 0) return hlt;	
	
	

	if(strcmp(s, "mov") == 0) 
		switch(m)
		{
		  case RegToReg: return movRegToReg;
		  case ImdToReg: return movImdToReg;
		  case MemToReg: return movMemToReg;
		  case RegToMem: return movRegToMem;	
		}

		if(strcmp(s, "and") == 0) 
		switch(m)
		{
		  case RegToReg: return andRegToReg;		 
		  case ImdToReg: return andImdToReg;
		  case MemToReg: return andMemToReg;
		}

		if(strcmp(s, "or") == 0) 
		switch(m)
		{ 
		  case RegToReg: return orRegToReg;
		  case ImdToReg: return orImdToReg;
		  case MemToReg: return orMemToReg;
		}

		if(strcmp(s, "xor") == 0) 
		switch(m)
		{
		   case RegToReg: return xorRegToReg;
		   case ImdToReg: return xorImdToReg;
		   case MemToReg: return xorMemToReg;
		}
		
		if(strcmp(s, "not") == 0) 
			return notRegToReg;

		if(strcmp(s, "add") == 0) 
		switch(m)
		{
		   case RegToReg: return addRegToReg;
		   case ImdToReg: return addImdToReg;
		   case MemToReg: return addMemToReg;
		}


		if(strcmp(s, "sub") == 0) 
		switch(m)
		{
		   case RegToReg: return subRegToReg;
		   case ImdToReg: return subImdToReg;
		   case MemToReg: return subMemToReg;
		}

		if(strcmp(s, "cmp") == 0) 
		switch(m)
		{
		   case RegToReg: return cmpRegToReg;
		   case ImdToReg: return cmpImdToReg;
		   case MemToReg: return cmpMemToReg;
		}

		if(strcmp(s, "div") == 0) 
			return divRegToReg;
		if(strcmp(s, "mul") == 0) 
			return mulRegToReg;

		if(strcmp(s, "jmp") == 0) 
			return jmp;
		if(strcmp(s, "jc")  == 0) 
			return jc;
		if(strcmp(s, "jnc")  == 0) 
			return jnc;
		if(strcmp(s, "je")  == 0) 
			return je;
		if(strcmp(s, "jne") == 0) 
			return jne;
		if(strcmp(s, "jl") == 0) 
			return jl;
		if(strcmp(s, "jle")  == 0) 
			return jle;
		if(strcmp(s, "jg") == 0) 
			return jg;
		if(strcmp(s, "jge")  == 0) 
			return jge;
		
	return 0;
}


int montador::getValReg(const char * s)
{
	if(strcmp(s, "ax") == 0)
		return 0;
	if(strcmp(s, "al") == 0 )
		return 1;
	if(strcmp(s, "ah") == 0 )
		return 2;
	if(strcmp(s, "bx") == 0 )
		return 3;
	if(strcmp(s, "bl") == 0 )
		return 4;
	if(strcmp(s, "bh") == 0 )
		return 5;

	return 0;
}


bool montador::is16BitReg(const char *s)
{
	return (strcmp(s, "ax") == 0) || (strcmp(s, "bx") == 0);
}