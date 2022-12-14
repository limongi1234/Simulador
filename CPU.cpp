// CPU.cpp: implementation of the CPU class.
//
//////////////////////////////////////////////////////////////////////

#include "CPU.h"
#include <iostream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPU::CPU()
{

}

CPU::CPU(char * Arquivo)
{
	
	this->delay=2000;
	int i;
	FILE * f;
	f = fopen(Arquivo, "rb");
		
	for(i = 0; i < SZ_MEM; i ++)
		Memoria[i] = 0;

	if(f != NULL)
		fread(Memoria, SZ_MEM, sizeof(unsigned char), f);
	
	fclose(f);


	//Tabela de Mnemonicos em fun??o dos opcodes
	//inicializa
	for(i = 0; i < 256; i ++)
		opMnemonicoTab[i]= INVALIDO;

	opMnemonicoTab[nop] = NOP;
	opMnemonicoTab[hlt] = HLT;
	opMnemonicoTab[stc] = STC;
	opMnemonicoTab[stc] = CLC;

	

	opMnemonicoTab[movRegToReg] = MOV; opMnemonicoTab[movImdToReg] = MOV; opMnemonicoTab[movMemToReg] = MOV; opMnemonicoTab[movRegToMem] = MOV;	
	opMnemonicoTab[andRegToReg] = AND; opMnemonicoTab[andImdToReg] = AND; opMnemonicoTab[andMemToReg] = AND;
	opMnemonicoTab[orRegToReg] = OR; opMnemonicoTab[orImdToReg] = OR; opMnemonicoTab[orMemToReg] = OR;
	opMnemonicoTab[xorRegToReg] = XOR; opMnemonicoTab[xorImdToReg] = XOR; opMnemonicoTab[xorMemToReg] = XOR;
	opMnemonicoTab[cmpRegToReg] = CMP; opMnemonicoTab[cmpImdToReg] = CMP; opMnemonicoTab[cmpMemToReg] = CMP;
	opMnemonicoTab[notRegToReg] = NOT;
	
	opMnemonicoTab[addRegToReg] = ADD; opMnemonicoTab[addImdToReg] = ADD; opMnemonicoTab[addMemToReg] = ADD;
	opMnemonicoTab[subRegToReg] = SUB; opMnemonicoTab[subImdToReg] = SUB; opMnemonicoTab[subMemToReg] = SUB;
	opMnemonicoTab[divRegToReg] = DIV; 
	opMnemonicoTab[mulRegToReg] = MUL; 

	opMnemonicoTab[jmp] = JMP;
	opMnemonicoTab[jc] = JC; opMnemonicoTab[jnc] = JNC;
	opMnemonicoTab[je] = JE; opMnemonicoTab[jne] = JNE;
	opMnemonicoTab[jg] = JG; opMnemonicoTab[jge] = JGE;
	opMnemonicoTab[jl] = JL; opMnemonicoTab[jle] = JLE;


}

CPU::~CPU()
{

}

