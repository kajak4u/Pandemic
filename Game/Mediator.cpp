#include "Mediator.h"
#include "CBoard.hpp"
#include "CCity.hpp"
#include "CPawn.hpp"
#include "COverlay.hpp"
#include "CCureMarker.hpp"
#include "CPathMarker.hpp"
#include "Lukasz\City.h"
#include "Lukasz\Player.h"
#include "Lukasz\Board.h"
#include "Lukasz\CardDeck.h"
#include "Lukasz\DiseaseCard.h"
#include "CDeck.hpp"
#include "CCard.hpp"
#include "CPlayer.h"
#include <QDebug>

CardType findType(Card* card)
{
    CardType type = CT_UNKNOWN;
    if (dynamic_cast<DiseaseCard*>(card) != nullptr)
        type = CT_DISEASE;
    else if (dynamic_cast<SpecialCard*>(card) != nullptr)
        type = CT_SPECIAL;
    else if (dynamic_cast<EpidemicCard*>(card) != nullptr)
        type = CT_EPIDEMIC;
    else
        type = CT_PLAYER;
    return type;
}


Mediator::Mediator(): GUI(nullptr), engine(nullptr)
{
}

void Mediator::checkGUI() const
{
    if (GUI == nullptr)
        throw "Cannot handle with NULL GUI";
    if (engine == nullptr)
        throw "Cannot handle with NULL engine";
}

void Mediator::setGUI(CBoard *newGUI)
{
    GUI = newGUI;
}

void Mediator::setEngine(Board *board)
{
    engine = board;
}

void Mediator::init(const std::vector<Player*>& players, const std::vector<City*>& cities, const CardDeck & playerCardDeck, const CardDeck & playerDiscard, const CardDeck & diseaseCardDeck, const CardDeck & diseaseDiscard)
{
    qDebug() << "init start";
    checkGUI();
    for (Player* player : players) {
        CPlayer* newPlayer = new CPlayer(player, GUI->findChild<CCity*>(CCity::createObjectName(QString::fromStdString(player->GetPosition()->GetName()))));
        GUI->addPlayer(newPlayer);
    }
    qDebug() << "connect cities";
    for (City* city : cities)
        dynamic_cast<CCity*>(GUI->findItemByName(QString::fromStdString(city->GetName())))->bindLogic(city);
    qDebug() << "bind decks";
    QVector<QPair<const CardDeck*, CDeck*> > deckPairs;
    deckPairs << qMakePair(&playerCardDeck, GUI->findChild<CDeck*>(CDeck::createObjectName(DT_PLAYER)))
        << qMakePair(&playerDiscard, GUI->findChild<CDeck*>(CDeck::createObjectName(DT_PLAYERDISCARD)))
        << qMakePair(&diseaseCardDeck, GUI->findChild<CDeck*>(CDeck::createObjectName(DT_DISEASE)))
        << qMakePair(&diseaseDiscard, GUI->findChild<CDeck*>(CDeck::createObjectName(DT_DISEASEDISCARD)));
    bool reversed = true;
    for (const QPair<const CardDeck*, CDeck*>& pair : deckPairs) {
        qDebug() << "deck " << pair.second->objectName();
        vector<Card*> cards = pair.first->SeeDeck();
        pair.second->setReversed(reversed);
        reversed = !reversed;
        for (Card* card : cards) {
            pair.second->addCard(findType(card), QString::fromStdString(card->GetName()));
        }
    }
    qDebug() << "init end";

// TODO "Mediator::init - not implemented"
}

void Mediator::setActions(QSet<Decision>)
{
    checkGUI();
// TODO "Mediator::setActions - not implemented"
}

void Mediator::setCurrent(Player *)
{
    checkGUI();
// TODO "Mediator::setCurrent - not implemented"
}

void Mediator::addDiseaseCube(City *infectedCity, DiseaseType color, int count)
{
    checkGUI();
    QString cityName = QString::fromStdString(infectedCity->GetName());
    CCity* cityGUI = dynamic_cast<CCity*>(GUI->findItemByName(cityName));
    if (cityGUI == nullptr)
        throw QString("City ") + cityName + " not found";
    for (int i = 0; i < count; ++i)
        cityGUI->addCube(color);
}

void Mediator::removeCube(City *healedCity, DiseaseType color, int count)
{
    checkGUI();
    QString cityName = QString::fromStdString(healedCity->GetName());
    CCity* cityGUI = dynamic_cast<CCity*>(GUI->findItemByName(cityName));
    if (cityGUI == nullptr)
        throw QString("City ") + cityName + " not found";
    for (int i = 0; i < count; ++i)
        cityGUI->removeCube(color);
}

void Mediator::movePawn(Player *player, City *destination)
{
    checkGUI();
    QString cityName = QString::fromStdString(destination->GetName());
    CCity* cityGUI = dynamic_cast<CCity*>(GUI->findItemByName(cityName));
    if (cityGUI == nullptr)
        throw QString("City ") + cityName + " not found";
    GUI->findPawn(player->GetRole())->moveTo(cityGUI);
}

