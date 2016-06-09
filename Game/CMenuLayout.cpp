#include "CMenuLayout.hpp"

CMenuLayout::CMenuLayout(QWidget *parent)
    : QVBoxLayout(parent)
{

}

CMenuLayout::~CMenuLayout()
{
    for (QWidget* widget : objects)
        delete widget;
}
