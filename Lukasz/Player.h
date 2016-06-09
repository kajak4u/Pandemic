#pragma once
#include <vector>
#include "PlayerCard.h"
#include "SpecialCard.h"
#include "Disease.h"
#include <algorithm>
#include "Mediator.h"
//#include "enumShort.h"

using namespace std;

class Player
{
	vector<PlayerCard*> hand;
	City* position;
	const string nick;
	const PlayerRole role; 
public:
	vector<PlayerCard*> SeeCards();
	PlayerCard* FindCard(string cardName);
	PlayerRole GetRole() const;
	City* GetPosition() const;
    string GetNick() const { return nick; }

	void MoveToNeighbour(City* target); //ruch do stacji - dziala dokladnie tak samo...
	Card* MoveToCard(City* target);
	Card* MoveEverywhere(City* target/*, Card* usedCard*/);		//zwraca karte do przelozenia na Discard
	Card* BuildStation();
	void DiscoverCure(vector<Card*> usedCards, Disease* toDiscover);
	void TreatCity(Disease* toTreat);
	int ShareKnowledge(Player * targetPlayer, PlayerCard * card);
	int EarnCards(vector<PlayerCard *> cards);			 //zwraca z ilu kart gracz MUSI sie zrzucic
	int EarnCardFrom(Player * sourcePlayer, PlayerCard * card);
	PlayerCard* Discard(PlayerCard* card);	 //wywal z reki, zwroc karte (karta musiala tam byc!!!)
	vector<PlayerCard*> Discard(vector<PlayerCard*> cards);//zaklada, ze karty w ARGUMENCIE sa wybrane z REKI tego gracza !!!
	vector<PlayerCard*> ChooseInColorCards(DiseaseType color);

	Player(PlayerRole hisRole, string nickName, City* startPosition);
	~Player();
};

