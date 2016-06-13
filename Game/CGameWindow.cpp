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
#include "CMainWindow.hpp"

CGameWindow::CGameWindow(Difficulty diff, const QVector<QPair<QString, PlayerRole>>& players, QWidget *parent)
    : QWidget(parent), game(nullptr), diff(diff), players(players), actualMovedPlayer(nullptr)
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
    connect(ui.board, &CBoard::endGame, this, &CGameWindow::endGame);
    for (int i = 0; i < 4;++i)
        connect(ui.playerOverlays[i], &CExtendedSignalWidget::leftButtonUp, this, &CGameWindow::choosePlayerToMove);
    connect(ui.passButton, &QPushButton::clicked, [this]() {
        engine()->Pass();
        disableAll();
        actualMovedPlayer = nullptr;
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
    game->MoveShort(target->toLogic(), actualMovedPlayer->toLogic());
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
    for (int i = 0; i < 4; ++i)
        ui.playerOverlays[i]->hide();
}

void CGameWindow::setStatusBar(int cubesBlue, int cubesYellow, int cubesBlack, int cubesRed, int stations, int playerCards, int outbreaks, int infectionsRate)
{
    ui.blue_info->setText(QString("<span style=\"color: %1;\">x%2</span>").arg(cubesBlue < 0 ? "red" : "black").arg(cubesBlue));
    ui.yellow_info->setText(QString("<span style=\"color: %1;\">x%2</span>").arg(cubesYellow < 0 ? "red" : "black").arg(cubesYellow));
    ui.black_info->setText(QString("<span style=\"color: %1;\">x%2</span>").arg(cubesBlack < 0 ? "red" : "black").arg(cubesBlack));
    ui.red_info->setText(QString("<span style=\"color: %1;\">x%2</span>").arg(cubesRed < 0 ? "red" : "black").arg(cubesRed));
    ui.base_info->setText(QString("<span style=\"color: %1;\">x%2</span>").arg(stations == 0 ? "red" : "black").arg(stations));
    ui.cards_info->setText(QString("<span style=\"color: %1;\">x%2</span>").arg(playerCards <= 0 ? "red" : "black").arg(playerCards));
    ui.outbreak_info->setText(QString("<span style=\"color: %1;\">x%2</span>").arg(outbreaks >= 8 ? "red" : "black").arg(outbreaks));
    ui.infections_info->setText(QString("<span style=\"color: %1;\">x%2</span>").arg(outbreaks == 4 ? "red" : "black").arg(infectionsRate));
}

void CGameWindow::endGame(GameResult result)
{
    QString color;
    QString title;
    if (result == WON) {
        color = "rgba(64,255,64,192)";
        title = "You won";
    }
    else {
        color = "rgba(255,64,64,192)";
        title = "You lost";

    }
    QString message;
    switch (result)
    {
    case LOST_OUTBREAKS:
        message = "You lost the game because too many outbreaks had occured.";
        break;
    case LOST_CUBES:
        message = "You lost the game because one of the diseases had already infected too many people.";
        break;
    case LOST_CARDS:
        message = "You lost the game because you run out of Player Cards.";
        break;
    case WON:
        message = "Congratulations! You managed it to save humanity!";
        break;
    }
    QWidget* endStatusWidget = new QWidget(this);
    endStatusWidget->setGeometry(0, 0, width(), height());
    endStatusWidget->setObjectName("endStatusWidget");
    endStatusWidget->setStyleSheet(QString("#endStatusWidget {background: %1;}").arg(color));
    QLabel* titleLabel = new QLabel(endStatusWidget);
    titleLabel->setText(QString("<h1>%1</h1>").arg(title));
    titleLabel->setGeometry(0, 200, width(), 200);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->show();
    QLabel* descriptionLabel = new QLabel(endStatusWidget);
    descriptionLabel->setText(QString("<h2>%1</h2>").arg(message));
    descriptionLabel->setGeometry(0, 400, width(), 200);
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->show();
    QPushButton* backBtn = new QPushButton("Back to menu", endStatusWidget);
    backBtn->setGeometry(200, 600, 200, 50);
    backBtn->show();
    connect(backBtn, &QPushButton::clicked, this, &CGameWindow::gotoMenu);
    endStatusWidget->show();
}

void CGameWindow::gotoMenu()
{
    CMainWindow *newWindow = new CMainWindow(false);
    newWindow->showFullScreen();
    this->close();
}

void CGameWindow::choosePlayerToMove()
{
    if (actualMovedPlayer != nullptr)
        for (int i = 0; i < 4;++i)
            if(ui.playerOverlays[i]->geometry().intersects(actualMovedPlayer->getIco()->geometry()))
                ui.playerOverlays[i]->setStyleSheet("background-color: rgba(64,64,64,64); border: 2px solid black;");
    CExtendedSignalWidget* source = dynamic_cast<CExtendedSignalWidget*>(sender());
    vector<Player*> players = game->ChoosePlayer();
    for (Player* player : players) {
        CPlayer* playerGUI = ui.board->findPlayer(player->GetRole());
        if (playerGUI->getIco()->geometry().intersects(source->geometry())) {
            QSet<City*> cities = game->ChooseMoveShort(player);
            QSet<CCity*> citiesGUI;
            for (City* city : cities)
                citiesGUI << ui.board->findChild<CCity*>(CCity::createObjectName(QString::fromStdString(city->GetName())));
            ui.board->setActiveCities(citiesGUI);
            actualMovedPlayer = playerGUI;
            source->setStyleSheet("background-color: rgba(0,255,0,128); border: 3px solid green;");
            break;
        }
    }
    mediator().setActualMovedPlayer(actualMovedPlayer->toLogic());
}

Board * CGameWindow::engine() const
{
    return game;
}

void CGameWindow::dispatchDecisions(const QSet<Decision>& decisions)
{
    if (actualMovedPlayer == nullptr)
        actualMovedPlayer = ui.board->currentPlayer();
    mediator().setActualMovedPlayer(actualMovedPlayer->toLogic());
    CCity* currentCity = actualMovedPlayer->getLocation();
    if (decisions.contains(DEC_MOVE_ANOTHER)) {
        vector<Player*> players = game->ChoosePlayer();
        for (int i = 0; i < players.size(); ++i) {
            ui.playerOverlays[i]->show();
            ui.playerOverlays[i]->raise();
            ui.playerOverlays[i]->setToolTip(ui.board->findPlayer(players[i]->GetRole())->getIco()->toolTip());
        }
        if(actualMovedPlayer == ui.board->currentPlayer())
            ui.playerOverlays[0]->setStyleSheet("background-color: rgba(0,255,0,128); border: 3px solid green;");
    }
    if (decisions.contains(DEC_MOVE_SHORT)) {
        QSet<City*> cities = game->ChooseMoveShort(actualMovedPlayer->toLogic());
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
        if ((decisions - QSet<Decision>({ DEC_PASS, DEC_USE_SPECIAL })).isEmpty()) {
            ui.passButton->blinkTillClicked();
        }
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
    menu_discoverCure = cityMenu->addOption(QPixmap(":/icons/img/icons/action_discoverCure.png"), "Discover A Cure", [this]() {
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
    menu_shareKnowledge = cityMenu->addOption(QPixmap(":/icons/img/icons/action_shareKnowledge.png"), "Share Knowledge", [this]() {
        mediator().ShareKnowledge();
    });
    cityMenu->hide();
    ui.board->setCityMenu(cityMenu);
}
