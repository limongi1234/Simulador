#include <iostream>
#include "CPU.h"
#include "montador.h"
#include <Windows.h>

using namespace std;

int main()
{
	char s[60];

	system("color 02");
	montador * m = new montador();
	cout << "\n######################################################################" << endl;
	cout << "#      |    |     |    |                                             #" << endl;
	cout << "#------[M|O|N|T|A|D|O|R]---------------------------------------------#" << endl;
	cout << "#      |    |     |    |                                             #" << endl;
	cout << "######################################################################\n" << endl;
	cout << ">>Digite nome do arquivo fonte:";
	cin >> s;
	m -> setArquivoFonte(s);
	m -> montar();	
	CPU * cpu = new CPU(m -> getArqSaida());
	cpu -> Interface();	
	system("pause");

    return 0;
}