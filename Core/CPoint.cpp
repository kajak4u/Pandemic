#include "CPoint.h"

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
    if (result.x() < area.right())
        result.setX(std::max(origin.x(), result.x()));
    else if (result.x() > area.left())
        result.setX(std::min(origin.x(), result.x()));

    if (result.y() < area.bottom())
        result.setY(std::max(origin.y(), result.y()));
    else if (result.y() > area.top())
        result.setY(std::min(origin.y(), result.y()));
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