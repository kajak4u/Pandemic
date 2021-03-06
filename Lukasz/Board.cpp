#include "Board.h"

#include <fstream>
#include <map>
#include <set>
#include <typeinfo>
#include <qset.h>
#include <ctime>
#include <cstdlib>
#include <algorithm>

//PRIVATE:
int Board::outbreaksMarker = 0;
vector<int> Board::infectionRateMarker;
GameResult Board::gameStatus = IN_PROGRESS;

void Board::MedicIncoming(Player* moved)
{
	for (Disease& dis : diseases)
	{
		if (dis.Status() == DISCOVERED)
		{
			moved->TreatCity(&dis);
		}
	}
}

void Board::DiscardSpecialCard(string cardName)
{
	for (Player* play : players)
	{
		PlayerCard* card = play->FindCard(cardName);
		if (card != nullptr)
		{
			play->Discard(card);
			playerDiscarded.PutCard(card);
			break;
		}
	}
}

void Board::Infect(Card* card, int cubesCount)
{
	diseasesDiscarded.PutCard(card);
	Disease* disease = FindDisease(card->GetColor());
	disease->diseaseAppearsIn(FindCity(card->GetName()), cubesCount);
	mediator().setCurrentStatus(FindDisease(BLUE)->MarkersLeft(), FindDisease(YELLOW)->MarkersLeft(), 
								FindDisease(BLACK)->MarkersLeft(), FindDisease(RED)->MarkersLeft(), 
								stationsLeft, playerNew.CardsLeft(), outbreaksMarker, GetInfectionRateMarker());
}

void Board::PlayTheInfection(bool isSkipped)
{
	if (!isSkipped)
	{
		Card* card;
		for (int i = 0; i < infectionRateMarker[infectionRateMarker[0]]; ++i)
		{
			card = diseasesNew.takeCard();
			Infect(card, 1);
		}
	}
	wasInfection = true;
}
//PUBLIC:
Player* Board::GetCurrentPlayer() const
{
	return currentPlayer;
}

int Board::GetMovesLeft() const
{
	return movesLeft;
}

vector<Card*> Board::SeeDeck(DeckType number) const
{
	switch (number)
	{
	case DT_PLAYERDISCARD:
		return playerDiscarded.SeeDeck();
	case DT_DISEASEDISCARD:
		return diseasesDiscarded.SeeDeck();
	default:  {
		vector<Card*> toReturn;
		return toReturn; }
	}
}

City* Board::FindCity(string name) const
{
	for (City* city : Cities)
	{
		if (city->GetName() == name)
		{
			return city;
		}
	}
	return nullptr;
}

vector<City*> Board::GetStations() const
{
	return(Stations);
}

void Board::DiscoverCure()
{
	if (currentPlayer->GetRole() == ROLE_SCIENTIST)
	{
        mediator().playerMayDiscardCards(4,cardsInColor);
	}
	else
	{
        mediator().playerMayDiscardCards(5,cardsInColor);
	}
}

bool IsEradicated(Disease& disease)	//lokalna funkcja dla ALL_OF
{
	return (disease.Status() == TREATED) || (disease.Status() == DISCOVERED);
}

void Board::DiscoverCure_FinalStep(QVector<Card*> cardsRemoved)
{ 
	vector<Card*> cards = cardsRemoved.toStdVector();
	Disease* dis = FindDisease(cardsInColor[0]->GetColor());//wszystkie karty maja ten sam kolor - wczesniej.. wiec wez pierwsza z brzegu i sprawdz
	currentPlayer->DiscoverCure(cards, dis);
	--movesLeft;
	for (Player* play : players)
	{
		if (play->GetRole() == ROLE_MEDIC)
		{
			MedicIncoming(play);
		}
	}
	for (Card* card : cards)
	{
		playerDiscarded.PutCard(card);
	}
	if (all_of(diseases.begin(), diseases.end(), IsEradicated))
	{
		WinOrLoose(WON); //zwyciestwo
	}
}

void Board::UseSpecial()
{
	ChooseSpecial();
}

