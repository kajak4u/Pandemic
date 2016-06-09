#pragma once

#include <QWidget>
#include "ui_CGameWindow.h"
#include "enums.h"
#include "Lukasz\Board.h"
#include <QVector>

class CGameWindow_UI : public Ui::CGameWindow {
public:
    QToolButton* playerAreaMaximizeButton;
    QLabel* playerLabels[4];
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
        }
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
    QVector<QPair<QString, PlayerRole>> players;
    void dispatchDecisions(const QVector<Decision>&);
private slots:
    void hidePlayerArea();
    void showPlayerArea();
    void createMenus();
    void afterCreate();
signals:
    void created();
};

