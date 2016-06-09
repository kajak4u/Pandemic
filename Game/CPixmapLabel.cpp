#include "CPixmapLabel.hpp"

CPixmapLabel::CPixmapLabel(QWidget * parent) : QLabel(parent) {
    setMinimumSize(1, 1);
}

CPixmapLabel::~CPixmapLabel() {
	
}

int CPixmapLabel::heightForWidth(int width) const
{
    return ((qreal)pix.height()*width) / pix.width();
}

QSize CPixmapLabel::sizeHint() const
{
    QSize hint = QLabel::sizeHint();
    //int w = this->width();
    QWidget *par = dynamic_cast<QWidget*>(parent());
    int w = qMin(par->width(), pix.width());
    return QSize(w, heightForWidth(w));
}

void CPixmapLabel::setPixmap(const QPixmap& p)
{
    pix = p;
    QLabel::setPixmap(p);
}

void CPixmapLabel::resizeEvent(QResizeEvent *)
{
    if (!pix.isNull())
        QLabel::setPixmap(pix.scaled(this->size(),
            Qt::KeepAspectRatio, Qt::FastTransformation));
}