GameResult Board::Pass()
{
	switch (gameStatus)
	{
		case IN_PROGRESS:
		{
            movesLeft = 0;
			Card* card;
			EpidemicCard* eCard;
			Mediator& m = mediator();
			vector<PlayerCard*> cardsForPlayer;
			for (int j = 0; j < 2; ++j) //nie rozbijam ciagniecia kart na 2 pojedyncze rozdzielane uzyciem specjalnej
			{
				if (playerNew.IsEmpty())
				{
					WinOrLoose(LOST_CARDS);
				}
				else
				{
					card = playerNew.takeCard();
					eCard = dynamic_cast<EpidemicCard*>(card);
					if (eCard != nullptr)
					{
						Card* dCard = diseasesNew.TakeBottom();
						Infect(dCard, 3);							//na podstawie karty zajmuje sie calym procesem infekowania
						diseasesNew.Rethrow(&diseasesDiscarded);
						InfectionIncrease();
						playerDiscarded.PutCard(card);
						//m.moveCard(card, &(this->playerDiscarded));		//czy tak adres bedzie przekazany? czyli uzyskam pointer?
					}
					else
					{
						cardsForPlayer.push_back((PlayerCard*)card);
					}
				}
			}
			if (gameStatus == IN_PROGRESS)
			{
				int toDiscardCount = currentPlayer->EarnCards(cardsForPlayer);
				if (toDiscardCount > 0)
				{
					m.playerMustDiscardCards(currentPlayer, toDiscardCount);
				}
				else
				{
					INFECTION_TIME();
				}
			}
		}
	}	
	return gameStatus;
}

Disease* Board::FindDisease(DiseaseType ID)
{
	for (Disease &dis : diseases)
	{
		if (dis.getID() == ID)
		{
			return &dis;
		}
	}
	return nullptr;
}

void Board::Outbreak()
{
	if (outbreaksMarker < 8)   //potem juz pomijaj, bo gra moze tylko miec na koniec jeszcze serie wybuchow
	{
		++outbreaksMarker;
		mediator().increaseOutbreaksMarker();
		if (outbreaksMarker == 8)
		{
			WinOrLoose(LOST_OUTBREAKS);
		}
	}
}

void Board::InfectionIncrease()
{
	if (infectionRateMarker[0] < infectionRateMarker.size()-1)
	{
		/*cout << infectionRateMarker[0] << endl;						//DIAGNOSTYCZNIE
		cout << "number: " << infectionRateMarker[infectionRateMarker[0]] << endl;*/		//odczyt wlasciwej wartosci - sposob
		infectionRateMarker[0]++;
		mediator().increaseInfectionsMarker();
	}
}

int Board::GetInfectionRateMarker()
{
	return infectionRateMarker[infectionRateMarker[0]];
}

void Board::NewTurn()
{
	if (gameStatus == IN_PROGRESS && wasInfection)
	{
		Mediator& m = mediator();
		wasInfection = false; //by nie wejsc tu znowu przed kolejna infekcja..
		++currentPlayerNumber;
		currentPlayer = players[currentPlayerNumber%players.size()];
		m.setCurrent(currentPlayer);
		movesLeft = 4;
		//int cubesBlue = FindDisease(BLUE)->MarkersLeft(),
		//	cubesRed = FindDisease(RED)->MarkersLeft(),
		//	cubesYellow = FindDisease(YELLOW)->MarkersLeft(),
		//	cubesBlack = FindDisease(BLACK)->MarkersLeft();
		////m.setCurrentStatus(cubesBlue,cubesYellow,cubesBlack,cubesRed,stationsLeft,playerNew.CardsLeft(), outbreaksMarker, GetInfectionRateMarker());
	}
}

