#include "Player.h"

PlayerCard* Player::FindCard(string cardName)
{
	for (PlayerCard* card : hand)
	{
		if (cardName == card->GetName())
		{
			return card;
		}
	}
	return nullptr;
}

void Player::MoveToNeighbour(City* target)
{
	position = target;
	mediator().movePawn(this, target);
}

vector<PlayerCard*> Player::SeeCards()
{
	return hand;
}

PlayerRole Player::GetRole() const
{
	return role;
}

City* Player::GetPosition() const
{
	return position;
}

Card* Player::MoveToCard(City* target)
{
	Card* thisCard;
	position = target;
	mediator().movePawn(this, target);
	for (vector<PlayerCard*>::iterator it = hand.begin(); it != hand.end();++it)
	{
		if ((*it)->GetName() == target->GetName())
		{
			thisCard = (*it);			
			hand.erase(it);	  // vector::erase(Iterator position) :P			
			return thisCard;
		}
	}	
	return nullptr;
}

Card* Player::MoveEverywhere(City* target/*, Card* usedCard*/)
{
	Card* thisCard;
	for (vector<PlayerCard*>::iterator it = hand.begin(); it != hand.end(); ++it)
	{
		if ((*it)->GetName() == position->GetName())
		{
			thisCard = (*it);
			position = target;
			hand.erase(it);	  // vector::erase(Iterator position) :P
			mediator().movePawn(this, target);
			return thisCard;
		}
	}
	return nullptr;
}

Card* Player::BuildStation()
{
	Card* thisCard;
	if (role != ROLE_OPERATIONSEXPERT)
	{
		for (vector<PlayerCard*>::iterator it = hand.begin(); it != hand.end(); ++it)
		{
			if ((*it)->GetName() == position->GetName())
			{
				thisCard = (*it);
				hand.erase(it);	  // vector::erase(Iterator position) :P
				position->BuildResearchStation();
				return thisCard;
			}
		}
		throw exception("No proper card !!!");		 //krytyczny blad
	}
	else
	{
		position->BuildResearchStation();
		return nullptr;							 //wyzej, czy nullptr -> nie przenos karty na discard..
	}
}

void Player::DiscoverCure(vector<Card*> usedCards, Disease* toDiscover)		  //warunek na STACJE - wyzej 	 | na karty - tez wyzej
{
	Card* ptr;
	for (int i = 0; i < usedCards.size(); ++i)
	{
		ptr = usedCards[i];
		for (vector<PlayerCard*>::iterator it = hand.begin(); it != hand.end(); ++it)
		{
			if ((*it) == ptr)
			{
				hand.erase(it);
				break;
			}
		}
	}
	toDiscover->Discover();
}

void Player::TreatCity(Disease* toTreat)	 //sprawdzic konstrukcje ze sposobem wywolania ostatecznie
{
	toTreat->healDisease(position, role);
}

int Player::ShareKnowledge(Player* targetPlayer, PlayerCard* card) //warunki - WYZEJ ++++++--- DODAC !!!
{
	for (vector<PlayerCard*>::iterator it = hand.begin(); it != hand.end(); ++it)
	{
		if ((*it) == card)
		{
			hand.erase(it);
			break;
		}
	}
	vector<PlayerCard*> cards;
	cards.push_back(card);
	int excessCardsCount = targetPlayer->EarnCards(cards);
	return excessCardsCount;
}

int Player::EarnCards(vector<PlayerCard*> cards)
{
	int mustDiscardCount = 0;
	Mediator& m = mediator();
	// TODO	 - zmienic wszedzie na dodawanie kilku (max 2) kart, a potem porzadkowanie EWENTUALNIE - poczatkowe rozdanie kart tez tutaj (mediator w petli)
	for (PlayerCard* card : cards)
	{
		hand.push_back(card);
		m.moveCard(card, this);
	}	 	
	if (hand.size() > 7)
	{
		mustDiscardCount = hand.size() - 7;
		//vector<SpecialCard*> specials;
		//SpecialCard* specialCard;
		//for (PlayerCard* cardInHand : hand)
		//{
		//	specialCard = dynamic_cast<SpecialCard*> (cardInHand);
		//	if (specialCard != nullptr)
		//	{
		//		specials.push_back(specialCard);
		//	}
		//}
		//if (specials.size() > 0)
		//{
		//						  //no idea... ma wybrac, co zrobi, potem trzeba to wykonac...
		//}
		//m.playerMustDiscardCards(hand.size()-7,THISMETHOD(&Player::Discard));
		//throw "zrob porzadek w kartach";
		////zrob porzadek w kartach
	}
	return mustDiscardCount;
}

int Player::EarnCardFrom(Player* sourcePlayer, PlayerCard* card) 
{
	for (vector<PlayerCard*>::iterator it = sourcePlayer->hand.begin(); it != sourcePlayer->hand.end(); ++it)
	{
		if ((*it) == card)
		{
			sourcePlayer->hand.erase(it);
			break;
		}
	}
	vector<PlayerCard*> cards;
	cards.push_back(card);
	int toDiscardCount = this->EarnCards(cards);	
	return toDiscardCount;
}

PlayerCard* Player::Discard(PlayerCard* card)		 
{
	for (vector<PlayerCard*>::iterator it = hand.begin(); it != hand.end(); ++it)
	{
		if ((*it) == card)
		{
			hand.erase(it);
			return card;
		}
	}
	return nullptr;
}

vector<PlayerCard*> Player::Discard(vector<PlayerCard*> cards)			   
{
	for (PlayerCard* singleCard : cards)
	{
		for (vector<PlayerCard*>::iterator it = hand.begin(); it != hand.end(); ++it)
		{
			if (singleCard == (*it))
			{
				hand.erase(it);
				break;
			}
		}
	}
	return cards;
}

vector<PlayerCard*> Player::ChooseInColorCards(DiseaseType color)
{
	vector<PlayerCard*> cards;
	for (PlayerCard* singleCard : hand)
	{
		if (singleCard->GetColor() == color)
		{
			cards.push_back(singleCard);
		}
	}  
	return cards;
}

Player::Player(PlayerRole hisRole, string nickName, City* startPosition) :role(hisRole), nick(nickName), position(startPosition)
{
}


Player::~Player()
{
}
