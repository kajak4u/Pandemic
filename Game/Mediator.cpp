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
            pair.second->addCard(findType(card), QString::fromStdString(card->GetName()), card);
        }
    }
    qDebug() << "init end";
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
}

void Mediator::setHand()
{
    vector<PlayerCard*> hand = engine->GetCurrentPlayer()->SeeCards();
    GUI->clearHand();
    for (PlayerCard* card : hand) {
        CCard* cardGUI = GUI->findChild<CCard*>(CCard::createObjectName(QString::fromStdString(card->GetName()), findType(card)));
        GUI->addCardToHand(cardGUI);
        qDebug() << "add card " << cardGUI;
    }

}

bool Mediator::playerUsedCard(CCard* card)
{
    if (card->getType() == CT_SPECIAL) {
        int result = QMessageBox::question(GUI, "Confirm action", QString("Are you sure you want to use %1 Special Card?").arg(card->getCityName()));
        if (result != QMessageBox::Yes)
            return false;
        SpecialCardType cardType = (SpecialCardType) SpecialCardType_SL.indexOf(card->getCityName());
        switch (cardType)
        {
        case SC_ONE_QUIET_NIGHT:
            engine->QuietNight();
            break;
        case SC_FORECAST:
            {
                vector<Card*> cards = engine->Forecast();
                QVector<CBoardItem*> cardsGUI;
                for (Card* card : cards)
                    cardsGUI += GUI->findChild<CCard*>(CCard::createObjectName(QString::fromStdString(card->GetName()), findType(card)));
                GUI->zoomOut();
                COverlay* overlay = GUI->showOverlay();
                overlay->displayItems(cardsGUI);
                overlay->connect(overlay, &COverlay::userChoseOne, [this](CBoardItem* chosen) {
                    if (chosen == nullptr) {
                        //u¿ytkownik wybra³ "anuluj"
                        throw "nie ma takiej opcji";
                    }
                    else {
                        CCity* chosenCity = dynamic_cast<CCity*>(chosen);
                        engine->GovernmentGrant(chosenCity->toLogic());
                    }
                });
            }
            // Overlay
            // Wrzuæ karty
            // odrzucaj po jednym, kolejnoœæ do ustalenia: albo jak na stos (czyli ostatnia odrzucona wejdzie jako pierwsza), albo w kolejnoœci wchodzenia...
            // engine->Forecast(discarded);
            break;
        case SC_RESILIENT_POPULATION:
            {
                vector<Card*> cards = engine->ResilientPopulation();
                QVector<CBoardItem*> cardsGUI;
                for (Card* card : cards)
                    cardsGUI += GUI->findChild<CCard*>(CCard::createObjectName(QString::fromStdString(card->GetName()), findType(card)));
                GUI->zoomOut();
                COverlay* overlay = GUI->showOverlay();
                overlay->displayItems(cardsGUI);
                overlay->letPlayerChoose(1, true);
                overlay->connect(overlay, &COverlay::userChoseOne, [this](CBoardItem* chosen) {
                    if (chosen == nullptr) {
                        //u¿ytkownik wybra³ "anuluj"
                    }
                    else {
                        CCard* chosenCard = dynamic_cast<CCard*>(chosen);
                        engine->ResilientPopulation(chosenCard->toLogic());
                        chosenCard->deleteLater();
                        emit GUI->actionPerformed();
                    }
                });
            }
            // Overlay
            // Wrzuæ karty z DiseaseDiscard
            // Wybierz jedn¹ kartê
            // GUI->removeCard(...) - bo chyba logika z tym nic nie robi...
            // engine->ResilientPopulation(toRemove);
            break;
        case SC_AIRLIFT:
            // Overlay
            // wybierz gracza
            // wybierz miasto docelowe
            // engine->Airlift(player, target);
            break;
        case SC_GOVERNMENT_GRANT:
            {
                QSet<City*> cities = engine->ChooseCitiesToBuildStation();
                QSet<CBoardItem*> citiesGUI;
                for (City* city : cities)
                    citiesGUI += GUI->findChild<CCity*>(CCity::createObjectName(QString::fromStdString(city->GetName())));
                GUI->zoomOut();
                COverlay* overlay = GUI->showOverlay();
                overlay->track(citiesGUI);
                overlay->connect(overlay, &COverlay::userChoseOne, [this](CBoardItem* chosen) {
                    if (chosen == nullptr) {
                        //u¿ytkownik wybra³ "anuluj"
                        emit GUI->actionCancelled();
                    }
                    else {
                        CCity* chosenCity = dynamic_cast<CCity*>(chosen);
                        engine->GovernmentGrant(chosenCity->toLogic());
                    }
                });
            }
            break;
        default:
            throw "dupa, z³y typ karty";
            break;
        }
        return true;
// TODO        Mediator::UseCard - special
    }
    else if (card->getCityName().toStdString() == engine->GetCurrentPlayer()->GetPosition()->GetName()) {
        int result = QMessageBox::question(GUI, "Confirm action", QString("Are you sure you want to discard %1 Card to move anywhere?").arg(card->getCityName()));
        if (result != QMessageBox::Yes)
            return false;
        QSet<City*> cities = engine->ChooseMoveEverywhere(engine->GetCurrentPlayer());
        QSet<CBoardItem*> citiesGUI;
        for (City* city : cities)
            citiesGUI += GUI->findChild<CCity*>(CCity::createObjectName(QString::fromStdString(city->GetName())));
        GUI->zoomOut();
        COverlay* overlay = GUI->showOverlay();
        overlay->track(citiesGUI);
        overlay->connect(overlay, &COverlay::userChoseOne, [this](CBoardItem* chosen) {
            if (chosen == nullptr) {
                //u¿ytkownik wybra³ "anuluj"
                emit GUI->actionCancelled();
            }
            else {
                CCity* chosenCity = dynamic_cast<CCity*>(chosen);
                engine->MoveEverywhere(chosenCity->toLogic());
            }
        });
        return true;
    }
    else {
        int result = QMessageBox::question(GUI, "Confirm action", QString("Are you sure you want to discard %1 Card to move there?").arg(card->getCityName()));
        if (result != QMessageBox::Yes)
            return false;
        engine->MoveToCard(dynamic_cast<PlayerCard*>(card->toLogic()));
        return true;
    }
}