vector<Decision> Board::IsAbleTo()
{
	int cubesBlue = FindDisease(BLUE)->MarkersLeft(),
		cubesRed = FindDisease(RED)->MarkersLeft(),
		cubesYellow = FindDisease(YELLOW)->MarkersLeft(),
		cubesBlack = FindDisease(BLACK)->MarkersLeft();
	mediator().setCurrentStatus(cubesBlue, cubesYellow, cubesBlack, cubesRed, stationsLeft, playerNew.CardsLeft(), outbreaksMarker, GetInfectionRateMarker());
	vector<Decision> decisionsAvailable;
	if (gameStatus == IN_PROGRESS)
	{
		if (movesLeft > 0)
		{
			currentCity = currentPlayer->GetPosition();
			cityCard = currentPlayer->FindCard(currentCity->GetName());
			decisionsAvailable.push_back(DEC_MOVE_SHORT);								//MOVE_SHORT
			if (currentPlayer->GetRole() == ROLE_DISPATCHER)							//MOVE_ANOTHER
			{
				decisionsAvailable.push_back(DEC_MOVE_ANOTHER);
			}
			for (PlayerCard* card : currentPlayer->SeeCards())							//MOVE_TO_CARD
			{
				if (card->GetName() != currentCity->GetName())
				{
					decisionsAvailable.push_back(DEC_MOVE_TO_CARD);
					break;
				}
			}
			if (cityCard != nullptr)													//MOVE_ENYWHERE
			{
				decisionsAvailable.push_back(DEC_MOVE_EVERYWHERE);
			}
			if (!(currentCity->IsStation()) && (currentPlayer->GetRole() == ROLE_OPERATIONSEXPERT || cityCard != nullptr))
			{
				decisionsAvailable.push_back(DEC_BUILD_STATION);						//BUILD_STATION
			}
			for (Disease dis : diseases)												//TREAT
			{
				if (currentCity->DiseaseCounter(dis.getID()) > 0)
				{
					decisionsAvailable.push_back(DEC_TREAT);
					break;
				}
			}
			supPlayers.clear();
			playersToGetCardFrom.clear();
			isResearcher = false;
			anotherPlayersCityCard = nullptr;
			for (Player* play : players)				//wybierz graczy w miescie BEZ obecnego gracza
			{
				if (play != currentPlayer && play->GetPosition() == currentCity)
				{
					supPlayers.push_back(play);
					if (play->GetRole() == ROLE_RESEARCHER)		  //czy dodawany gracz bedzie mogl dawac dowolne karty
					{
						for (PlayerCard* oneCard : play->SeeCards())//czy karty na rece gracza to nie tylko specjalne (tych nie oddajemy)
						{
							SpecialCard	* areTheyNotOnlySpecial = dynamic_cast<SpecialCard*>(oneCard);
							if (areTheyNotOnlySpecial == nullptr)//jak choc jedna karta NIE jest specjalna
							{
								isResearcher = true;//wtedy ma sens informowanie, ze jest Researcher	- inaczej nic jego rola nie da
								playersToGetCardFrom.push_back(play);//od niego mozna wziac karte
								break;//..i nie szukaj dalej w jego kartach
							}
						}
					} 
					else if (cityCard == nullptr && anotherPlayersCityCard == nullptr) //jak obecny nie ma miasta, to czy sprawdzany (NIE-researcher) jej nie posiada
					{																 //ale tez CZY WCZESNIEJSZY JEJ NIE MIAL
						anotherPlayersCityCard = play->FindCard(currentCity->GetName());
						if (anotherPlayersCityCard != nullptr)//jesli posiada..
						{
							playersToGetCardFrom.push_back(play);//..dodaj go do listy potencjalnych dawcow organow
						}
					}
				}
			}
			if (!supPlayers.empty())
			{
				if (currentPlayer->GetRole() == ROLE_RESEARCHER)
				{
					for (PlayerCard* oneCard : currentPlayer->SeeCards())//czy karty na rece gracza to nie tylko specjalne (tych nie oddajemy)
					{
						SpecialCard	* areTheyNotOnlySpecial = dynamic_cast<SpecialCard*>(oneCard);
						if (areTheyNotOnlySpecial == nullptr)//jak choc jedna karta NIE jest specjalna
						{
							decisionsAvailable.push_back(DEC_GIVE_CARD);//dodaj opcje
							break;//..i nie szukaj dalej
						}
					}
				}
				else if (cityCard != nullptr)
				{
					decisionsAvailable.push_back(DEC_GIVE_CARD);
				}
				if (isResearcher || anotherPlayersCityCard != nullptr)				   //GAIN_CARD
				{
					decisionsAvailable.push_back(DEC_GAIN_CARD);
				}
			}
			if (currentCity->IsStation())											   //DISCOVER_CURE
			{
				for (Disease dis : diseases)
				{
					cardsInColor.clear();
					for (PlayerCard* card : currentPlayer->SeeCards())
					{
						if (card->GetColor() == dis.getID())
						{
							cardsInColor.push_back(card);
						}
					}
					if (cardsInColor.size() >= 5)
					{
						decisionsAvailable.push_back(DEC_DISCOVER_CURE);
						break;
					}
					else
					{
						if (currentPlayer->GetRole() == ROLE_SCIENTIST && cardsInColor.size() >= 4)
						{
							decisionsAvailable.push_back(DEC_DISCOVER_CURE);
							break;
						}
					}
				}
			}
		}
		for (Player* play : players)
		{
			for (PlayerCard* card : play->SeeCards())								//USE_SPECIAL
			{
				checker = dynamic_cast<SpecialCard*>(card);
				if (checker != nullptr)
				{
					decisionsAvailable.push_back(DEC_USE_SPECIAL);
					break;
				}
			}
			if (checker != nullptr)
			{
				break;
			}
		}
		decisionsAvailable.push_back(DEC_PASS);	//PASS
	}		
	if ((!supPlayers.empty()) && none_of(decisionsAvailable.begin(), decisionsAvailable.end(), [](Decision decided){return decided == DEC_GIVE_CARD; }))
	{
		supPlayers.clear();
	}
	return decisionsAvailable;
}

