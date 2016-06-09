#include "CPixmapLoader.hpp"

CPixmapLoader::CPixmapLoader(QObject *parent)
    : QObject(parent)
{

}

CPixmapLoader::~CPixmapLoader()
{

}

CPixmapLoader& CPixmapLoader::add(const QString & path, QLabel * target)
{
    queue += qMakePair(path, target);
    return *this;
}
#include <QDebug>
#include <Windows.h>
#include <QMessageBox>
#include <QApplication>
void CPixmapLoader::start() {
    while (!queue.isEmpty()) {
        int time = GetTickCount();
        QPair<QString, QLabel*> elem = queue.first();
        queue.pop_front();
        QPixmap pxm(elem.first);
        int time2 = GetTickCount(); qDebug() << __LINE__ << " -> " << time2-time;
        emit pixmapLoaded(elem.second, pxm);
        int time3 = GetTickCount(); qDebug() << __LINE__ << " -> " << time3-time2;
    }
    emit finished();
}