void CPU::executa()
{
	unsigned short int tmp1;
	unsigned short int tmp2;
	unsigned short int tmp3;
	unsigned short int instrIP;
	char instr[10];
	char oper1[10];
	char oper2[10];
	unsigned char D;
	unsigned char F;
	modo_ender modo;
	this-> log = fopen("log.txt","w");
	Mnemonico M;
	Reset();
	while(this -> Memoria[IP]!=hlt)
	{
		
		//***************************************************************************************************
		//busca instru??o na memoria
		//***************************************************************************************************
		IR = LeByte(IP);
		M = GetMnemonico(IR);
		modo = invalido;
		instrIP = IP;
		
		//***************************************************************************************************
		// instru??es que n?o requerem operandos
		//***************************************************************************************************
		switch(IR)
		{
		
		case nop:
			IP++;
			break;
		
		case clc:
			flags.C = 0;
			IP ++;
			break;
		case stc:
			flags.C = 1;
			IP ++;
			break;

		case hlt:			
			//exit(1);
			break;

		//***************************************************************************************************
		// JMP?s
		//***************************************************************************************************
			
		case jmp:
			tmp1  = IP = LeWord(IP + 1);
			break;

		//condicionais
			
		case jc:
			if(flags.C)
				IP = LeWord(IP + 1);
			else
				IP += 3;
			tmp1 = IP;
			break;

		case jnc:
			if(!flags.C)
				IP = LeWord(IP + 1);
			else
				IP += 3;
			tmp1 = IP;
			break;

			
		case je:
			if(flags.Z)
				IP = LeWord(IP + 1);
			else
				IP += 3;
			tmp1 = IP;
			break;

		case jne:
			if(!flags.Z)
				IP = LeWord(IP + 1);
			else
				IP += 3;
			tmp1 = IP;
			break;

		case jl:
			if(flags.S != flags.O)
				IP = LeWord(IP + 1);
			else
				IP += 3;
			tmp1 = IP;
			break;

		case jle:
			if(flags.S != flags.O || flags.Z == 1)
				IP = LeWord(IP + 1);
			else
				IP += 3;
			tmp1 = IP;
			break;

		case jg:
			if(flags.Z == 0 && (flags.S == flags.O))
				IP = LeWord(IP + 1);
			else
				IP += 3;
			tmp1 = IP;
			break;

		case jge:
			if(flags.S ==  flags.O)
				IP = LeWord(IP + 1);
			else
				IP += 3;
			tmp1 = IP;
			break;




		default:
			//***************************************************************************************************
			//*				prepara tmp1 e tmp2
			//*
			//***************************************************************************************************

			

			if(M == NOT || M == DIV || M == MUL)
			{
				modo  = GetModo(IR);
				D = LeByte(IP + 1);//pega registrador sequente

			}else
			{
				
				M = GetMnemonico(IR & 0xF8);
				modo  = GetModo(IR & 0xF8);
				D = IR & 0x07;//pega registrador impl?cito no opcode
			}
		
			//se modo RegToReg tmp2 contem o valor do registrador fonte
            if(modo == RegToReg)
			{	
				F = LeByte(IP+1);
				if(M == DIV || M == MUL)
					tmp2 = AX.word;else
				switch(F)
				{
					case rAX: tmp2 = AX.word; break;				
					case rAH: tmp2 = AX.HighByte; break;
					case rAL: tmp2 = AX.LowByte; break;
					case rBX: tmp2 = BX.word; break;
					case rBH: tmp2 = BX.HighByte; break;
					case rBL: tmp2 = BX.LowByte; break;
				}				

				IP += 2;				

			}else
			//senao, tmp2 recebe valor lido da memoria				
			{
				if(D == rAX || D == rBX )
				{
					tmp2 = LeWord(IP+1);
					IP += 3;
				} else 
				{
					tmp2 = LeByte(IP+1);
					IP+= 2;
				}
				//se modo MemToReg tmp2 cont?m o valor lido da memoria apontada
				//tmp3 salva a posi??o de mem?ria
				if(modo == MemToReg)
				{
					
					tmp3 = tmp2;//esta vari?vel ? usada no disassembly somente

					if(D == rAX || D == rBX)
						tmp2 = LeWord(tmp2);
					else tmp2 = LeByte(tmp2);
				}
			}

			
			//tmp1 recebe valor do registrador impl?cito no opcode
			switch(D)
			{
			   case rAX: tmp1 = AX.word; break;				
			   case rAH: tmp1 = AX.HighByte; break;				
			   case rAL: tmp1 = AX.LowByte; break;
			   case rBX: tmp1 = BX.word; break;
			   case rBH: tmp1 = BX.HighByte; break;
			   case rBL: tmp1 = BX.LowByte; break;
			}




			//***************************************************************************************************
			//* Executa instru??o
			//***************************************************************************************************
			//verifica Mnemonico			
			switch(M)
			{
			case MOV:
				if(modo != RegToMem)
					tmp1 = tmp2;
				break;
			//altera S e Z
			case AND: 
				tmp1 &= tmp2;
				flags.C = 0;
				flags.O = 0;
				flags.Z = tmp1 == 0;
				if(is16BitReg(D))
					flags.S = tmp1 & 0x8000 == 0x8000;
				else
					flags.S = (tmp1 & 0x80) == 0x80;							

				break;
			case OR:  
				tmp1 |= tmp2;
				flags.C = 0;
				flags.O = 0;

				flags.Z = tmp1 == 0;

				if(is16BitReg(D))
					flags.S = tmp1 & 0x8000 == 0x8000;
				else
					flags.S = (tmp1 & 0x80) == 0x80;							

				break;
			case 
				XOR: tmp1 ^= tmp2;
				flags.C = 0;
				flags.O = 0;

				flags.Z = tmp1 == 0;

				if(is16BitReg(D))
					flags.S = tmp1 & 0x8000 == 0x8000;
				else
					flags.S = (tmp1 & 0x80) == 0x80;							

				break;

			case CMP: 
				unsigned short int tt;

				tt = tmp1 - tmp2;
				flags.C = tmp2 > tmp1;					

				if(is16BitReg(D))
				{
					
					flags.O = tmp2 > (0x8000 - tmp1);
					
				}else
				{
				//	flags.C = tmp2 > (0xFF - tmp1);					
					flags.O = tmp2 > (0x80 - tmp1);

				}

				tt = tmp1 - tmp2;

				if(is16BitReg(D))

					flags.S = (tt & 0x8000) == 0x8000;
				else
					flags.S = (tt & 0x80) == 0x80;

				
				

				flags.Z = tt == 0;
				break;


			//n?o altera flags
			case NOT: 
				tmp1 = ~tmp1;
				break;

			case ADD: 
				if(is16BitReg(D))
				{
					flags.C = tmp2 > (0xFFFF - tmp1);
					flags.O = tmp2 > (0x8000 - tmp1);
					
				}else
				{
					flags.C = tmp2 > (0xFF - tmp1);
					flags.O = tmp2 > (0x80 - tmp1);

				}

				tmp1 = tmp1 + tmp2;

				if(is16BitReg(D))
					flags.S = (tmp1 & 0x8000) == 0x8000;
				else
					flags.S = (tmp1 & 0x80) == 0x80;


				
				flags.Z = tmp1 == 0;
				
				break;
			case SUB: 

				if(is16BitReg(D))
				{
					flags.C = tmp2 > (0xFFFF - tmp1);
					flags.O = tmp2 > (0x8000 - tmp1);
					
				}else
				{
					flags.C = tmp2 > (0xFF - tmp1);
					flags.O = tmp2 > (0x80 - tmp1);

				}

				tmp1 = tmp1 - tmp2;

				if(is16BitReg(D))
					flags.S = (tmp1 & 0x8000) == 0x8000;
				else
					flags.S = (tmp1 & 0x80) == 0x80;


				flags.Z = tmp1 == 0;

				break;

			//somente quando divisor for de 8 bits
			case DIV:

				if(tmp1 == 0)
					exit(1);

				tmp1 = tmp2 / tmp1;
				D = rAL;
				AX.HighByte = tmp2%tmp1;
				break;

			case MUL: 
				tmp1 = tmp2 * tmp1;
				D = rAX;
				if(tmp1 <= 0xFF)
				{
					flags.C = 0;
					flags.Z = 0;
				}else
				{
					flags.C = tmp2 > (0xFFFF - tmp1);
					flags.S = (tmp1 & 0x8000) == 0x8000;
					flags.O = tmp2 > (0x8000 - tmp1);
				}
				break;

			}

			//***************************************************************************************************
			//* Faz registrador/memoria de destino receber  resultado
			//***************************************************************************************************

			if(modo == RegToMem)
			{
				switch(D)
				{
				  case rAX: Memoria[tmp2] = AX.HighByte; Memoria[tmp2 + 1] = AX.LowByte; break;			
				  case rAL: Memoria[tmp2] = AX.LowByte; break;
				  case rAH: Memoria[tmp2] = AX.HighByte; break;

				  case rBX: Memoria[tmp2] = BX.HighByte; Memoria[tmp2 + 1] = BX.LowByte; break;			
				  case rBL: Memoria[tmp2] = BX.LowByte; break;
				  case rBH: Memoria[tmp2] = BX.HighByte; break;				
				}


			}else				
			    switch(D)
			    {
			      case rAX: AX.word = tmp1;break;			
			      case rAL: AX.LowByte = (unsigned char)tmp1; break;
			      case rAH: AX.HighByte = (unsigned char)tmp1; break;
			
			      case rBX: BX.word = tmp1;break;			
			      case rBL: BX.LowByte = (unsigned char)tmp1; break;
			      case rBH: BX.HighByte = (unsigned char)tmp1; break;
			    }
		}
		
		//***************************************************************************************************
		// DISASSEMBLY-
		// CONVERTE A INSTRU??O EM UMA SITRNG
		//***************************************************************************************************

		MnemToStr(M, instr);
		strcpy(oper1,"");
		strcpy(oper2,"");

		
		switch(modo)
		{
		  case RegToReg:
			RegToStr(D, oper1);
			RegToStr(F, oper2);
			break;
		  case ImdToReg:
			RegToStr(D, oper1);
			itoa(tmp2, oper2, 10);
			break;
		  case MemToReg:
			RegToStr(D, oper1);
			itoa(tmp3, oper2, 10);
			break;
		  case RegToMem:
			RegToStr(D, oper2);
			itoa(tmp2, oper1, 10);
			break;
		  default:
			if(instr[0] == 'J')
				itoa(tmp1, oper1, 10);
        }
		char g[10] = ", ";
		
		if(strcmp(oper2,"") != 0)
		{
			strcat(g, oper2);
			strcpy(oper2, "");
			strcat(oper2, g);
		}
		
		printf("\n-Instrucao:\n");
		printf("%Xh: %s %s%s\n", instrIP, instr, oper1, oper2);	
		fprintf(this->log,"\n-Instrucao:\n%Xh: %s %s%s\n", instrIP, instr, oper1, oper2);
		Imprime();			
	}
	
	cout << "Fim da execucao!" << endl;
	//system("pause");
	//system("cls");
	fclose(this->log);
	return;
}

