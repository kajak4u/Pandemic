#include "Mediator.h"
#include "CBoard.hpp"
#include "CCity.hpp"
#include "CCard.hpp"
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
#include <QParallelAnimationGroup>
#include <QMessageBox>

#define FIND(CLASS, ...) findChild<CLASS*>(CLASS::createObjectName(__VA_ARGS__))
#define QSTR(s) QString::fromStdString(s)

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
        CPlayer* newPlayer = new CPlayer(player, GUI->FIND(CCity, QSTR(player->GetPosition()->GetName())));
        GUI->addPlayer(newPlayer);
    }
    qDebug() << "connect cities";
    for (City* city : cities)
        GUI->FIND(CCity, QSTR(city->GetName()))->bindLogic(city);
    qDebug() << "bind decks";
    QVector<QPair<const CardDeck*, CDeck*> > deckPairs;
    deckPairs << qMakePair(&playerCardDeck, GUI->FIND(CDeck, DT_PLAYER))
        << qMakePair(&playerDiscard, GUI->FIND(CDeck, DT_PLAYERDISCARD))
        << qMakePair(&diseaseCardDeck, GUI->FIND(CDeck, DT_DISEASE))
        << qMakePair(&diseaseDiscard, GUI->FIND(CDeck, DT_DISEASEDISCARD));
    bool reversed = true;
    for (const QPair<const CardDeck*, CDeck*>& pair : deckPairs) {
        qDebug() << "deck " << pair.second->objectName();
        vector<Card*> cards = pair.first->SeeDeck();
        pair.second->setReversed(reversed);
        reversed = !reversed;
        for (Card* card : cards) {
            pair.second->addCard(findType(card), QSTR(card->GetName()), card);
        }
    }
    qDebug() << "init end";
}

void Mediator::setCurrentStatus(int cubesBlue, int cubesYellow, int cubesBlack, int cubesRed, int stations, int playerCards, int outbreaks, int infectionsRate)
{
    checkGUI();
    emit GUI->setCurrentStatus(cubesBlue, cubesYellow, cubesBlack, cubesRed, stations, playerCards, outbreaks, infectionsRate);
}

void Mediator::setCurrent(Player *current)
{
    checkGUI();
    qDebug() << "set current to " << PlayerRole_SL[current->GetRole()];
    QMetaObject::Connection *conn = new QMetaObject::Connection;
    *conn = GUI->connect(GUI, &CBoard::animationFinished, [this, current, conn]() {
        GUI->disconnect(*conn);
        delete conn;
        while (GUI->currentPlayer()->getRole() != current->GetRole())
            GUI->nextPlayer();
        setHand();
    });
    emit GUI->actionPerformed();
}

void Mediator::setPlayerToolTips()
{
    vector<Player*> players = engine->ChoosePlayer();
    for (Player* player : players) {
        CPlayer* playerGUI = GUI->findPlayer(player->GetRole());
        QString tooltip = QString("<h1>%1</h1><h2>Role: <b>%2</b></h2><table>").arg(QSTR(player->GetNick())).arg(PlayerRole_SL[player->GetRole()]);
        vector<PlayerCard*> cards = player->SeeCards();
        for (PlayerCard* card : cards)
            tooltip += QString("<tr><td style=\"background-color: %1; color: %2; padding: 10px; border: 2px solid green;\"><h3>%3</h3></td></tr>").arg(card->GetColor() == UNKNOWN ? "ORANGE" : DiseaseType_SL[card->GetColor()]).arg(card->GetColor() == BLACK ? "white" : "black").arg(QSTR(card->GetName()));
        tooltip += "</table>";
        playerGUI->getIco()->setToolTip(tooltip);
    }
}

void Mediator::setHand()
{
    vector<PlayerCard*> hand = engine->GetCurrentPlayer()->SeeCards();
    GUI->clearHand();
    for (PlayerCard* card : hand) {
        CCard* cardGUI = GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
        GUI->addCardToHand(cardGUI);
        qDebug() << "add card " << cardGUI;
    }

}

