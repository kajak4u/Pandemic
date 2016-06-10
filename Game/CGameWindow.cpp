#include "CGameWindow.hpp"
#include <QLabel>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QDebug>
#include "CCureMarker.hpp"
#include "CCircleMenu.hpp"
#include "Mediator.h"
#include "CPlayer.h"
#include "CCity.hpp"

CGameWindow::CGameWindow(Difficulty diff, const QVector<QPair<QString, PlayerRole>>& players, QWidget *parent)
    : QWidget(parent), game(nullptr), diff(diff), players(players)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet("");
    ui.board->setPlayerWidgets(ui.playerLabels, ui.playerCardsVBLayout);
    ui.board->loadFrom("basic.ini");
    connect(ui.playerAreaMinimizeButton, &QToolButton::clicked, this, &CGameWindow::hidePlayerArea);
    /*connect(ui.playerAreaMinimizeButton, &QToolButton::clicked, [this]() {
        ui.playerAreaWidget->move(ui.playerAreaWidget->pos() - CPoint(10, 0));
    });*/
    connect(ui.playerAreaMaximizeButton, &QToolButton::clicked, this, &CGameWindow::showPlayerArea);
    dynamic_cast<CCureMarker*>(ui.board->findItemByName("CureMarker_BLUE"))->connectIcon(ui.cureStatus_blue);
    dynamic_cast<CCureMarker*>(ui.board->findItemByName("CureMarker_YELLOW"))->connectIcon(ui.cureStatus_yellow);
    dynamic_cast<CCureMarker*>(ui.board->findItemByName("CureMarker_BLACK"))->connectIcon(ui.cureStatus_black);
    dynamic_cast<CCureMarker*>(ui.board->findItemByName("CureMarker_RED"))->connectIcon(ui.cureStatus_red);
    createMenus();
    mediator().setGUI(ui.board);
    connect(this, &CGameWindow::created, this, &CGameWindow::afterCreate, Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
    emit created();
//    game->
}

CGameWindow::CGameWindow(const QString & filename) //load game from a file
    : game(nullptr)
{
    ui.setupUi(this);
    setStyleSheet("");
    ui.board->loadFrom(filename);
}

CGameWindow::~CGameWindow()
{
    if (game)
        delete game;
}

void CGameWindow::afterCreate()
{
    game = new Board(diff, players);
    connect(ui.board, &CBoard::cityClicked, this, &CGameWindow::targetCityClicked);
    nextAction();
}

void CGameWindow::targetCityClicked(CCity* target)
{
    game->MoveShort(target->toLogic());
    waitForNextAction();
}

void CGameWindow::nextAction()
{
    disconnect(conn);
    QVector<Decision> decisions = QVector<Decision>::fromStdVector(game->IsAbleTo());
    QSet<Decision> decisionsSet;
    for (Decision decision : decisions)
        decisionsSet << decision;
    dispatchDecisions(decisionsSet);
}

void CGameWindow::waitForNextAction()
{
    conn = connect(ui.board, &CBoard::animationFinished, this, &CGameWindow::nextAction);
}

Board * CGameWindow::engine() const
{
    return game;
}

void CGameWindow::dispatchDecisions(const QSet<Decision>& decisions)
{
    CPlayer* currentPlayer = ui.board->currentPlayer();
    CCity* currentCity = currentPlayer->getLocation();
    if (decisions.contains(DEC_MOVE_TO_PLAYER)) {
    }
    if (decisions.contains(DEC_MOVE_ANOTHER)) {
    }
    if (decisions.contains(DEC_MOVE_SHORT)) {
        QSet<City*> cities = game->ChooseMoveShort(currentPlayer->toLogic());
        QSet<CCity*> citiesGUI;
        for (City* city : cities)
            citiesGUI << ui.board->findChild<CCity*>(CCity::createObjectName(QString::fromStdString(city->GetName())));
        ui.board->setActiveCities(citiesGUI);
    }
    else
        ui.board->setActiveCities(QSet<CCity*>());
    if (decisions.contains(DEC_MOVE_TO_CARD)) {
        // TODO decision
    }
    if (decisions.contains(DEC_MOVE_EVERYWHERE)) {
        // TODO decision
    }
    menu_buildStation->setEnabled(decisions.contains(DEC_BUILD_STATION));
    menu_discoverCure->setEnabled(decisions.contains(DEC_DISCOVER_CURE));
    if (decisions.contains(DEC_TREAT)) {
        menu_treatBlack->setEnabled(currentCity->cubesOf(BLACK)>0);
        menu_treatBlue->setEnabled(currentCity->cubesOf(BLUE)>0);
        menu_treatRed->setEnabled(currentCity->cubesOf(RED)>0);
        menu_treatYellow->setEnabled(currentCity->cubesOf(YELLOW)>0);
    }
    else {
        menu_treatBlack->setEnabled(false);
        menu_treatBlue->setEnabled(false);
        menu_treatRed->setEnabled(false);
        menu_treatYellow->setEnabled(false);
    }
    if (decisions.contains(DEC_GAIN_CARD)) {
        // TODO decision
    }
    if (decisions.contains(DEC_GIVE_CARD)) {
        // TODO decision
    }
    if (decisions.contains(DEC_USE_SPECIAL)) {
        // TODO decision
    }
    if (decisions.contains(DEC_PASS)) {
        // TODO decision
    }
}