void CPU::Reset()
{
	IP = 0;
	AX.word = 0;
	BX.word = 0;
	flags.C = 0;
	flags.Z = 0;
	flags.O = 0;
	flags.S = 0;
}
void CPU::Imprime()
{
	cout << "\n-Registradores: " << endl;
	printf("\tIP = % 0.4Xh \t \t AX = % 0.4Xh \t \t BX = % 0.4Xh", IP, AX.word, BX.word);
	cout << endl << "\n-Flags: " << endl;
	cout << "\tC = " << flags.C << "\t \t O = " << flags.O << "\t \t Z = " << flags.Z << "\t \t S = " << flags.S << endl;
	cout << "--------------------------------------------------------------------------------" << endl;

	fprintf(this -> log,"\n-Registradores: \n \t IP = % 0.4Xh \t \t AX = % 0.4Xh \t \t BX = %0.4Xh", IP, AX.word, BX.word);
	fprintf(this -> log,"\n-Flags: \n \t C = % d \t O = % d \t Z = % d \t S = %d \n\n",flags.C,flags.O,flags.Z,flags.S);
	fprintf(this -> log,"--------------------------------------------------------------------------------");
	
	if(this -> modoExec == STEP)
	   system("pause");
	else
	   sleep(this->delay);
}

modo_ender CPU::GetModo(int opcode)
{
	if( opcode == movRegToReg || 
		opcode == andRegToReg ||
		opcode ==  orRegToReg ||
		opcode == xorRegToReg ||
		opcode == notRegToReg ||
		opcode == addRegToReg ||
		opcode == subRegToReg ||
		opcode == divRegToReg ||
		opcode == mulRegToReg ||
		opcode == cmpRegToReg
		)return RegToReg;

	if( opcode == movImdToReg ||
		opcode == andImdToReg ||
		opcode ==  orImdToReg ||
		opcode == xorImdToReg ||		
		opcode == addImdToReg ||
		opcode == subImdToReg ||
		opcode == cmpImdToReg


		)return ImdToReg;

	if( opcode == movMemToReg ||
		opcode == andMemToReg ||
		opcode ==  orMemToReg ||
		opcode == xorMemToReg ||		
		opcode == addMemToReg ||
		opcode == subMemToReg ||
		opcode == cmpMemToReg

		)return MemToReg;

	if( opcode == movRegToMem ||
		opcode == movRegToMem 
		)return RegToMem;

	return RegToReg;

}