vector<Player*> Board::ChoosePlayer() const
{
	return players;
}

vector<Player*> Board::ChoosePlayerToShareKnowledge() const
{		 
	if (!supPlayers.empty())
		return supPlayers; // je�li mo�e da� kart, to ka�demu, wi�c give + gain == give
	return playersToGetCardFrom; //a je�li give=={} to give + gain == gain
}

//QSet<City*> Board::SeeFREECitiesAsDispatcher(Player* toMove)
//{
//	QSet<City*> toReturn = ChooseMoveShort(toMove);
//	for (Player* play : players)
//	{
//		if (play != toMove)
//		{
//			toReturn.insert(play->GetPosition());
//		}
//	}
//	return toReturn;
//}

QSet<City*> Board::ChooseMoveShort(Player* toMove)
{
	QSet<City*> toSend;
	for (City* city : toMove->GetPosition()->Neighbours())
	{
		toSend.insert(city);
	}
	if (toMove->GetPosition()->IsStation())
	{
		for (City* city : Stations)
		{
			if (city != currentCity)
			{
				toSend.insert(city);
			}
		}
	}
	if (currentPlayer->GetRole() == ROLE_DISPATCHER)
	{
		for (Player* play : players)
		{
			if (play != toMove && play->GetPosition() != toMove->GetPosition())
			{
				toSend.insert(play->GetPosition());
			}
		}	  		
	}
	return toSend;
}

QSet<City*> Board::ChooseMoveEverywhere(Player * toMove)
{
	QSet<City*> toReturn;
	City* current = toMove->GetPosition();
	for (City* city : Cities)
	{
		if (city != current)
		{
			toReturn.insert(city);
		}
	}
	return toReturn;
}

vector<DiseaseType> Board::ChooseDiseaseToTreat()
{
	vector<DiseaseType> toChoose;
	for(Disease& dis:diseases)
	{
		if (currentCity->DiseaseCounter(dis.getID()) > 0)
		{
			toChoose.push_back(dis.getID());
		}
	}
	return toChoose;
}

vector<SpecialCard*> Board::ChooseSpecial()
{
	vector<SpecialCard*> specialsToUse;
	for (Player* play : players)
	{
		for (PlayerCard* card : play->SeeCards())
		{
			if (card->GetColor() == UNKNOWN) //bo specjalne maja byc bez koloru	|| uwaga na mozliwy blad - Epidemic na rece !!!
			{
				SpecialCard* special = dynamic_cast<SpecialCard*>(card);
				if (special != nullptr)
				{
					specialsToUse.push_back(special);
				}
				else
				{
					throw string("niby karta jest, ale nie ma !?!?!");
				}				
			}
		}
	}
	return specialsToUse;
}

void Board::DiscardToLimit(vector<PlayerCard*> toRemove, Player* Braian)
{
	Braian->Discard(toRemove);
	for (PlayerCard* card : toRemove)
	{
		playerDiscarded.PutCard(card);
	}
	if (movesLeft == 0)
	{
		INFECTION_TIME();
	}		
}