void Mediator::playerUsedCard(CCard* card)
{
    if (card->getType() == CT_SPECIAL) {
        int result = QMessageBox::question(GUI, "Confirm action", QString("Are you sure you want to use %1 Special Card?").arg(card->getCityName()));
        if (result != QMessageBox::Yes)
            return;
        SpecialCardType cardType = (SpecialCardType) SpecialCardType_SL.indexOf(card->getCityName());
        switch (cardType)
        {
        case SC_ONE_QUIET_NIGHT:
            engine->QuietNight();
            emit GUI->actionPerformed();
            break;
        case SC_FORECAST:
            {
                vector<Card*> cards = engine->Forecast();
                QVector<CBoardItem*> cardsGUI;
                for (Card* card : cards)
                    cardsGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
                GUI->zoomOut();
                COverlay* overlay = GUI->showOverlay();
                overlay->displayItems(cardsGUI);
                overlay->setDescription("<h2>Choose order of the disease card. Click the card to place it at the top of Disease Deck.</h2><p>Note: later clicked cards will appear sooner.</p>");
                overlay->setDeleteOnClick(false);
                overlay->setItemDeleteOnClick(true);
                overlay->letPlayerChoose(1, false);
                int numberOfCards = cards.size();
                overlay->connect(overlay, &COverlay::userChoseOne, [this, overlay, numberOfCards](CBoardItem* chosen) {
                    static vector<Card*> cards;
                    cards.push_back(dynamic_cast<CCard*>(chosen)->toLogic());
                    if (cards.size() == numberOfCards) {
                        engine->Forecast(cards);
                        cards.clear();
                        overlay->deleteLater();
                        emit GUI->actionPerformed();
                    }
                });
                emit GUI->actionStarted();
            }
            break;
        case SC_RESILIENT_POPULATION:
            {
                vector<Card*> cards = engine->ResilientPopulation();
                QVector<CBoardItem*> cardsGUI;
                for (Card* card : cards)
                    cardsGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
                GUI->zoomOut();
                COverlay* overlay = GUI->showOverlay();
                overlay->displayItems(cardsGUI);
                overlay->setDescription("<h2>Choose a Disease Card to remove it from game.</h2>");
                overlay->letPlayerChoose(1, true);
                overlay->connect(overlay, &COverlay::userChoseOne, [this](CBoardItem* chosen) {
                    if (chosen == nullptr) {
                        //u¿ytkownik wybra³ "anuluj"
                        emit GUI->actionCancelled();
                    }
                    else {
                        CCard* chosenCard = dynamic_cast<CCard*>(chosen);
                        engine->ResilientPopulation(chosenCard->toLogic());
                        GUI->removeItem(chosenCard);
                        chosenCard->deleteLater();
                        emit GUI->actionPerformed();
                    }
                });
            }
            break;
        case SC_AIRLIFT:
            {
                GUI->zoomOut();
                vector<Player*> players = engine->ChoosePlayer();
                QSet<CPlayer*> playersGUI;
                for (Player* player : players)
                    playersGUI += GUI->findPlayer(player->GetRole());
                COverlay* overlay = GUI->showOverlay();
                overlay->setDescription("<h2>Choose player and then click city to move his pawn.</h2>");
                overlay->track(playersGUI, playersGUI, true);
                overlay->connect(overlay, &COverlay::userChosePlayer, [this, overlay](CPlayer* player) {
                    selectedPlayer = player->toLogic();
                    QSet<City*> targetCities = engine->ChooseMoveEverywhere(selectedPlayer);
                    QSet<CBoardItem*> citiesGUI;
                    for (City* city : targetCities)
                        citiesGUI += GUI->FIND(CCity, QSTR(city->GetName()));
                    overlay->track(citiesGUI, true);
                });
                overlay->connect(overlay, &COverlay::userChoseOne, [this, overlay](CBoardItem* chosen) {
                    if (chosen == nullptr) {
                        //u¿ytkownik wybra³ "anuluj"
                        emit GUI->actionCancelled();
                    }
                    else {
                        CCity* chosenCity = dynamic_cast<CCity*>(chosen);
                        engine->Airlift(selectedPlayer, chosenCity->toLogic());
                        emit GUI->actionPerformed();
                    }
                });
                emit GUI->actionStarted();
            }
            break;
        case SC_GOVERNMENT_GRANT:
            {
                QSet<City*> cities = engine->ChooseCitiesToBuildStation();
                QSet<CBoardItem*> citiesGUI;
                for (City* city : cities)
                    citiesGUI += GUI->FIND(CCity, QSTR(city->GetName()));
                GUI->zoomOut();
                COverlay* overlay = GUI->showOverlay();
                overlay->setDescription("<h2>Choose a city to build a Research Station for free.</h2>");
                overlay->track(citiesGUI, true);
                overlay->connect(overlay, &COverlay::userChoseOne, [this](CBoardItem* chosen) {
                    if (chosen == nullptr) {
                        //u¿ytkownik wybra³ "anuluj"
                        emit GUI->actionCancelled();
                    }
                    else {
                        CCity* chosenCity = dynamic_cast<CCity*>(chosen);
                        engine->GovernmentGrant(chosenCity->toLogic());
                        emit GUI->actionPerformed();
                    }
                });
                emit GUI->actionStarted();
            }
            break;
        default:
            throw "dupa, z³y typ karty";
            break;
        }
    }
    else if (card->getCityName().toStdString() == engine->GetCurrentPlayer()->GetPosition()->GetName()) {
        int result = QMessageBox::question(GUI, "Confirm action", QString("Are you sure you want to discard %1 Card to move anywhere?").arg(card->getCityName()));
        if (result != QMessageBox::Yes)
            return;
        QSet<City*> cities = engine->ChooseMoveEverywhere(engine->GetCurrentPlayer());
        QSet<CBoardItem*> citiesGUI;
        for (City* city : cities)
            citiesGUI += GUI->FIND(CCity, QSTR(city->GetName()));
        GUI->zoomOut();
        COverlay* overlay = GUI->showOverlay();
        overlay->setDescription("<h2>Choose a city to move your pawn.</h2>");
        overlay->track(citiesGUI, true);
        overlay->connect(overlay, &COverlay::userChoseOne, [this](CBoardItem* chosen) {
            if (chosen == nullptr) {
                //u¿ytkownik wybra³ "anuluj"
                emit GUI->actionCancelled();
            }
            else {
                CCity* chosenCity = dynamic_cast<CCity*>(chosen);
                engine->MoveEverywhere(chosenCity->toLogic());
                emit GUI->actionPerformed();
            }
        });
        emit GUI->actionStarted();
    }
    else {
        int result = QMessageBox::question(GUI, "Confirm action", QString("Are you sure you want to discard %1 Card to move there?").arg(card->getCityName()));
        if (result != QMessageBox::Yes)
            return;
        engine->MoveToCard(dynamic_cast<PlayerCard*>(card->toLogic()));
        emit GUI->actionPerformed();
    }
}

