#pragma once

#include <QWidget>
#include "ui_CHowToPlayWindow.h"

class CHowToPlayWindow : public QWidget
{
    Q_OBJECT

public:
    CHowToPlayWindow(QWidget *parent = 0);
    virtual ~CHowToPlayWindow();

    void goBack();

private:
    Ui::CHowToPlayWindow ui;
    QMap<QLabel*, QPixmap> pixmaps;
};
