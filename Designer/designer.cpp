#include <QApplication>
#include <QSizePolicy>
#include <QWheelEvent>
#include <QEvent>
#include <QMessageBox>
#include "designer.hpp"
#include <QDebug>
#include "core.h"

Designer::Designer(): /*moved(0,0), scale(1.0), */errcode(0) {
    //mainLayout = new QHBoxLayout();
    setDesigner();
    try {
        board = new CBoard();
        board->loadFrom("data.txt");
        setCentralWidget(board);
        board->setPos(CPoint(0, 0));
        board->zoom(1.0);
    }
    catch (...) {
        errcode = 1;
    }
}

Designer::~Designer() {
}

void Designer::closeEvent(QCloseEvent *event)
{
    int answer = QMessageBox::question(this, "Save changes", "Do you want to save the changes?", QMessageBox::Yes | QMessageBox::No);
    if (answer == QMessageBox::Yes)
        board->saveTo("data.txt");
    event->accept();
}
