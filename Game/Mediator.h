#pragma once
#include "enums.h"
class CBoard;
class Board;
class City;
class Card;
class PlayerCard;
class DiseaseCard;
class Player;
class CardDeck;
class CCard;
#include <vector>
#include <QVector>

template<typename _OBJ, typename _MET> struct MethodCallback {
    _OBJ object;
    _MET method;
    MethodCallback(_OBJ o, _MET m) : object(o), method(m) {}
};
template<typename _OBJ, typename _MET> MethodCallback<_OBJ, _MET> makeCallback(_OBJ o, _MET m) {
    MethodCallback<_OBJ, _MET> ret(o, m);
    return ret;
}
//#define THISMETHOD(method) makeCallback(this, method)
//#define CALLBACK(class, retType, args) MethodCallback<class*, retType(class::*)(args)>
//#define TWOPARAM(x, y) x, y
//#define STRING2(x) #x
//#define STRING(x) STRING2(x)
//#define STAMP __FILE__  "(" STRING(__LINE__)"): "
//#define COMMENT(s) message(STAMP s)

class Mediator {
    CBoard* GUI;
    Board* engine; // jednak te� jest potrzebny
    Mediator(); //prywatny konstruktor - celowo, �eby nie da�o si� tworzy� obiekt�w tej klasy poza wyznaczonymi miejscami
    void checkGUI() const;
public:
    void setGUI(CBoard*);
    void setEngine(Board*);
	void init(const std::vector<Player*>& players, const std::vector<City*>& cities, 
			  const CardDeck& playerCardDeck, const CardDeck& playerDiscard, const CardDeck& diseaseCardDeck, const CardDeck& diseaseDiscard);
    void setCurrent(Player*);
    void setHand();
    bool playerUsedCard(CCard*);
    void addDiseaseCube(City*, DiseaseType, int count=1);
    void removeCube(City*, DiseaseType, int count=1);
    void movePawn(Player*, City*);
    void moveCard(Card*, CardDeck* dest);   //przesu� kart� ze stosu na stos albo z r�ki na stos
    void moveCard(Card*, Player* dest); //przesu� kart� ze stosu do r�ki albo mi�dzy graczami
    void setDiseaseStatus(DiseaseType, CureStatus);
    void buildResearchStation(City*);
    void increaseOutbreaksMarker(int count = 1);
    void increaseInfectionsMarker();
    void removeResearchStation(City*); //jak chcemy zbudowa� baz� a nie ma wi�cej kostek, to jak�� trzeba usun��
	void endGame(GameResult);

    void chooseStationToRemove(std::vector<City*>);
    void ShareKnowledge();
    void playerMayDiscardCards(int count);
    void playerMustDiscardCards(Player* which, int count);

    friend Mediator& mediator(); //ta metoda mo�e wywo�a� prywatny konstruktor
};

Mediator& mediator();