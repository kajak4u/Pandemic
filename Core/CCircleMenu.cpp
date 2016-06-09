#include "CCircleMenu.hpp"

CCircleMenu::CCircleMenu(QWidget * parent) : QWidget(parent), dirty(true) {
	
}

CCircleMenu::~CCircleMenu() {
	
}

void CCircleMenu::show(const CPoint & middle)
{
    if (dirty)
        recalc();
    dirty = false;
    CPoint offset = size() / 2;
    move(middle - offset);
    QWidget::show();
}

int CCircleMenu::countItems() const
{
    return items.size();
}

void CCircleMenu::recalc()
{
    const int itemWidth = 50;
    double menuRadius = 2 * itemWidth*double(items.size()) / 6;
    resize(menuRadius + itemWidth, menuRadius + itemWidth);
    int siz = items.size();
    for (int i = 0; i < siz; ++i) {
        double kat = double(i) / siz * 360;
        int x = menuRadius / 2 * (1 - sin(3.14159*kat / 180));
        int y = menuRadius / 2 * (1 - cos(3.14159*kat / 180));
        items[i]->setGeometry(x, y, itemWidth, itemWidth);
        items[i]->setStyleSheet(QString() + "border: 0px outset black; border-radius: " + QString::number(itemWidth / 2) + "px; background-color: #BEE;");
    }
}
