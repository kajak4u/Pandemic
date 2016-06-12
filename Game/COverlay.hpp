#pragma once
#include <QLabel>
#include <QSet>
#include <QMap>

class CBoardItem;
class QPushButton;

class COverlay : public QLabel {
	Q_OBJECT

public:
	COverlay(QWidget * parent = Q_NULLPTR);
	~COverlay();
    void track(const QSet<CBoardItem*>& items);
    void setDescription(const QString&);
    void displayItems(const QVector<CBoardItem*>& items);
    void setDeleteOnClick(bool);
    void letPlayerChoose(int count, bool canCancel);
private:
    bool deleteOnClick;
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
signals:
    void perform();
    void cancel();
    void userChoseOne(CBoardItem*);
    void userChoseMany(const QSet<CBoardItem*>&);
};