void Board::MoveToAnotherPlayer(City* target, Player* toMove)
{
	MoveShort(target, toMove);
}

void Board::MoveShort(City * target, Player * toMove) //PAMIETAJ O STACJACH !!!
{
	toMove->MoveToNeighbour(target);//zawiera wewnatrz przeniesienie na ekranie
	if (toMove->GetRole() == ROLE_MEDIC)
	{
		MedicIncoming(toMove);
	}
	--movesLeft;
}

void Board::MoveShort(City * target)
{
	MoveShort(target, currentPlayer);
}

void Board::MoveToCard(PlayerCard * card, Player * toMove)
{
	Card* takenCard = toMove->MoveToCard(FindCity(card->GetName()));//zawiera przemieszczenie pionka
	if (takenCard == nullptr) //jak nie karta ruszanego gracza
	{
		takenCard = currentPlayer->Discard(card);//to karta DISPATCHERA
	}
	if (takenCard == nullptr)
	{
		throw string("zgubiona karta !!!"); //krytyczny blad
	}
	playerDiscarded.PutCard(takenCard);
	if (toMove->GetRole() == ROLE_MEDIC)
	{
		MedicIncoming(toMove);
	}
	--movesLeft;
}

void Board::MoveToCard(PlayerCard * card)
{
	MoveToCard(card, currentPlayer);
}

void Board::MoveEverywhere(City * target, Player * toMove)
{
	Card* used = toMove->MoveEverywhere(target);
	if (used == nullptr) //jak nie karta ruszanego gracza
	{
		for (PlayerCard* card : currentPlayer->SeeCards())
		{
			if (card->GetName() == target->GetName())
			{
				used = currentPlayer->Discard(card);//to karta DISPATCHERA
				break;
			}
		}
	}
	if (used == nullptr)
	{
		throw string("zgubiona karta !!!"); //krytyczny blad
	}
	playerDiscarded.PutCard(used);
	if (toMove->GetRole() == ROLE_MEDIC)
	{
		MedicIncoming(toMove);
	}
	--movesLeft;
}

void Board::MoveEverywhere(City * target)
{
	MoveEverywhere(target, currentPlayer);
}

void Board::BuildStation()
{
	Card* usedCard = currentPlayer->BuildStation();
	if (usedCard!=nullptr)
	{
		playerDiscarded.PutCard(usedCard);
	}
	if (stationsLeft > 0)
	{
		--stationsLeft;
		Stations.push_back(currentCity);
		--movesLeft;
		mediator().buildResearchStation(currentCity);
	}
	else
	{
        mediator().chooseStationToRemove(Stations);
	}		
}

void Board::BuildStationIfFull(City* c)
{																			//>>>>>>> TUTAJ <<<<<<<<<
    for (vector<City*>::iterator it = Stations.begin(); it != Stations.end(); ++it)
    {
        if ((*it) == c)
        {
            Stations.erase(it);
            c->RemoveResearchStation();
			mediator().removeResearchStation(c);
            break;
        }
    } 
	Stations.push_back(currentCity);
	mediator().buildResearchStation(currentCity);
	--movesLeft;
}

void Board::Treat(DiseaseType disType)
{
	Disease* dis = FindDisease(disType);
	dis->healDisease(currentCity, currentPlayer->GetRole());
	--movesLeft;
}

vector<PlayerCard*> Board::ChooseCardToGive(Player* target)
{
	vector<PlayerCard*> toReturn;
	if (currentPlayer->GetRole() == ROLE_RESEARCHER)
	{
		for (PlayerCard* card : currentPlayer->SeeCards())
		{
			if (card->GetColor() != UNKNOWN)
			{
				toReturn.push_back(card);
			}
		}
	}
	else
	{
        PlayerCard* card = currentPlayer->FindCard(currentCity->GetName());
        if (card != nullptr)
            toReturn.push_back(card);
	}
	return toReturn;
}

