#include "CPoint.h"
#include <algorithm>
#include <qstring>

CPoint::CPoint(): QPoint()
{
}

CPoint::CPoint(int x, int y): QPoint(x,y)
{
}

CPoint::CPoint(const CPoint & p) : QPoint(p.x(), p.y())
{
}

CPoint::CPoint(const QPoint & p): QPoint(p)
{
}

CPoint::CPoint(const QSize & s): QPoint(s.width(), s.height())
{
}

CPoint CPoint::limited(const CPoint &origin, const QRect & area)
{
    CPoint result(*this);
    QString log;
    if (result.x() < area.right()) {
        log += "x (" + QString::number(result.x()) + ") to small - changed to ";
        result.setX(std::max(origin.x(), result.x()));
        log += QString::number(result.x()) + "\n";
    }
    else if (result.x() > area.left()) {
        log += "x (" + QString::number(result.x()) + ") to great - changed to ";
        result.setX(std::min(origin.x(), result.x()));
        log += QString::number(result.x()) + "\n";
    }
    if (result.y() < area.bottom()) {
        log += "y (" + QString::number(result.y()) + ") to small - changed to ";
        result.setY(std::max(origin.y(), result.y()));
        log += QString::number(result.y()) + "\n";
    }
    else if (result.y() > area.top()) {
        log += "y (" + QString::number(result.y()) + ") to great - changed to ";
        result.setY(std::min(origin.y(), result.y()));
        log += QString::number(result.y()) + "\n";
    }
    return result;
}

CPoint::operator QPoint() const
{
    return QPoint(x(), y());
}

CPoint CPoint::operator+(const QSize& s) const {
    return *this + CPoint(s.width(), s.height());
}
CPoint CPoint::operator-(const QSize& s) const {
    return *this - CPoint(s.width(), s.height());
}
CPoint CPoint::operator+(const QPoint& p) const {
    return CPoint(x()+p.x(), y()+p.y());
}
CPoint CPoint::operator-(const QPoint& p) const {
    return CPoint(x() - p.x(), y() - p.y());
}