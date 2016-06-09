#pragma once
#include <QPoint>
#include <QRect>
class CPoint :
    public QPoint
{
public:
    CPoint();
    CPoint(int x, int y);
    CPoint(const CPoint& p);
    CPoint(const QPoint& p);
    CPoint(const QSize& s);
    CPoint operator+(const QSize& s) const;
    CPoint operator-(const QSize& s) const;
    CPoint operator+(const QPoint& p) const;
    CPoint operator-(const QPoint& P) const;
    CPoint limited(const CPoint& origin, const QRect& area);
    operator QPoint() const;
};