vector<PlayerCard*> Board::ChooseCardToGain(Player* source)		
{	 
	vector<PlayerCard*> toReturn;
	if (source->GetRole() == ROLE_RESEARCHER)
	{
		for (PlayerCard* card : source->SeeCards())
		{
			if (card->GetColor() != UNKNOWN)
			{
				toReturn.push_back(card);
			}
		}
	}
	else //obsluga tutaj dla pojedynczej karty
	{
        PlayerCard* card = source->FindCard(currentCity->GetName());
        if (card != nullptr)
            toReturn.push_back(card);
	} 
	return toReturn;
}

void Board::GiveCard(Player* target, PlayerCard* card)	  //niech GUI pobiera z planszy (Board) liste dostepnych graczy - wybiera gracza || kart
{
	int toDiscardCount = currentPlayer->ShareKnowledge(target, card);
	--movesLeft;
	if (toDiscardCount > 0)
	{
        mediator().playerMustDiscardCards(target, toDiscardCount);
	}
}

void Board::GainCard(Player* source, PlayerCard* card)
{			
	--movesLeft;
	int excessToDiscard	= currentPlayer->EarnCardFrom(source, card);	//wewnatrz jest przekazanie karty na ekranie
	if (excessToDiscard > 0)
	{
        mediator().playerMustDiscardCards(currentPlayer, excessToDiscard);
	}
}

void Board::QuietNight()
{					  	
	SpecialCardType  number = SC_ONE_QUIET_NIGHT;
	DiscardSpecialCard(SpecialCardType_SL[number].toStdString());
	skipInfecting = true;
}
vector<Card*> Board::Forecast()
{
	SpecialCardType  number = SC_FORECAST;
	DiscardSpecialCard(SpecialCardType_SL[number].toStdString());
	vector<Card*> topCards;
	for (int i = 0; i < 6; ++i)
	{
		topCards.push_back(diseasesNew.takeCard());
	}
	return topCards;
/*
	mediator().seeTopDiseaseDeck(6);*/
}

void Board::Forecast(vector<Card*> returned)
{
	for (int i = 0; i < returned.size(); ++i)
	{
		diseasesNew.PutCard(returned[i]);
	}	 
}

vector<Card*> Board::ResilientPopulation()
{
	return vector<Card*>(diseasesDiscarded.SeeDeck());
}

void Board::ResilientPopulation(Card* toRemove)
{
	SpecialCardType number = SC_RESILIENT_POPULATION;
	DiscardSpecialCard(SpecialCardType_SL[number].toStdString());
	diseasesDiscarded.RemoveCard(toRemove);
}

void Board::Airlift(Player* toMove, City* target)
{
	SpecialCardType  number = SC_AIRLIFT;
	DiscardSpecialCard(SpecialCardType_SL[number].toStdString());
	toMove->MoveToNeighbour(target);   //bo tylko ta nie usunie karty z reki
	MedicIncoming(toMove);
}
QSet<City*> Board::ChooseCitiesToBuildStation()
{
	QSet<City*> toReturn;
	for (City* c : Cities)
	{
		if (!c->IsStation())
		{
			toReturn.insert(c);
		}
	}	
	return toReturn;
}
void Board::GovernmentGrant(City* toBuild)
{
	SpecialCardType  number = SC_GOVERNMENT_GRANT;
	DiscardSpecialCard(SpecialCardType_SL[number].toStdString());
	toBuild->BuildResearchStation();
	Stations.push_back(toBuild);
	mediator().buildResearchStation(toBuild);
	if (stationsLeft > 0)
	{
		--stationsLeft;
	}
	else
	{
        mediator().chooseStationToRemove(Stations);
	}
}

void Board::INFECTION_TIME()
{
	PlayTheInfection(skipInfecting); //ROZSZERZ CHOROBY
	skipInfecting = false;
	NewTurn();	
}