void Mediator::addDiseaseCube(City *infectedCity, DiseaseType color, int count)
{
    qDebug() << "add cube to " << QSTR(infectedCity->GetName());
    checkGUI();
    CCity* cityGUI = GUI->FIND(CCity, QSTR(infectedCity->GetName()));
    for (int i = 0; i < count; ++i)
        cityGUI->addCube(color);
}

void Mediator::removeCube(City *healedCity, DiseaseType color, int count)
{
    qDebug() << "remove cube from " << QSTR(healedCity->GetName());
    checkGUI();
    CCity* cityGUI = GUI->FIND(CCity, QSTR(healedCity->GetName()));
    for (int i = 0; i < count; ++i)
        cityGUI->removeCube(color);
}

void Mediator::movePawn(Player *player, City *destination)
{
    qDebug() << "move pawn " << PlayerRole_SL[player->GetRole()] << " to " << QSTR(destination->GetName());
    checkGUI();
    CCity* cityGUI = GUI->FIND(CCity, QSTR(destination->GetName()));
    GUI->findPawn(player->GetRole())->moveTo(cityGUI);
}

void Mediator::moveCard(Card *card, CardDeck * dest)
{
    qDebug() << "move card " << QSTR(card->GetName()) << "to deck " << DeckType_SL[dest->GetType()];
    checkGUI();
    CCard* cardGUI = GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
    CDeck* deckGUI = GUI->FIND(CDeck, dest->GetType());
    if (cardGUI == nullptr)
        throw QString("Card %1 not found in game!").arg(QSTR(card->GetName()));
    if (deckGUI == nullptr)
        throw QString("Deck %1 not found in game!").arg(DeckType_SL[dest->GetType()]);
    if (!cardGUI->isVisible()) { //jest u gracza
        QWidget* par = dynamic_cast<QWidget*>(GUI->parent());
        CPoint boardEndPos = GUI->mapFrom(par, deckGUI->mapTo(par, CPoint(deckGUI->size()) / 2));
        double endFactor = GUI->getZoom();
        double beginFactor = cardGUI->getZoom();
        cardGUI->show();
        QParallelAnimationGroup* group = new QParallelAnimationGroup(cardGUI);
        group->addAnimation(cardGUI->createStandardMiddleAnim(boardEndPos / endFactor));
        QPropertyAnimation* scaleAnim = createPropertyAnimation(cardGUI, "zoomFactor", cardGUI->getZoom(), endFactor, 1000, QEasingCurve::OutQuad);
        scaleAnim->connect(scaleAnim, &QPropertyAnimation::stateChanged, cardGUI, &CCard::scaleAnimationChanged);
        scaleAnim->connect(group, &QParallelAnimationGroup::finished, cardGUI, &CCard::restoreParent);
        GUI->removeCardFromHand(cardGUI);
        group->addAnimation(scaleAnim);
        GUI->addAnimation(group);
    }
    else {
        cardGUI->setStandardMiddleAnim(deckGUI->getStandardMiddle());
    }
    if (cardGUI->isReversed() != deckGUI->isReversed())
        cardGUI->invert();
}

