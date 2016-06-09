#include "CardDeck.h"

//#include <ctime>
//#include <cstdlib>
#include <fstream> 
#include <algorithm>
#include "DiseaseCard.h"
#include "PlayerCard.h"
#include "SpecialCard.h"
#include "EpidemicCard.h"
#include "enumShort.h"
//#include "enums.h"

#include <iostream>

void CardDeck::PutCard(Card * usedCard)
{
	Deck.push_back(usedCard);
	mediator().moveCard(usedCard, this);
}

void CardDeck::putClearPlayerCards(vector<PlayerCard*> &cardsToAdd)
{
	while (!cardsToAdd.empty())
	{
		Deck.push_back(cardsToAdd.back());
		cardsToAdd.pop_back();
	}
}

bool CardDeck::IsEmpty()
{
	return Deck.empty();
}

vector<Card*> CardDeck::SeeDeck() const
{
	return Deck;
}

int myrandom(int i)  //lokalna randomizujaca...
{ 
	return rand() % i;
}

void CardDeck::shuffle()
{
	random_shuffle(Deck.begin(), Deck.end(), myrandom);
	random_shuffle(Deck.begin(), Deck.end(), myrandom); 
}

void CardDeck::prepare(string fileName/*, int difficulty, int playersCount*/)
{
	fstream File(fileName, ios::in); // |ios::binary
	if (!File.is_open())
		throw "dupa in opening the FILE";

	string Type, colorSTR;						  //Type potem jako NAZWA MIASTA
	DiseaseType color;
	getline(File, Type);
	vector<Card*> toReturn;
	if (Type == "DiseaseCard")
	{
		while (!File.eof())
		{
			getline(File, Type);
			getline(File, colorSTR);
			color = static_cast<DiseaseType> (DiseaseType_SL.indexOf(QString::fromStdString(colorSTR)));
			Deck.push_back(new DiseaseCard(Type, color));
		}
		shuffle();	shuffle();
	}
	else
	{
		if (Type == "PlayerCard")
		{
			while (!File.eof())
			{
				getline(File, Type);
				getline(File, colorSTR);
				color = static_cast<DiseaseType> (DiseaseType_SL.indexOf(QString::fromStdString(colorSTR)));
				Deck.push_back(new PlayerCard(Type, color));
			}
			for (int i = 0; i < 5; i++)
			{
				Deck.push_back(new SpecialCard(static_cast<SpecialCardType>(i)));
			}
			shuffle(); shuffle();
		}
	}
}	

void CardDeck::prepareEpidemies(int difficulty, int playersCount)
{	   
	shuffle();
	difficulty += 4;
	int size, remaining;						//kart jest 48+5+(4|5|6) = 57|58|59
	size = Deck.size() / difficulty;	   // 13	|| 10	|| 8
	remaining = Deck.size() % difficulty;  // 1 	|| 3	|| 5
	vector<CardDeck> Pile;
	Pile.reserve(difficulty);
	CardDeck OneStack;
	for (int i = 0; i < difficulty; i++)
	{
		for (int j = 0; j < size; j++)
		{
			OneStack.Deck.push_back(Deck.back());
			Deck.pop_back();
		}
		if (remaining > 0)
		{
			OneStack.Deck.push_back(Deck.back());
			Deck.pop_back();
			remaining--;
		}
		OneStack.Deck.push_back(new EpidemicCard(i));
		OneStack.shuffle();
		Pile.push_back(OneStack);
		OneStack.Deck.clear();
	}

	for (int i = 0; i < difficulty; i++)
	{
		for (Card* card : Pile[i].Deck)
		{
			Deck.push_back(card);
		}
	}
	for (CardDeck& lilDeck : Pile)
	{
		lilDeck.Deck.clear();
	}
}

Card* CardDeck::takeCard()
{
	Card* card = Deck.back();
	Deck.pop_back();
	return card;
}

Card* CardDeck::TakeBottom()
{
	Card* card = Deck.front();
	Deck.erase(Deck.begin());
	return card;
}

void CardDeck::Rethrow(CardDeck* used)
{
	used->shuffle();
	for (Card* card : used->Deck)
	{
		Deck.push_back(card);
		mediator().moveCard(card, this); // czy tutaj jest to potrzebne?
	}
	used->Deck.clear();
}

void CardDeck::RemoveCard(Card* cardToRemove)
{
	Card* card;
	for (vector<Card*>::iterator it = Deck.begin(); it != Deck.end(); ++it)
	{
		if ((*it) == cardToRemove)
		{
			card = (*it);
			Deck.erase(it);
			delete card;
			return;
		}
	}
}

DeckType CardDeck::GetType() const
{
	return type;
}

CardDeck::CardDeck(DeckType typeNumber) :type(typeNumber)
{
}

CardDeck::CardDeck() : type(DT_PLAYER)
{
}


CardDeck::~CardDeck()
{
	for(Card* card : Deck)
	{
		delete card;
	}
}