//KONSTRUKCJA:
Board::Board(Difficulty difficulty, QVector<QPair<QString, PlayerRole>> players)
	: diseasesNew(DT_DISEASE), diseasesDiscarded(DT_DISEASEDISCARD), playerNew(DT_PLAYER), playerDiscarded(DT_PLAYERDISCARD)
{
	mediator().setEngine(this); //cokolwiek to robi...
	diseases.push_back(Disease("BLUE"));
	diseases.push_back(Disease("YELLOW"));
	diseases.push_back(Disease("BLACK"));
	diseases.push_back(Disease("RED"));
	outbreaksMarker = 0;
	gameStatus = IN_PROGRESS;
	skipInfecting = false;
	infectionRateMarker = { 1, 2, 2, 2, 3, 3, 4, 4 }; //pierwszy wskazuje, ktory element obecnie brac !!!

	prepareCityList();

	City* cityPtr = FindCity("Atlanta");
	cityPtr->BuildResearchStation();
	mediator().buildResearchStation(cityPtr);
	Stations.push_back(cityPtr);
	stationsLeft = 5;
	srand(time(NULL));
	for (QPair<QString, PlayerRole> onePlayer : players)
	{
		if (onePlayer.second == ROLE_RANDOM)
		{
			PlayerRole role;
			bool isUnique = true;
			do
			{
				role = static_cast<PlayerRole>((rand() % 5) + 1);	   //<0;4> + 1
				isUnique = true;
				for (Player* play : this->players)
				{
					if (play->GetRole() == role)
					{
						isUnique = false;
						break;
					}
				}
				for (QPair<QString, PlayerRole> one : players)
				{	
					if (role == one.second)
					{
						isUnique = false;
						break;
					}
				}
			} while (!isUnique);
			onePlayer.second = role;
		}
		this->players.push_back(new Player(onePlayer.second, onePlayer.first.toStdString(), cityPtr));
	}
	int playersCount = this->players.size();
	diseasesNew.prepare("DiseaseDeck.txt");
	playerNew.prepare("PlayerDeck.txt");
	vector<PlayerCard*> forPlayers;
	for (int j = 0; j < playersCount; ++j)
	{
		for (int i = 9 / playersCount; i > 0; --i)
		{
			forPlayers.push_back((PlayerCard*)playerNew.takeCard());
		}
	}
	playerNew.prepareEpidemies((int)difficulty, playersCount);
	playerNew.putClearPlayerCards(forPlayers);
	mediator().init(this->players, Cities, playerNew, playerDiscarded, diseasesNew, diseasesDiscarded);
	for (Player* play : this->players)
	{
		for (int i = 9 / playersCount; i > 0; --i)
		{
			forPlayers.push_back((PlayerCard*)playerNew.takeCard());
		}
		play->EarnCards(forPlayers);
		forPlayers.clear();
	}
	PrepareDiseases();
	currentPlayerNumber = -1;		//bo zwiekszane w NewTurn()
	//currentPlayer = this->players[currentPlayerNumber];
	wasInfection = true;
	NewTurn();
}

void Board::prepareCityList()
{
	fstream File("Cities.txt", ios::in); // |ios::binary
	if (!File.is_open())
		throw "dupa in opening the FILE cities";

	int cityCounter, neighbourCounter;
	string ccityControl, cityName, neighbours, cityColor;
	QString colour;
	map<City*, vector<string>> citiesWithNeighbourhood;

	File >> cityCounter;
	getline(File, ccityControl);
	for (int i = 0; i < cityCounter; i++)
	{
		getline(File, ccityControl);
		if (ccityControl == "CCity")
		{
            vector<string> neighboursForCities;
			getline(File, cityName);
			getline(File, cityColor);
			File >> neighbourCounter;
			getline(File, neighbours);	//przesun do nowej linii w pliku
			for (int i = 0; i < neighbourCounter; ++i)
			{
				getline(File, neighbours);
				neighboursForCities.push_back(neighbours);
			}
			colour = QString::fromStdString(cityColor);
			citiesWithNeighbourhood[new City(cityName, (DiseaseType)DiseaseType_SL.indexOf(colour))] = neighboursForCities;
		}
	}
	Cities = City::AddNeighbours(citiesWithNeighbourhood);
}

void Board::PrepareDiseases()
{
	for (int j = 3; j > 0; --j)
	{
		for (int i = 0; i < 3; ++i)
		{
			Infect(diseasesNew.takeCard(), j);
		}
	}
}

Board::~Board()
{
	for (City* city : Cities)
	{
		delete city;
	}
	for (Player* player : players)
	{
		delete player;
	}
}

GameResult Board::GameStatus() const
{
	return gameStatus;
}

void Board::WinOrLoose(GameResult res)
{
	gameStatus = res;
	mediator().endGame(gameStatus);
}