void Mediator::moveCard(Card *card, Player * dest)
{
    qDebug() << "move card" << QSTR(card->GetName()) << " to player " << PlayerRole_SL[dest->GetRole()];
    checkGUI();
    CCard* cardGUI = GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
    CPlayer* playerGUI = GUI->findPlayer(dest->GetRole());
    if (cardGUI == nullptr)
        throw QString("Card %1 not found in game!").arg(QSTR(card->GetName()));
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
    QParallelAnimationGroup* group = new QParallelAnimationGroup(cardGUI);
    cardGUI->setParent(GUI);
    cardGUI->show();
    cardGUI->setStandardMiddle(boardStartPos / cardGUI->getZoom());
    group->addAnimation( cardGUI->createStandardMiddleAnim(boardEndPos / endFactor) );
    QPropertyAnimation* scaleAnim = createPropertyAnimation(cardGUI, "zoomFactor", cardGUI->getZoom(), endFactor, 1000, QEasingCurve::InQuad);
    scaleAnim->connect(scaleAnim, &QPropertyAnimation::stateChanged, cardGUI, &CCard::scaleAnimationChanged);
    scaleAnim->connect(group, &QPropertyAnimation::finished, cardGUI, &CCard::hide);
    group->addAnimation(scaleAnim);
    GUI->addAnimation(group);
}

void Mediator::setDiseaseStatus(DiseaseType color, CureStatus status)
{
    qDebug() << "set disease status";
    checkGUI();
    CCureMarker* diseaseMarker = GUI->FIND(CCureMarker, color);
    diseaseMarker->setStatus(status);
}

void Mediator::buildResearchStation(City *destination)
{
    qDebug() << "build station in " << QSTR(destination->GetName());
    checkGUI();
    CCity* cityGUI = GUI->FIND(CCity, QSTR(destination->GetName()));
    cityGUI->buildResearchStation();
}

