#pragma once

#include "enums.h"
#include <vector>
using namespace std;
class PlayerCard;
class Card;

class CardDeck
{
	vector<Card*> Deck;	 
	const DeckType type;	
public:
	void PutCard(Card* usedCard);
	bool IsEmpty();
	vector<Card*> SeeDeck() const;		//dla sprawdzania stosu DISCARD
	void shuffle();
	void prepare(string fileName/*,int difficulty, int playersCount*/);
	void prepareEpidemies(int difficulty, int playersCount);
	Card* takeCard();
	Card* TakeBottom();
	void RemoveCard(Card* cardToRemove);
	void Rethrow(CardDeck* used);
	void putClearPlayerCards(vector<PlayerCard*> &cardsToAdd);
	DeckType GetType() const;
	int CardsLeft() const;

	CardDeck(DeckType typeNumber);
	CardDeck();
	~CardDeck();
};

