#pragma once
#include <QTextStream>
#include <QLabel>
#include "CPoint.h"

class CBoardItem;
class CBoard;

class CBoardItemsFactory {
    static int counter;
    static CBoard *board;
    static QWidget *itemsContainer;
public:
    CBoardItemsFactory& initFrom(QTextStream&);
    void saveTo(QTextStream&);
    void setWith(CBoard* board, QWidget* container) { this->board = board; itemsContainer = container; }
    CBoardItem* create(QTextStream&);
    CBoardItem* create(QString className, double zoomFactor, CPoint middle);
    CBoard* getBoard() const { return board; }
    int grantId();
};