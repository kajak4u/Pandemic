#pragma once
#include <vector>
#include "Player.h"
#include "CardDeck.h"
#include "Disease.h"
#include "City.h"
#include "EpidemicCard.h" //w tym Card.h oraz PlayerCard.h
#include "SpecialCard.h"

#include <qstring.h>
#include <qvector.h>
#include <qpair.h>
#include "enumShort.h"
#include <stdexcept> //wyjatki
#include "Mediator.h"

using namespace std;

class Board
{
	vector<Player*> players;
	CardDeck diseasesNew;
	CardDeck diseasesDiscarded;
	CardDeck playerNew;
	CardDeck playerDiscarded;
	vector<Disease> diseases; //Red, Blue, Yellow, Black;  //SPRAWDZIC !!!
	vector<City*> Cities; 
	int currentPlayerNumber;
	int movesLeft;
	GameResult gameStatus;
	int stationsLeft;
	bool skipInfecting;
	static int outbreaksMarker; //ten od przegrywania
	static vector<int> infectionRateMarker;
	vector<City*> Stations;
//deklaracje na etapie SPRAWDZANIA MOZLIWYCH RUCHOW - zachowane na pozniej
	bool wasInfection;
	Player* currentPlayer;
	City* currentCity;
	PlayerCard* cityCard;
	vector<Player*> supPlayers;	  //gracze w tym samym miescie, co obecny (BEZ NIEGO w tym vectorze)
	bool isResearcher;
	PlayerCard* anotherPlayersCityCard;
	vector<PlayerCard*> cardsInColor;
	SpecialCard* checker;			//sprawdzajaca, czy gracz ma karte specjalna i przechowujaca ja..

	void Infect(Card* card, int cubesCount);
	void PlayTheInfection(bool isSkipped);
	void DiscardSpecialCard(string cardName);

	void prepareCityList();
	void PrepareDiseases();
	void MedicIncoming(Player* moved);
	void INFECTION_TIME();
	void NewTurn();
public:	
//w turze znamy: OBECNY GRACZ, JEGO POLOZENIE, INNI GRACZE W TYM MIESCIE, STACJE, CHOROBY, karty do wynalezienia choroby
	vector<Decision> IsAbleTo();
	vector<Player*> ChoosePlayer() const; //praktycznie getter
	vector<Player*> ChoosePlayerInCity();  //see players in the same city as current player
	QSet<City*> SeeFREECitiesAsDispatcher(Player* toMove);
	QSet<City*> ChooseMoveShort(Player* toMove);
	QSet<City*> ChooseMoveEverywhere(Player* toMove);
	vector<DiseaseType> ChooseDiseaseToTreat();
	vector<PlayerCard*> ChooseCardToGive(Player* target);
	vector<PlayerCard*> ChooseCardToGain(Player* source); //od kogo brac - wczesniej pobrane w GUI
	vector<SpecialCard*> ChooseSpecial();
	void DiscardToLimit(vector<PlayerCard*> toRemove, Player* Braian);
	
	void MoveToAnotherPlayer(City* target, Player* toMove);
	void MoveShort(City* target, Player* toMove);
	void MoveShort(City* target);
	void MoveToCard(PlayerCard* card, Player* toMove);
	void MoveToCard(PlayerCard* card);
	void MoveEverywhere(City* target, Player* toMove);
	void MoveEverywhere(City* target);
	void BuildStation();
	void BuildStationIfFull(City* toRemove);
	void Treat(DiseaseType disType);
	void GiveCard(Player* target, PlayerCard* card);
	void GainCard(Player* source, PlayerCard* card);
	void DiscoverCure();
	void DiscoverCure_FinalStep(QVector<Card*> cardsRemoved);
	void UseSpecial();		 //wywalic (bo tylko przekierowaniem)  ?
	GameResult Pass();

	void QuietNight();
	vector<Card*> Forecast();				//part 1
	void Forecast(vector<Card*> returned);	//part 2   //PIERWSZA W vectorze BEDZIE SZLA NA DOL !!!!
	vector<Card*> ResilientPopulation();//pytanie do gracza
	void ResilientPopulation(Card* toRemove);//akcja wlasciwa
	void Airlift(Player* toMove, City* target);
	void GovernmentGrant(City* toBuild);
/*
	enum SpecialCardType
	{
		SC_ONE_QUIET_NIGHT,
		SC_FORECAST,
		SC_RESILIENT_POPULATION,
		SC_AIRLIFT,
		SC_GOVERNMENT_GRANT
	};*/

	Disease* FindDisease(DiseaseType ID);
	City* FindCity(string name) const;
	vector<City*> GetStations() const;
	GameResult GameStatus() const;
	Player* GetCurrentPlayer() const;

	static void Outbreak(); //throw out_of_range() -> catch w PASS() //wywolane tez wewnatrz
	static void InfectionIncrease();
	static int GetInfectionRateMarker();

	Board(Difficulty difficulty, QVector<QPair<QString,PlayerRole>> players);		   
	~Board();
};