void Mediator::moveCard(Card *card, CardDeck * dest)
{
    checkGUI();
    CCard* cardGUI = GUI->findChild<CCard*>(CCard::createObjectName(QString::fromStdString(card->GetName()), findType(card)));
    CDeck* deckGUI = GUI->findChild<CDeck*>(CDeck::createObjectName(dest->GetType()));
    if (cardGUI == nullptr)
        throw QString("Card %1 not found in game!").arg(QString::fromStdString(card->GetName()));
    if (deckGUI == nullptr)
        throw QString("Deck %1 not found in game!").arg(DeckType_SL[dest->GetType()]);
    if (cardGUI->isReversed())
        cardGUI->invert();
    cardGUI->setStandardMiddleAnim(deckGUI->getStandardMiddle());
    qDebug() << cardGUI->objectName() << " - moved to " << deckGUI->objectName() << endl;
}

#include <QPropertyAnimation>

void Mediator::moveCard(Card *card, Player * dest)
{
    checkGUI();
    CCard* cardGUI = GUI->findChild<CCard*>(CCard::createObjectName(QString::fromStdString(card->GetName()), findType(card)));
    CPlayer* playerGUI = GUI->findPlayer(dest->GetRole());
    if (cardGUI == nullptr)
        throw QString("Card %1 not found in game!").arg(QString::fromStdString(card->GetName()));
    if(playerGUI==nullptr)
        throw QString("Player %1 not found in game!").arg(PlayerRole_SL[dest->GetRole()]);
    if (cardGUI->isReversed())
        cardGUI->invert();
    QLabel* target = playerGUI->getIco();
    QWidget* par = dynamic_cast<QWidget*>(GUI->parent());
    CPoint boardEndPos = GUI->mapFrom(par, target->mapTo(par, CPoint(target->size()) / 2));
    CPoint boardStartPos = GUI->mapFrom(par, cardGUI->mapTo(par, CPoint(cardGUI->size()) / 2));
    QPointF sizeFactor = { double(target->width()) / cardGUI->width(), double(target->height()) / cardGUI->height() };
    double endFactor = qMin(sizeFactor.x(), sizeFactor.y()) * cardGUI->getZoom();
    cardGUI->setParent(GUI);
    cardGUI->show();
    cardGUI->setStandardMiddle(boardStartPos / cardGUI->getZoom());
    cardGUI->setStandardMiddleAnim(boardEndPos / endFactor);
    //QPropertyAnimation* anim = new QPropertyAnimation(cardGUI, "standardSize");
    //anim->setStartValue(cardGUI->size()/cardGUI->getZoom());
    //anim->setEndValue(target->size()/cardGUI->getZoom());
    QPropertyAnimation* anim = createPropertyAnimation(cardGUI, "zoomFactor", cardGUI->getZoom(), endFactor, 1000, QEasingCurve::InQuad);
    anim->connect(anim, &QPropertyAnimation::stateChanged, cardGUI, &CCard::scaleAnimationChanged);
    anim->connect(anim, &QPropertyAnimation::finished, cardGUI, &CCard::hide);
    GUI->addAnimation(anim);
// TODO "Mediator::moveCard(Card*,Player*) - not implemented"
}

void Mediator::setDiseaseStatus(DiseaseType color, CureStatus status)
{
    checkGUI();
    CCureMarker* diseaseMarker = GUI->findChild<CCureMarker*>("CureMarker_" + CureStatus_SL[color]);
    diseaseMarker->setStatus(status);
}

void Mediator::buildResearchStation(City *destination)
{
    checkGUI();
    QString cityName = QString::fromStdString(destination->GetName());
    CCity* cityGUI = dynamic_cast<CCity*>(GUI->findItemByName(cityName));
    cityGUI->buildResearchStation();
}

void Mediator::increaseOutbreaksMarker(int count)
{
    checkGUI();
    CPathMarker* marker = GUI->findChild<CPathMarker*>("Outbreaks marker");
    marker->moveToNext(count);
}

void Mediator::increaseInfectionsMarker()
{
    checkGUI();
    CPathMarker* marker = GUI->findChild<CPathMarker*>("Infections marker");
    marker->moveToNext();
}

void Mediator::removeResearchStation(City *destination)
{
    checkGUI();
    QString cityName = QString::fromStdString(destination->GetName());
    CCity* cityGUI = dynamic_cast<CCity*>(GUI->findItemByName(cityName));
    cityGUI->removeResearchStation();
}

void Mediator::endGame(GameResult)
{
    // TODO: Mediator::endGame not implemented
}

void Mediator::chooseStationToRemove(std::vector<City*> stations, CALLBACK(Board, void, City *)callback)
{
    checkGUI();
    QSet<CBoardItem*> stationsGUI;
    for (City* city : stations)
        stationsGUI << GUI->findItemByName(QString::fromStdString(city->GetName()));

    GUI->zoomOut();
    COverlay* overlay = GUI->showOverlay();
    overlay->track(stationsGUI);
    overlay->connect(overlay, &COverlay::userMadeChoice, [callback, this](CBoardItem* chosen) {
        if (chosen == nullptr) {
            //u¿ytkownik wybra³ "anuluj"
        }
        else {
            CCity* chosenCity = dynamic_cast<CCity*>(chosen);
            engine->BuildStationIfFull(chosenCity->toLogic());
        }
    });
}

void Mediator::playerMayDiscardCards(int count, CALLBACK(Board, void, QVector<Card*>) callbackIfSuccess)
{
}

void Mediator::playerMustDiscardCards(Player * which, int count, CALLBACK(Board, void, TWOPARAM(std::vector<PlayerCard*>, Player *))callback)
{
    // TODO: Mediator::playerMustDiscardCards not implemented
}

Mediator & mediator()
{
    static Mediator instance;
    return instance;
}
