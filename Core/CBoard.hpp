#pragma once
#include <QVector>
#include <QMap>
#include <QSet>
#include <QPixmap>
#include "CExtendedSignalWidget.hpp"
#include <QAbstractAnimation>
#include "enums.h"
#include "CPoint.h"

class CPlayer;
class QParallelAnimationGroup;
class CBoardItem;
class CPathItem;
class CPawn;
class CCircleMenu;
class COverlay;
class CCity;
class CCard;
class CHandCard;

class Board;


class CBoard : public CExtendedSignalWidget {
    Q_OBJECT
        Q_PROPERTY(double zoomFactor MEMBER zoomFactor WRITE zoomTo);
public:
	CBoard(QWidget * parent = Q_NULLPTR);
	~CBoard();
    void wheelEvent(QWheelEvent*);
    void setPlayerWidgets(QLabel* arr[4], QLayout* playerArea);

    void zoom(double factor, CPoint refPoint = { 0,0 });
    void zoomTo(double newZoomFactor);
    void zoomOut();
    double getZoom() const;
    void setPos(CPoint pos);
    void move(CPoint offset);
    void loadFrom(const QString&);
    void saveTo(const QString&);
    CBoardItem* findItemByName(const QString &);
    CBoardItem* findItemById(int);
    CPawn* findPawn(PlayerRole);
    void addItem(CBoardItem*);
    void removeItem(CBoardItem*);
    void firstPathItem(CPathItem* item);
    
    void showCityMenu(const CPoint& pos);
    void setCityMenu(CCircleMenu*);
    COverlay* showOverlay();
    void addAnimation(QAbstractAnimation*);
    void addPlayer(CPlayer*);
    CPlayer* findPlayer(PlayerRole role);
    CPlayer* currentPlayer() const;
    void setActiveCities(const QSet<CCity*>&);
    bool isCurrentCity(CCity*) const;
    void addCardToHand(CCard*);
    void removeCardFromHand(CCard*);
    void clearHand();
    void nextPlayer();
    double minZoomFactor() const;
private:
    QLayout* playerArea;
    Qt::TransformationMode scaleMode;
    CCircleMenu* cityMenu;
    QWidget* activeMenu;
    double scaleContent(double factor);
    void addItemFromClick(QMouseEvent*);
    void changeInsertableClass(QMouseEvent*);
    QVector<QLabel*> playerLabels; //wskazują kolejkę - [0]-current ... [3]-last
    QVector<CPlayer*> players;
    QVector<CBoardItem*> items;
    QVector<CHandCard*> hand;
    QSet<CCity*> activeCities;
    CPathItem* outbreaksPath;
    CPathItem* infectionsPath;
    CPoint dragStart;
    bool isDragging;
    double zoomFactor;
    CExtendedSignalWidget *container;
    QPixmap image;
    QPixmap smallImage;
    Insertable actualInserted;
    QMap<Decision, QWidget*> decisions;
    QMetaObject::Connection closeMenuConn;
// NOTE sprawdzić, czy nie będzie tego trzeba usunąć:
////////////////////////////// Czy to tak ma być? //////////////////////////
    QSet<QWidget*> cityDecisions; // healDisease x4, findCure, buildResearchStation, shareKnowledge
    QSet<QWidget*> cardDecisions; // (moveFrom / moveTo / use), discard
    QSet<QWidget*> playersDecisions; // moveAnother
public slots:
    void clickCity(QMouseEvent*);
    void onMouseMove(QMouseEvent*);
    void startDragging(QMouseEvent*);
private slots:
    void afterCreate();
    void setMode(Qt::TransformationMode);
    void setSmoothMode();
    void closeCityMenu();
    void useCard(CCard*);
signals:
    void created();
    void cityClicked(CCity*);
    void cardActivated();
    void actionCancelled();
    void actionPerformed();
    void animationFinished();
};
