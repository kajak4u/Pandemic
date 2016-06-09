#include <iostream>
#include <qvector.h>
#include <qpair.h>


#include "Board.h"

using namespace std;

int main()
{
	int players, difficulty;
	cout << "podaj liczbe graczy: ";
	cin >> players;
	cout << endl << "Poziom <0-2>: ";
	cin >> difficulty;
	QVector<QPair<QString, PlayerRole>>gracze;
	string nickname;
	int role;
	for (int i = 0; i < players; i++)
	{
		cout << "Nick: ";
		cin >> nickname;
		cout << endl << "Rola:" << endl << "ROLE_RANDOM = 0,\nROLE_DISPATCHER,\nROLE_MEDIC,\nROLE_OPERATIONSEXPERT,\nROLE_RESEARCHER,\nROLE_SCIENTIST" << endl;
		cin >> role;
		QPair<QString, PlayerRole> gracz(QString::fromStdString(nickname),(PlayerRole)role);
		gracze.push_back(gracz);
	}

	Board gra((Difficulty)difficulty, gracze);//(players, difficulty);


	return 0;
}