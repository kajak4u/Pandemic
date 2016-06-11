#pragma once
#include <QLabel>
#include <QSet>
class CBoardItem;

class COverlay : public QLabel {
	Q_OBJECT

public:
	COverlay(QWidget * parent = Q_NULLPTR);
	~COverlay();
    void track(const QSet<CBoardItem*>& items);
private:
    QVector<QMetaObject::Connection> references;
signals:
    void userMadeChoice(CBoardItem*);
};