bool CPU::is16BitReg(unsigned char r)
{
	return r == rAX || r == rBX;
}

Mnemonico CPU::GetMnemonico(unsigned char opcode)
{
	return opMnemonicoTab[opcode];
}

unsigned char CPU::LeByte(unsigned short int indice)
{
	return Memoria[indice];
}

unsigned short int CPU::LeWord(unsigned short int indice)
{
	return (Memoria[indice] <<8 | Memoria[indice + 1]);
}

//Converte Mnemonico em string
char * CPU::MnemToStr(unsigned char m, char * s)
{
	switch(m)
	{

	   case HLT: return strcpy(s,"HLT");
	   case NOP: return strcpy(s,"NOP");
	   case STC: return strcpy(s,"STC");
	   case CLC: return strcpy(s,"STC");

	   case MOV: return strcpy(s,"MOV");

	   case AND: return strcpy(s,"AND");
	   case OR: return strcpy(s,"OR");
	   case XOR: return strcpy(s,"XOR");
	   case NOT: return strcpy(s, "NOT");
	   case CMP:return strcpy(s,"CMP");

	   case SUB: return strcpy(s, "SUB");
	   case ADD: return strcpy(s, "ADD");
	   case DIV: return strcpy(s, "DIV");
	   case MUL: return strcpy(s, "MUL");

       case JMP:return strcpy(s,"JMP");
	   case JC :return strcpy(s,"JC");
	   case JNC :return strcpy(s,"JNC");
	   case JE :return strcpy(s,"JE");
	   case JNE :return strcpy(s,"JNE");
	   case JL :return strcpy(s,"JL");
	   case JLE :return strcpy(s,"JLE");
	   case JG :return strcpy(s,"JG");
	   case JGE :return strcpy(s,"JGE");
    }
	return strcpy(s, "NONE");
}

