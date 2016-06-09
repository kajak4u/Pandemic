#include "CBoardItemsFactory.h"
#include <QMessageBox>
#include "CBoardItem.hpp"
#include <QDebug>

int CBoardItemsFactory::counter = 0;
CBoard *CBoardItemsFactory::board;
QWidget *CBoardItemsFactory::itemsContainer;

CBoardItemsFactory& CBoardItemsFactory::initFrom(QTextStream &ts)
{
    counter = ts.readLine().toInt();
    return *this;
}
void CBoardItemsFactory::saveTo(QTextStream& ts)
{
    ts << counter << endl;
}

CBoardItem * CBoardItemsFactory::create(QTextStream &is)
{
    QString type = is.readLine();
    QByteArray typeName = type.toLocal8Bit();
    int id = QMetaType::type(typeName);
    CBoardItem *newItem;
    if (id == QMetaType::UnknownType) {
        QMessageBox::critical(nullptr, "Unknown type", "Cannot create object of unknown type " + typeName);
        return nullptr;
    }
    newItem = (CBoardItem*)QMetaType::create(id);
    newItem->setParent(itemsContainer);
    newItem->setContainer(board);
    newItem->loadFrom(is);
    newItem->show();
    newItem->rename();
    return newItem;
}

CBoardItem *CBoardItemsFactory::create(QString className, double zoomFactor, CPoint middle)
{
    int classid = QMetaType::type(className.toLocal8Bit());
    CBoardItem *newItem;
    if (classid == QMetaType::UnknownType) {
        QMessageBox::critical(nullptr, "Unknown class", "Error - cannot create object of unknown class " + className);
        return nullptr;
    }
    newItem = (CBoardItem*)QMetaType::create(classid);
    newItem->id = ++counter;
    newItem->setContainer(board);
    newItem->setParent(itemsContainer);
    newItem->scale(zoomFactor);
    CPoint pos = middle - itemsContainer->pos() - newItem->size() / 2;
    newItem->setStandardPos(pos);
    newItem->rename();
    newItem->show();
    return newItem;
}

int CBoardItemsFactory::grantId()
{
    return ++counter;
}
