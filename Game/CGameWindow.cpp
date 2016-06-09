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
    ui.board->setPlayerIconLabels(ui.playerLabels);
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
    QVector<Decision> decisions = QVector<Decision>::fromStdVector(game->IsAbleTo());
    dispatchDecisions(decisions);
}

Board * CGameWindow::engine() const
{
    return game;
}

void CGameWindow::dispatchDecisions(const QVector<Decision>& decisions)
{
    for(Decision decision : decisions)
        switch (decision)
        {
        case DEC_MOVE_TO_PLAYER:
            break;
        case DEC_MOVE_ANOTHER:
            break;
        case DEC_MOVE_SHORT:
        {
            QSet<City*> cities = game->ChooseMoveShort(ui.board->currentPlayer()->toLogic());
            QSet<CCity*> citiesGUI;
            for (City* city : cities)
                citiesGUI << ui.board->findChild<CCity*>(CCity::createObjectName(QString::fromStdString(city->GetName())));
            ui.board->setActiveCities(citiesGUI);
            break;
        }
        case DEC_MOVE_TO_CARD:
            break;
        case DEC_MOVE_EVERYWHERE:
            break;
        case DEC_BUILD_STATION:
            break;
        case DEC_DISCOVER_CURE:
            break;
        case DEC_TREAT:
            break;
        case DEC_GAIN_CARD:
            break;
        case DEC_GIVE_CARD:
            break;
        case DEC_USE_SPECIAL:
            break;
        case DEC_PASS:
            break;
        default:
            break;
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
    cityMenu->addOption(QPixmap(":/icons/img/icons/action_buildStation.png"), "Build A Research Station", [this]() {
        engine()->BuildStation();
    });
    cityMenu->addOption(QPixmap(":/icons/img/icons/cure_black_discovered.png"), "Discover A Cure", [this]() {
        engine()->DiscoverCure();
    });
    cityMenu->addOption(QPixmap(":/icons/img/icons/action_healBlue.png"), "Treat the Blue Disease", [this]() {
        engine()->Treat(BLUE);
    });
    cityMenu->addOption(QPixmap(":/icons/img/icons/action_healYellow.png"), "Treat the Yellow Disease", [this]() {
        engine()->Treat(YELLOW);
    });
    cityMenu->addOption(QPixmap(":/icons/img/icons/action_healBlack.png"), "Treat the Black Disease", [this]() {
        engine()->Treat(BLACK);
    });
    cityMenu->addOption(QPixmap(":/icons/img/icons/action_healRed.png"), "Treat the Red Disease", [this]() {
        engine()->Treat(RED);
    });
    cityMenu->addOption(QPixmap(":/icons/img/icons/card.png"), "Share Knowledge", [this]() {
        // TODO "Share Knowledge not implemented";
        //engine()->ShareKnowledge();
    });
    cityMenu->hide();
    ui.board->setCityMenu(cityMenu);
}