char * CPU::RegToStr(unsigned char r, char *s)
{
	switch(r)
	{
	   case rAX: return strcpy(s, "ax");
	   case rAH: return strcpy(s, "ah");
	   case rAL: return strcpy(s, "al");
	   case rBX: return strcpy(s, "bx");
	   case rBH: return strcpy(s, "bh");
	   case rBL: return strcpy(s, "bl");
	   default:
		 return strcpy(s, "");
    }
}

void CPU::menu()
{
	cout << "######################################################################" << endl;
	cout << "#** << SIMULADOR 8086 >> ********************************************#" << endl;
	cout << "######################################################################" << endl;
	cout << "#                                                                    #" << endl;
	cout << "#                                                                    #" << endl;
	cout << "#   [1] Executar                   [2] Executar Passo a Passo        #" << endl;
	cout << "#   [3] Mostrar log de execucao    [4] Help                          #" << endl;
	cout << "#   [5] Tempo de delay             [6] Sair                          #" << endl;
	cout << "#                                                                    #" << endl;
	cout << "#                                                                    #" << endl;
	cout << "######################################################################\n" << endl;
	cout << ">> Digite uma opcao valida: ";
}


void CPU::help()
{
	system("cls");
	cout << "###################################################################\n";
	cout << "#                                                                 #\n";
	cout << "       #   #        #######            #              #####      \n";
	cout << "       #   #        #                  #              #   #      \n";
	cout << "       #####        ####               #              #####      \n";
	cout << "       #   #        #                  #              #          \n";
	cout << "       #   #        #######            ######         #          \n";
	cout << "#                                                                 #\n";
	cout << "###################################################################\n";
	cout << "\n\nBem vindo ao help do Simulador. Aqui voce vera as principais opcoes\n";
	cout << "encontradas no programa. Ao executar , voce devera informar o arquivo\n";
	cout << "que sera montado e depois simulado. Apos isso, se tudo correr bem o \n";
	cout << "montador ira gerar um arquivo *.mem que logo em seguida sera passado\n";
	cout << "ao simulador. A tela principal sera exibida e a simulacao/execucao do\n";
	cout << "programa podera ser realizada.As opcoes do menu sao descritas abaixo:\n\n";
	cout << "\n[Executar]: consiste na simples execucao do codigo que foi passado\n"; 
	cout << "anteriormente. O programa e executado ate seu fim, e a cada passo os \n";
	cout << "registradores, flags e a instrucao corrente e mostrado na tela. A cada\n";
	cout << "execucao existe um delay, ou seja, um tempo de espera a cada execucao. \n";
	cout << "Este tempo pode ser alterado de acordo com a preferencia do usuario na opcao\n";
	cout << "\"Tempo de delay\"\n";
	cout << "\n[Executar Passo a Passo]: essa opcao tambem executa o programa gerado no\n";
	cout << "montador, porem de forma passo a passo. Dessa forma, cada instrucao e \n";
	cout << "executada, e apos o termino da execucao da instrucao corrente, e mostrado \n";
	cout << "na tela os registradores,flags e a instrucao que foi executada. A proxima \n";
	cout << "instru??o so sera executada quando alguma tecla for pressionada, ou seja,\n";
	cout << "dessa forma o usuario que controla a execucao do programa.\n";
	cout << "\n[Mostrar Log de Execucao]: nessa opcao e poss?vel visualizar um log de execucao,";
	cout << "ou seja, os passos feitos na ultima execucao do programa que foi realizada. \n";
	cout << "O arquivo de log e aberto usando o \"edit\" do MSDOS. O arquivo de log e \n";
	cout << "gerado implicitamente no mesmo momento em que e exibida as informa??es de\n";
	cout << "cada instrucao na tela.\n";
	cout << "\n[Tempo de Delay]:nessa opcao e poss?vel definir o tempo de delay que sera\n";
	cout << "usado na execu??o passo a passo descrita anteriormente.\n";
	cout << "\n[Sair]: sair do programa, aborta voltando ao Sistema Operacional.\n";
	cout << endl;
}

void CPU::Interface()
{
	int choice = 0;
menu:
	this -> menu();
	cin >> choice;
	switch(choice)
	{
	case 1:
		this -> modoExec=EXEC;
		this -> executa();
		system("pause");
		system("cls");
		goto menu;
	case 2:
		this -> modoExec = STEP;
		this -> executa();
		system("pause");
		system("cls");
		goto menu;
	case 3:
		system("edit log.txt");
		system("pause");
		system("cls");
		goto menu;
	case 4:
		this -> help();
		system("pause");
		system("cls");
		goto menu;
	case 5:
		cout << "Entre com o tempo de delay para execucao de cada instrucao(em segundos):";
		cin >> this -> delay;
		this -> delay * = 1000;
		system("pause");
		system("cls");
		goto menu;
	case 6:
		exit(1);
	default:
		cout << "Nenhuma opcao valida foi escolhida!" << endl;
		system("pause");
		system("cls");
		goto menu;
	}
}

void sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock()); 
}
