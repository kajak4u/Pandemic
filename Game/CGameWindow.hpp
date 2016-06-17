#pragma once

#include <QWidget>
#include <QMap>
#include "ui_CGameWindow.h"
#include "enums.h"
#include <QVector>
#include <QPushButton>
#include <QPropertyAnimation>

class QKeyEvent;
class Board;

class CPushButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(QColor color READ getColor WRITE setColor)
public:
    CPushButton(QWidget* parent = 0) : QPushButton(parent), animation(nullptr) {}
    virtual ~CPushButton() {}
    QColor getColor()
    {
        return Qt::transparent;
    }
    void setColor(QColor color)
    {
        setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue()));
    }
    void blinkTillClicked()
    {
        //gracz mo¿e tylko spasowaæ / zagraæ kartê specjaln¹
        if (animation == nullptr)
        {
            QColor startColor = palette().color(QPalette::Button);
            animation = new QPropertyAnimation(this, "color");
            animation->setKeyValueAt(0.0, startColor);
            animation->setKeyValueAt(0.25, QColor(0, 255, 0));
            animation->setKeyValueAt(0.5, startColor);
            animation->setKeyValueAt(1.0, startColor);
            animation->setDuration(2000);
        }
        if (animation->state() == QAbstractAnimation::Stopped) {
            animation->setLoopCount(-1);
            animation->start(QAbstractAnimation::KeepWhenStopped);
            connect(this, &CPushButton::clicked, this, &CPushButton::stopBlink);
        }
    }
private:
    QPropertyAnimation* animation;
public slots:
    void stopBlink()
    {
        if (animation != nullptr && animation->state() == QAbstractAnimation::Running) {
            animation->pause();
            animation->setLoopCount(1);
            animation->resume();
            disconnect(this, &CPushButton::clicked, this, &CPushButton::stopBlink);
        }
    }
};

class CGameWindow_UI : public Ui::CGameWindow {
public:
    QToolButton* playerAreaMaximizeButton;
    QLabel* playerLabels[4];
    CExtendedSignalWidget* playerOverlays[4];
    CPushButton* passButton;
    void setupUi(QWidget *CGameWindow) {
        Ui::CGameWindow::setupUi(CGameWindow);

        playerAreaMaximizeButton = new QToolButton(CGameWindow);
        playerAreaMaximizeButton->setObjectName(QStringLiteral("playerAreaMaximizeButton"));
        playerAreaMaximizeButton->setToolTip(QApplication::translate("CGameWindow", "Maximize card toolbar", 0));
        playerAreaMaximizeButton->setText(QApplication::translate("CGameWindow", "\342\226\272", 0));
        playerAreaMaximizeButton->hide();
        playerAreaMaximizeButton->setGeometry(-25, 0, 22, 19);
        for (int i = 0; i < 4; ++i)
        {
            QLabel* label = new QLabel(CGameWindow);
            label->setObjectName(QStringLiteral("playerLabel%1").arg(QString::number(i)));
            label->setGeometry(160+55*i, 60, 50, 50);
            label->setStyleSheet("border: 1px solid black;");
            label->hide();
            playerLabels[i] = label;
            CExtendedSignalWidget* overlay = new CExtendedSignalWidget(CGameWindow);
            overlay->setGeometry(label->geometry().marginsAdded(QMargins(2, 2, 2, 2)));
            overlay->hide();
            playerOverlays[i] = overlay;
        }
        passButton = new CPushButton(CGameWindow);
        passButton->setObjectName(QStringLiteral("passPushButton"));
        passButton->setText("Pass");
        passButton->setToolTip("End your turn");
        passButton->setGeometry(160, 115, 105, 30);
    }
};

class CGameWindow : public QWidget
{
    Q_OBJECT

public:
    CGameWindow(Difficulty diff, const QVector<QPair<QString, PlayerRole>>& players, QWidget *parent = 0);
    CGameWindow(const QString& filename);
    virtual ~CGameWindow();
    Board* engine() const;

private:
    CGameWindow_UI ui;
    Board *game;
    Difficulty diff;
    CPlayer* actualMovedPlayer;
    QVector<QPair<QString, PlayerRole>> players;
    void dispatchDecisions(const QSet<Decision>&);
    CExtendedSignalWidget *menu_buildStation,
        *menu_discoverCure,
        *menu_shareKnowledge;
    QMap<DiseaseType, CExtendedSignalWidget*> menu_treat;
    QMetaObject::Connection conn;
private slots:
    void hidePlayerArea();
    void showPlayerArea();
    void createMenus();
    void afterCreate();
    void targetCityClicked(CCity*);
    void nextAction();
    void waitForNextAction();
    void disableAll();
    void setStatusBar(int cubesBlue, int cubesYellow, int cubesBlack, int cubesRed, int stations, int playerCards, int outbreaks, int infectionsRate);
    void endGame(GameResult);
    void gotoMenu();
    void choosePlayerToMove();
    void keyReleaseEvent(QKeyEvent*);
signals:
    void created();
};