void CGameWindow::showPlayerArea()
{
    static QSequentialAnimationGroup *animation = nullptr;
    if (animation == nullptr) {
        animation = new QSequentialAnimationGroup(this);
        QPropertyAnimation *hideMaximizeButton = createPropertyAnimation(ui.playerAreaMaximizeButton, "pos",
            QPoint(0, ui.playerAreaMaximizeButton->y()),
            QPoint(-ui.playerAreaMaximizeButton->width(), ui.playerAreaMaximizeButton->y()),
            250, QEasingCurve::InSine);
        animation->addAnimation(hideMaximizeButton);
        QParallelAnimationGroup* showAreaGroup = new QParallelAnimationGroup(this);
        QPropertyAnimation* minAnimation = createPropertyAnimation(ui.playerAreaWidget, "minimumWidth", 1, 150, 1000, QEasingCurve::OutCubic);
        showAreaGroup->addAnimation(minAnimation);
        QPropertyAnimation* maxAnimation = createPropertyAnimation(ui.playerAreaWidget, "maximumWidth", 1, 150, 1000, QEasingCurve::OutCubic);
        showAreaGroup->addAnimation(maxAnimation);
        animation->addAnimation(showAreaGroup);
    }
    animation->start();
}

void CGameWindow::hidePlayerArea() {
    static QSequentialAnimationGroup *animation = nullptr;
    if (animation == nullptr) {
        animation = new QSequentialAnimationGroup(this);
        QParallelAnimationGroup* hideAreaGroup = new QParallelAnimationGroup(this);
        QPropertyAnimation* minAnimation = createPropertyAnimation(ui.playerAreaWidget, "minimumWidth", 150, 1, 1000, QEasingCurve::InCubic);
        hideAreaGroup->addAnimation(minAnimation);
        QPropertyAnimation* maxAnimation = createPropertyAnimation(ui.playerAreaWidget, "maximumWidth", 150, 1, 1000, QEasingCurve::InCubic);
        hideAreaGroup->addAnimation(maxAnimation);
        animation->addAnimation(hideAreaGroup);
        QPoint minBtnPos = ui.playerAreaMinimizeButton->mapTo(this, QPoint(0, 0));
        ui.playerAreaMaximizeButton->show();
        QPropertyAnimation *showMaximizeButton = createPropertyAnimation(ui.playerAreaMaximizeButton, "pos",
            QPoint(-ui.playerAreaMaximizeButton->width(), minBtnPos.y()),
            QPoint(0, minBtnPos.y()),
            250, QEasingCurve::OutSine);
        animation->addAnimation(showMaximizeButton);
    }
    animation->start();
}

void CGameWindow::createMenus()
{
    CCircleMenu* cityMenu = new CCircleMenu(ui.board);
    menu_buildStation = cityMenu->addOption(QPixmap(":/icons/img/icons/action_buildStation.png"), "Build A Research Station", [this]() {
        engine()->BuildStation();
        waitForNextAction();
    });
    menu_discoverCure = cityMenu->addOption(QPixmap(":/icons/img/icons/cure_black_discovered.png"), "Discover A Cure", [this]() {
        engine()->DiscoverCure();
        waitForNextAction();
    });
    menu_treatBlue = cityMenu->addOption(QPixmap(":/icons/img/icons/action_healBlue.png"), "Treat the Blue Disease", [this]() {
        engine()->Treat(BLUE);
        waitForNextAction();
    });
    menu_treatYellow = cityMenu->addOption(QPixmap(":/icons/img/icons/action_healYellow.png"), "Treat the Yellow Disease", [this]() {
        engine()->Treat(YELLOW);
        waitForNextAction();
    });
    menu_treatBlack = cityMenu->addOption(QPixmap(":/icons/img/icons/action_healBlack.png"), "Treat the Black Disease", [this]() {
        engine()->Treat(BLACK);
        waitForNextAction();
    });
    menu_treatRed = cityMenu->addOption(QPixmap(":/icons/img/icons/action_healRed.png"), "Treat the Red Disease", [this]() {
        engine()->Treat(RED);
        waitForNextAction();
    });
    menu_shareKnowledge = cityMenu->addOption(QPixmap(":/icons/img/icons/card.png"), "Share Knowledge", [this]() {
        // TODO "Share Knowledge not implemented";
        //engine()->ShareKnowledge();
    });
    cityMenu->hide();
    ui.board->setCityMenu(cityMenu);
}
