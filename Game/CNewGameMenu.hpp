#pragma once

#include <QWidget>
#include "ui_CNewGameMenu.h"

class CNewGameMenu : public QWidget
{
    Q_OBJECT

public:
    CNewGameMenu(QWidget *parent = 0);
    virtual ~CNewGameMenu();
    void filterRoles(int i);
    void exit();
    void newGame();
private:
    Ui::NewGameMenu ui;
    QVector<QVector<QWidget*>> playerRows;
    QVector<QComboBox*> playersCombobox;
    QVector<QLabel*> roleDescriptionLabels;
};
