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
#include "CCard.hpp"

CGameWindow::CGameWindow(Difficulty diff, const QVector<QPair<QString, PlayerRole>>& players, QWidget *parent)
    : QWidget(parent), game(nullptr), diff(diff), players(players)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet("");
    ui.board->setPlayerWidgets(ui.playerLabels, ui.playerCardsVBLayout);
    ui.board->loadFrom("basic.ini");
    connect(ui.playerAreaMinimizeButton, &QToolButton::clicked, this, &CGameWindow::hidePlayerArea);
    connect(ui.playerAreaMaximizeButton, &QToolButton::clicked, this, &CGameWindow::showPlayerArea);
    dynamic_cast<CCureMarker*>(ui.board->findItemByName("CureMarker_BLUE"))->connectIcon(ui.cureStatus_blue);
    dynamic_cast<CCureMarker*>(ui.board->findItemByName("CureMarker_YELLOW"))->connectIcon(ui.cureStatus_yellow);
    dynamic_cast<CCureMarker*>(ui.board->findItemByName("CureMarker_BLACK"))->connectIcon(ui.cureStatus_black);
    dynamic_cast<CCureMarker*>(ui.board->findItemByName("CureMarker_RED"))->connectIcon(ui.cureStatus_red);
    createMenus();
    mediator().setGUI(ui.board);
    connect(this, &CGameWindow::created, this, &CGameWindow::afterCreate, Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(ui.board, &CBoard::cityClicked, this, &CGameWindow::targetCityClicked);
    connect(ui.board, &CBoard::actionStarted, this, &CGameWindow::disableAll);
    connect(ui.board, &CBoard::cardActivated, this, &CGameWindow::waitForNextAction);
    connect(ui.board, &CBoard::actionPerformed, this, &CGameWindow::waitForNextAction);
    connect(ui.board, &CBoard::actionCancelled, this, &CGameWindow::nextAction);
    connect(ui.board, &CBoard::setCurrentStatus, this, &CGameWindow::setStatusBar);
    connect(ui.passButton, &QPushButton::clicked, [this]() {
        engine()->Pass();
        waitForNextAction();
    });
    emit created();
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
    waitForNextAction();
}

void CGameWindow::targetCityClicked(CCity* target)
{
    game->MoveShort(target->toLogic());
    waitForNextAction();
}

void CGameWindow::nextAction()
{
    qDebug() << "ACTION, left " << game->GetMovesLeft();
    mediator().setPlayerToolTips();
    mediator().setHand();
    disconnect(conn);
    QVector<Decision> decisions = QVector<Decision>::fromStdVector(game->IsAbleTo());
    QSet<Decision> decisionsSet;
    for (Decision decision : decisions)
        decisionsSet << decision;
    dispatchDecisions(decisionsSet);
}

void CGameWindow::waitForNextAction()
{
    if (!conn)
        conn = connect(ui.board, &CBoard::animationFinished, this, &CGameWindow::nextAction);
    else
        qDebug() << "dupa";
    disableAll();
}
void CGameWindow::disableAll()
{
    //blokada przed wykonaniem innych akcji w czasie trwania bie¿¹cej
    ui.board->setActiveCities(QSet<CCity*>());
    ui.board->setCardsEnabled(CT_PLAYER, false);
    menu_buildStation->setEnabled(false);
    menu_discoverCure->setEnabled(false);
    menu_shareKnowledge->setEnabled(false);
    menu_treat[BLACK]->setEnabled(false);
    menu_treat[BLUE]->setEnabled(false);
    menu_treat[RED]->setEnabled(false);
    menu_treat[YELLOW]->setEnabled(false);
    ui.passButton->setEnabled(false);
}

void CGameWindow::setStatusBar(int cubesBlue, int cubesYellow, int cubesBlack, int cubesRed, int stations, int playerCards)
{
    ui.blue_info->setText(QString::number(cubesBlue));
    ui.yellow_info->setText(QString::number(cubesYellow));
    ui.black_info->setText(QString::number(cubesBlack));
    ui.red_info->setText(QString::number(cubesRed));
    ui.base_info->setText(QString::number(stations));
    ui.cards_info->setText(QString::number(playerCards));
}

