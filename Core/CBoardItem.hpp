#pragma once
#include "CExtendedSignalWidget.hpp"
#include <QTextStream>
#include <QPropertyAnimation>
#include <QBitmap>
#include <QPixmap>
#include "CPoint.h"

class CBoard;
class COption;

QPropertyAnimation* createPropertyAnimation(QWidget* target, const QString& property, const QVariant& startVal, const QVariant& endVal, int duration, QEasingCurve curve=QEasingCurve::Linear);

class CBoardItem : public CExtendedSignalWidget {
    Q_OBJECT
    Q_PROPERTY(bool selected MEMBER selected)
    Q_PROPERTY(QPoint middlePos READ getStandardMiddle WRITE setStandardMiddle)
    Q_PROPERTY(QSize standardSize MEMBER standardSize WRITE setStandardSize)
    Q_PROPERTY(double angleY MEMBER angleY WRITE rotateY)
    Q_PROPERTY(double zoomFactor MEMBER zoomFactor WRITE scaleTo)
    Q_PROPERTY(bool highlighted MEMBER highlighted)
public:
    CBoardItem(QWidget * parent = (QWidget*)defaultContainer);
    CBoardItem(const CBoardItem& other);
	virtual ~CBoardItem();

    void onMouseDown(QMouseEvent*);
    void onLeftBtnUp(QMouseEvent*);
    void onMouseMove(QMouseEvent*);

    CBoardItem* getSelected();
    virtual void select();
    virtual void unselect();
    virtual void toggleSelect();
    bool isSelected() const;
    virtual void loadFrom(QTextStream&);
    void delayedLoad();
    virtual void saveTo(QTextStream&) const;
    void setContainer(CBoard*);
    CBoard* getContainer() const;
    static void setDefaultContainer(CBoard*);
    virtual void scale(double factor);
    void scaleTo(double factor);
    virtual void unload() {}
    void update();
    virtual QString createToolTip();
    void setStandardPos(int x, int y);
    void setStandardPos(const CPoint &p);
    CPoint getStandardPos() const;
    void setStandardSize(int w, int h);
    void setStandardSize(const QSize &s);
    QSize getStandardSize() const;
    CPoint getStandardMiddle() const;
    void setStandardMiddle(const CPoint &p);
    QPropertyAnimation* createStandardMiddleAnim(const CPoint& p);
    void setStandardMiddleAnim(const CPoint& p);
    void setRealPos(int x, int y);
    void setRealPos(const CPoint &p);
    void setRealSize(int w, int h);
    void setRealSize(const QSize &s);
    QString getName() const { return name; }
    void enableResize() { allowResize = true; }
    void disableResize() { allowResize = false; }
    int getId() const { return id; }
    double getZoom() const { return zoomFactor; }
    virtual void paintEvent(QPaintEvent*);
    void rotateY(double);
    void rename();
    virtual QString createObjectName() const;
    static QString createObjectName(const QString& name);
    void setImage(const QPixmap&);
    void setMask(const QBitmap&);
    virtual void cloneTo(CBoardItem*) const;
protected:
    CPoint standardPos;
    QSize standardSize;
    double zoomFactor;
    bool selected;
    QString name;
    CPoint grabStart;
    CBoard *container;
    static CBoard *defaultContainer;
    bool isDragging;
    QVector<COption*> options;
    virtual void updateOptions() {}
    QMap<CBoardItem**, int> references;
    QPixmap image;
    QBitmap itemMask;
    Qt::TransformationMode mode;
    bool highlighted;
private:
    double angleY;
    bool allowResize;
    int id;
    friend class CBoardItemsFactory;
private slots:
    virtual void raiseOnAnimation(QAbstractAnimation::State);
signals:
    void scaled(double);
};
