#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>
#include "CBoard.hpp"

class Designer : public QMainWindow {
	Q_OBJECT

public:
	Designer();
	~Designer();
    bool errorOccured() const { return errcode != 0; }
    void closeEvent(QCloseEvent*);
private:
    int errcode;
    /*QPoint moved;
    double scale;
    QHBoxLayout *mainLayout;
    QVBoxLayout *layout1;
    QPushButton *btn, *btn2;*/
    CBoard *board;
    //QScrollArea *scrollArea;
};
