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
    void setDeleteOnClick(bool);
private:
    QVector<QMetaObject::Connection> references;
    bool deleteOnClick;
signals:
    void userMadeChoice(CBoardItem*);
};
