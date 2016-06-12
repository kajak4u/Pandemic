#pragma once
#include <QLabel>
#include <QSet>
#include <QMap>

class CBoardItem;
class QPushButton;
class CPlayer;

class COverlay : public QLabel {
	Q_OBJECT

public:
	COverlay(QWidget * parent = Q_NULLPTR);
	~COverlay();
    void track(const QSet<CBoardItem*>& items, bool canCancel);
    void track(const QSet<CPlayer*>& players, bool canCancel);
    void setDescription(const QString&);
    void displayItems(const QVector<CBoardItem*>& items);
    void setDeleteOnClick(bool);
    void setItemDeleteOnClick(bool);
    void letPlayerChoose(int count, bool canCancel);
private:
    bool deleteOnClick;
    bool deleteItemOnClick;
    QMap<CBoardItem*, CBoardItem*> links;
    int numberToSelect;
    QSet<CBoardItem*> selected;
    QPushButton* performButton;
    QPushButton* cancelButton;
private slots:
    void itemMoved(const QPoint& pt);
    void itemResized(const QSize & siz);
    void itemClicked();
    void itemToggled();
    void perform();
    void cancel();
signals:
    void userChosePlayer(CPlayer*);
    void userChoseOne(CBoardItem*);
    void userChoseMany(const QSet<CBoardItem*>&);
};