void Mediator::increaseOutbreaksMarker(int count)
{
    qDebug() << "increase outbreaks marker" ;
    checkGUI();
    CPathMarker* marker = GUI->findChild<CPathMarker*>("CBoardItem Outbreaks marker");
    marker->moveToNext(count);
}

void Mediator::increaseInfectionsMarker()
{
    qDebug() << "increase infections marker" ;
    checkGUI();
    CPathMarker* marker = GUI->findChild<CPathMarker*>("CBoardItem Infections marker");
    marker->moveToNext();
}

void Mediator::removeResearchStation(City *destination)
{
    qDebug() << "remove research station" ;
    checkGUI();
    CCity* cityGUI = GUI->FIND(CCity, QSTR(destination->GetName()));
    cityGUI->removeResearchStation();
}

void Mediator::seeDeck(DeckType type)
{
    vector<Card*> cards = engine->SeeDeck(type);
    QVector<CBoardItem*> cardsGUI;
    for (Card* card : cards)
        cardsGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
    COverlay* overlay = GUI->showOverlay();
    overlay->displayItems(cardsGUI);
    QString description = QString("<h2>See the contant of the Deck. Press Ok when finished.</h2>");
    overlay->setDescription(description);
    overlay->letPlayerChoose(0, false);
}

void Mediator::endGame(GameResult res)
{
    qDebug() << "end game" ;
    emit GUI->endGame(res);
    // TODO: Mediator::endGame not implemented
}

void Mediator::chooseStationToRemove(std::vector<City*> stations)
{
    qDebug() << "choose station to remove" ;
    checkGUI();
    QSet<CBoardItem*> stationsGUI;
    for (City* city : stations)
        stationsGUI << GUI->FIND(CCity, QSTR(city->GetName()));

    GUI->zoomOut();
    COverlay* overlay = GUI->showOverlay();
    overlay->track(stationsGUI, true);
    overlay->setDescription("You are out of Research Stations. To build a new one, choose an existing Station to remove.");
    overlay->connect(overlay, &COverlay::userChoseOne, [this](CBoardItem* chosen) {
        if (chosen == nullptr) {
            //u¿ytkownik wybra³ "anuluj"
            emit GUI->actionCancelled();
        }
        else {
            CCity* chosenCity = dynamic_cast<CCity*>(chosen);
            engine->BuildStationIfFull(chosenCity->toLogic());
        }
    });
}

void Mediator::ShareKnowledge()
{
    vector<Player*> players = engine->ChoosePlayer();
    QSet<CPlayer*> playersGUI;
    for (Player* player : players)
        playersGUI += GUI->findPlayer(player->GetRole());
    vector<Player*> playersToShare = engine->ChoosePlayerToShareKnowledge();
    QSet<CPlayer*> playersToShareGUI;
    for (Player* player : playersToShare)
        playersToShareGUI += GUI->findPlayer(player->GetRole());
    COverlay* overlay = GUI->showOverlay();
    overlay->setDescription("<h2>Choose player and then click card to share it.</h2>");
    overlay->track(playersGUI, playersToShareGUI, true);
    static Player* chosenPlayer;
    overlay->connect(overlay, &COverlay::userChosePlayer, [this, overlay](CPlayer* player) {
        chosenPlayer = player->toLogic();
        vector<PlayerCard*> currentPlayerCards = engine->GetCurrentPlayer()->SeeCards();
        vector<PlayerCard*> guestCards = chosenPlayer->SeeCards();
        overlay->clearDisplay();
        overlay->setDescription("<h2>Choose player and then click card to share it.</h2>");
        vector<PlayerCard*> cardsToGive = engine->ChooseCardToGive(chosenPlayer);
        vector<PlayerCard*> cardsToGain = engine->ChooseCardToGain(chosenPlayer);
        QVector<CBoardItem*> currentPlayerCardsGUI;
        for (PlayerCard* card : currentPlayerCards)
            currentPlayerCardsGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
        QVector<CBoardItem*> guestCardsGUI;
        for (PlayerCard* card : guestCards)
            guestCardsGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
        QSet<CBoardItem*> toGiveGUI;
        for (PlayerCard* card : cardsToGive)
            toGiveGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
        QSet<CBoardItem*> toGainGUI;
        for (PlayerCard* card : cardsToGain)
            toGainGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
        overlay->displayItems(currentPlayerCardsGUI, GUI->currentPlayer()->getColor());
        overlay->displayItems(guestCardsGUI, player->getColor());
        overlay->setEnabledItems(toGainGUI + toGiveGUI);
        overlay->letPlayerChoose(1, true);
    });
    overlay->connect(overlay, &COverlay::userChoseOne, [this, overlay](CBoardItem* chosen) {
        if (chosen == nullptr) {
            //u¿ytkownik wybra³ "anuluj"
            emit GUI->actionCancelled();
        }
        else {
            CCard* chosenCard = dynamic_cast<CCard*>(chosen);
            PlayerCard* logicCard = dynamic_cast<PlayerCard*>(chosenCard->toLogic());
            if (chosenPlayer->FindCard(logicCard->GetName()) == nullptr)
                engine->GiveCard(chosenPlayer, logicCard);
            else
                engine->GainCard(chosenPlayer, logicCard);
            emit GUI->actionPerformed();
        }
    });
}