Board * CGameWindow::engine() const
{
    return game;
}

void CGameWindow::dispatchDecisions(const QSet<Decision>& decisions)
{
    CPlayer* currentPlayer = ui.board->currentPlayer();
    CCity* currentCity = currentPlayer->getLocation();
    if (decisions.contains(DEC_MOVE_ANOTHER)) {
        //todo implement dispatcher MOVE_ANOTHER
    }
    if (decisions.contains(DEC_MOVE_SHORT)) {
        QSet<City*> cities = game->ChooseMoveShort(game->GetCurrentPlayer());
        QSet<CCity*> citiesGUI;
        for (City* city : cities)
            citiesGUI << ui.board->findChild<CCity*>(CCity::createObjectName(QString::fromStdString(city->GetName())));
        ui.board->setActiveCities(citiesGUI);
    }
    if (decisions.contains(DEC_MOVE_TO_CARD) || decisions.contains(DEC_MOVE_EVERYWHERE)) {
        ui.board->setCardsEnabled(CT_PLAYER, true);
    }
    menu_buildStation->setEnabled(decisions.contains(DEC_BUILD_STATION));
    menu_discoverCure->setEnabled(decisions.contains(DEC_DISCOVER_CURE));
    if (decisions.contains(DEC_TREAT)) {
        vector<DiseaseType> toTreat = game->ChooseDiseaseToTreat();
        for (DiseaseType dt : toTreat)
            menu_treat[dt]->setEnabled(true);
    }
    if (decisions.contains(DEC_GAIN_CARD) || decisions.contains(DEC_GIVE_CARD)) {
        menu_shareKnowledge->setEnabled(true);
    }
    if (decisions.contains(DEC_USE_SPECIAL)) {
        // TODO decision
    }
    if (decisions.contains(DEC_PASS)) {
        ui.passButton->setEnabled(true);
    }
    if ((decisions - QSet<Decision>({ DEC_PASS, DEC_USE_SPECIAL })).isEmpty()) {
        //gracz mo¿e tylko spasowaæ / zagraæ kartê specjaln¹
        QColor startColor = ui.passButton->palette().color(QPalette::Button);
        QPropertyAnimation* animation = new QPropertyAnimation(ui.passButton, "color");
        animation->setLoopCount(-1);
        animation->setKeyValueAt(0.0, startColor);
        animation->setKeyValueAt(0.25, QColor(0,255,0));
        animation->setKeyValueAt(0.5, startColor);
        animation->setKeyValueAt(1.0, startColor);
        animation->setDuration(2000);
        QMetaObject::Connection* conn = new QMetaObject::Connection;
        *conn = connect(ui.passButton, &CPushButton::clicked, [animation, conn]() {
            animation->pause();
            animation->setLoopCount(1);
            animation->resume();
            animation->disconnect(*conn);
            delete conn;
        });
        animation->start(QAbstractAnimation::DeleteWhenStopped);
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
    menu_treat[BLUE] = cityMenu->addOption(QPixmap(":/icons/img/icons/action_healBlue.png"), "Treat the Blue Disease", [this]() {
        engine()->Treat(BLUE);
        waitForNextAction();
    });
    menu_treat[YELLOW] = cityMenu->addOption(QPixmap(":/icons/img/icons/action_healYellow.png"), "Treat the Yellow Disease", [this]() {
        engine()->Treat(YELLOW);
        waitForNextAction();
    });
    menu_treat[BLACK] = cityMenu->addOption(QPixmap(":/icons/img/icons/action_healBlack.png"), "Treat the Black Disease", [this]() {
        engine()->Treat(BLACK);
        waitForNextAction();
    });
    menu_treat[RED] = cityMenu->addOption(QPixmap(":/icons/img/icons/action_healRed.png"), "Treat the Red Disease", [this]() {
        engine()->Treat(RED);
        waitForNextAction();
    });
    menu_shareKnowledge = cityMenu->addOption(QPixmap(":/icons/img/icons/card.png"), "Share Knowledge", [this]() {
        mediator().ShareKnowledge();
    });
    cityMenu->hide();
    ui.board->setCityMenu(cityMenu);
}