void Mediator::addDiseaseCube(City *infectedCity, DiseaseType color, int count)
{
    qDebug() << "add cube to " << QString::fromStdString(infectedCity->GetName());
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
    qDebug() << "remove cube from " << QString::fromStdString(healedCity->GetName());
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
    qDebug() << "move pawn " << PlayerRole_SL[player->GetRole()] << " to " << QString::fromStdString(destination->GetName());
    checkGUI();
    QString cityName = QString::fromStdString(destination->GetName());
    CCity* cityGUI = dynamic_cast<CCity*>(GUI->findItemByName(cityName));
    if (cityGUI == nullptr)
        throw QString("City ") + cityName + " not found";
    GUI->findPawn(player->GetRole())->moveTo(cityGUI);
}

void Mediator::moveCard(Card *card, CardDeck * dest)
{
    qDebug() << "move card " << QString::fromStdString(card->GetName()) << "to deck " << DeckType_SL[dest->GetType()];
    checkGUI();
    CCard* cardGUI = GUI->findChild<CCard*>(CCard::createObjectName(QString::fromStdString(card->GetName()), findType(card)));
    CDeck* deckGUI = GUI->findChild<CDeck*>(CDeck::createObjectName(dest->GetType()));
    if (cardGUI == nullptr)
        throw QString("Card %1 not found in game!").arg(QString::fromStdString(card->GetName()));
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
    qDebug() << "move card" << QString::fromStdString(card->GetName()) << " to player " << PlayerRole_SL[dest->GetRole()];
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
    QParallelAnimationGroup* group = new QParallelAnimationGroup(cardGUI);
    cardGUI->setParent(GUI);
    cardGUI->show();
    cardGUI->setStandardMiddle(boardStartPos / cardGUI->getZoom());
    group->addAnimation( cardGUI->createStandardMiddleAnim(boardEndPos / endFactor) );
    QPropertyAnimation* scaleAnim = createPropertyAnimation(cardGUI, "zoomFactor", cardGUI->getZoom(), endFactor, 1000, QEasingCurve::InQuad);
    scaleAnim->connect(scaleAnim, &QPropertyAnimation::stateChanged, cardGUI, &CCard::scaleAnimationChanged);
    /*scaleAnim->connect(group, &QPropertyAnimation::finished, cardGUI, &CCard::gotoPlayer);*/
    scaleAnim->connect(group, &QPropertyAnimation::finished, cardGUI, &CCard::hide);
    group->addAnimation(scaleAnim);
    GUI->addAnimation(group);
}

void Mediator::setDiseaseStatus(DiseaseType color, CureStatus status)
{
    qDebug() << "set disease status";
    checkGUI();
    CCureMarker* diseaseMarker = GUI->findChild<CCureMarker*>(CCureMarker::createObjectName(color));
    diseaseMarker->setStatus(status);
}

void Mediator::buildResearchStation(City *destination)
{
    qDebug() << "build station in " << QString::fromStdString(destination->GetName());
    checkGUI();
    QString cityName = QString::fromStdString(destination->GetName());
    CCity* cityGUI = dynamic_cast<CCity*>(GUI->findItemByName(cityName));
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
    QString cityName = QString::fromStdString(destination->GetName());
    CCity* cityGUI = dynamic_cast<CCity*>(GUI->findItemByName(cityName));
    cityGUI->removeResearchStation();
}

void Mediator::endGame(GameResult)
{
    qDebug() << "end game" ;
    // TODO: Mediator::endGame not implemented
}

void Mediator::chooseStationToRemove(std::vector<City*> stations)
{
    qDebug() << "choose station to remove" ;
    checkGUI();
    QSet<CBoardItem*> stationsGUI;
    for (City* city : stations)
        stationsGUI << GUI->findItemByName(QString::fromStdString(city->GetName()));

    GUI->zoomOut();
    COverlay* overlay = GUI->showOverlay();
    overlay->track(stationsGUI);
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

}

void Mediator::playerMayDiscardCards(int count, CALLBACK(Board, void, QVector<Card*>) callbackIfSuccess)
{
    qDebug() << "player may discard cards" ;
    Player* player = engine->GetCurrentPlayer();
    vector<PlayerCard*> cards = player->SeeCards();
    QVector<CBoardItem*> cardsGUI;
    for (PlayerCard* card : cards)
        cardsGUI += GUI->findChild<CCard*>(CCard::createObjectName(QString::fromStdString(card->GetName()), findType(card)));
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

void Mediator::playerMustDiscardCards(Player * which, int count, CALLBACK(Board, void, TWOPARAM(std::vector<PlayerCard*>, Player *))callback)
{
    qDebug() << "player must discard cards" ;
    engine->DiscardToLimit(std::vector<PlayerCard*>(), engine->GetCurrentPlayer());
    // TODO: Mediator::playerMustDiscardCards not implemented
}

Mediator & mediator()
{
    static Mediator instance;
    return instance;
}