void Mediator::playerMayDiscardCards(int count)
{
    qDebug() << "player may discard cards" ;
    Player* player = engine->GetCurrentPlayer();
    vector<PlayerCard*> cards = player->SeeCards();
    QVector<CBoardItem*> cardsGUI;
    for (PlayerCard* card : cards)
        cardsGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
    COverlay* overlay = GUI->showOverlay();
    overlay->displayItems(cardsGUI);
    QString description = QString("<h2>Choose <b>%1</b> cards in the same color to discover a cure for the disease in this color</h2>").arg(count);
    overlay->setDescription(description);
    overlay->letPlayerChoose(count, true);
    overlay->connect(overlay, &COverlay::userChoseMany, [this](const QSet<CBoardItem*> chosenGUI) {
        if (chosenGUI.size()==0) {
            //u¿ytkownik wybra³ "anuluj"
            emit GUI->actionCancelled();
        }
        else {
            QVector<Card*> chosen;
            for (CBoardItem* item : chosenGUI)
                chosen += dynamic_cast<CCard*>(item)->toLogic();
            engine->DiscoverCure_FinalStep(chosen);
            emit GUI->actionPerformed();
        }
    });
}

void Mediator::playerMustDiscardCards(Player * player, int count)
{
    qDebug() << "player must discard cards" ;
    vector<PlayerCard*> cards = player->SeeCards();
    QVector<CBoardItem*> cardsGUI;
    for (PlayerCard* card : cards)
        cardsGUI += GUI->FIND(CCard, QSTR(card->GetName()), findType(card));
    COverlay* overlay = GUI->showOverlay();
    overlay->displayItems(cardsGUI);
    QString description = QString("<h2>You've exceeded you card limit. Discard up to 7 cards - discard <b>%1</b> cards or use special cards.</h2>").arg(count);
    overlay->setDescription(description);
    overlay->letPlayerChoose(count, false);
    selectedPlayer = player;
    overlay->connect(overlay, &COverlay::userChoseMany, [this](const QSet<CBoardItem*> chosenGUI) {
        if (chosenGUI.size() == 0) {
            //u¿ytkownik wybra³ "anuluj"
            emit GUI->actionCancelled();
        }
        else {
            vector<PlayerCard*> chosen;
            for (CBoardItem* item : chosenGUI)
                chosen.push_back(dynamic_cast<PlayerCard*>(dynamic_cast<CCard*>(item)->toLogic()));
            engine->DiscardToLimit(chosen, selectedPlayer);
            emit GUI->actionPerformed();
        }
    });
}

Mediator & mediator()
{
    static Mediator instance;
    return instance;
}
