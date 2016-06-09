#pragma once
#include <QLabel>

class CPixmapLabel : public QLabel {
	Q_OBJECT

public:
    explicit CPixmapLabel(QWidget * parent = Q_NULLPTR);
	~CPixmapLabel();
    virtual int heightForWidth(int width) const;
    virtual QSize sizeHint() const;
public slots:
    void setPixmap(const QPixmap&);
    void resizeEvent(QResizeEvent*);
private:
    QPixmap pix;
	
};
