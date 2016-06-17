#include "CBoardItem.hpp"
#include "CBoard.hpp"
#include "COption.h"
#include "CBoardItemsFactory.h"
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QMessageBox>
#include <QStyle>
#include <QTextStream>
#include <QApplication>
#include <QPainter>
#include <QBitmap>
#include <QStyleOption>
#include "core.h"


QPropertyAnimation* createPropertyAnimation(QWidget * target, const QString & property, const QVariant & startVal, const QVariant & endVal, int duration, QEasingCurve curve)
{
    QPropertyAnimation* newAnim = new QPropertyAnimation(target, property.toLocal8Bit());
    newAnim->setStartValue(startVal);
    newAnim->setEndValue(endVal);
    newAnim->setDuration(duration);
    newAnim->setEasingCurve(curve);
    return newAnim;
}


CBoard *CBoardItem::defaultContainer = nullptr;

CBoardItem::CBoardItem(QWidget * parent)
    : CExtendedSignalWidget(parent),
    container(defaultContainer),
    selected(false),
    zoomFactor(1.0),
    allowResize(true),
    angleY(0.0),
    standardSize(70, 70),
    mode(Qt::SmoothTransformation),
    highlighted(false)
{
    standardPos = pos();

    if (isDesigner()) {
        connect(this, &CBoardItem::leftButtonUp, this, &CBoardItem::onLeftBtnUp);
        connect(this, &CBoardItem::leftButtonDown, this, &CBoardItem::onMouseDown);
        connect(this, &CBoardItem::rightButtonDown, this, &CBoardItem::onMouseDown);
        connect(this, &CBoardItem::mouseMove, this, &CBoardItem::onMouseMove);
    }
}

CBoardItem::CBoardItem(const CBoardItem & other) :
    CExtendedSignalWidget((QWidget*)other.parent()),
    container(other.container),
    standardPos(other.standardPos),
    standardSize(other.standardSize),
    zoomFactor(other.zoomFactor),
    selected(other.selected),
    name(other.name),
    allowResize(other.allowResize)
{
        for (COption* opt : other.options)
            options += opt->clone();
        id = CBoardItemsFactory().grantId();
}

CBoardItem::~CBoardItem() {
    if (isSelected())
        unselect();
    for (COption*& opt : options)
        delete opt;
}

void CBoardItem::onMouseDown(QMouseEvent *event)
{
    event->accept();
    grabStart = event->pos();
    isDragging = false;
}

void CBoardItem::onLeftBtnUp(QMouseEvent *event)
{
    event->accept();
    if (event->modifiers() & Qt::ControlModifier) { //Ctrl+Leftbutton -> delete object
        QString message = QString("Are you sure you want to delete the ") + metaObject()->className() + "?\n\n" + toolTip();
        bool ok;
        int confirmation = QMessageBox::warning(nullptr, "Confirmation", message, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
        if (confirmation == QMessageBox::Yes) {
            container->removeItem(this);
        }
        event->ignore();
    }
    else if (!(event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)) && !isDragging) {
        for (COption* opt : options)
            opt->changeByUser();
        updateOptions();
    }
}

void CBoardItem::onMouseMove(QMouseEvent* event) {
    event->accept();
    isDragging = true;
    if (event->buttons() & Qt::LeftButton) {
        setRealPos(mapToParent(event->pos() - grabStart));
    }
    else if (allowResize && event->buttons() & Qt::RightButton) {
        QSize increment = { event->pos().x() - grabStart.x(), event->pos().y() - grabStart.y() };
        setRealSize(size() + increment);
        grabStart = event->pos();
    }
}

CBoardItem * CBoardItem::getSelected()
{
    QVariant selectedProp = container->property("selectedItem");
    if (!selectedProp.isValid())
        return nullptr;
    return (CBoardItem*)selectedProp.value<void*>();
}

void CBoardItem::select()
{
    container->setProperty("selectedItem", qVariantFromValue((void*)this));
    setProperty("selected", true);
    update();
}

void CBoardItem::unselect()
{
    container->setProperty("selectedItem", qVariantFromValue((void*)nullptr));
    setProperty("selected", false);
    update();
}

void CBoardItem::toggleSelect()
{
    if (isSelected())
        unselect();
    else {
        CBoardItem *old = getSelected();
        if (old != nullptr)
            old->unselect();
        this->select();
    }
}


bool CBoardItem::isSelected() const
{
    QVariant prop = property("selected");
    return prop.isValid() && prop.value<bool>();
}

void CBoardItem::loadFrom(QTextStream &ts)
{
    name = ts.readLine();
    id = ts.readLine().toInt();
    int x, y, w, h;
    ts >> x >> y >> w >> h;
    QString newline = ts.readLine();
    setStandardPos(x, y);
    setStandardSize(w, h);
}

void CBoardItem::delayedLoad()
{
    for (auto iter = references.begin(); iter != references.end(); ++iter)
        *iter.key() = container->findItemById(iter.value());
    updateOptions();
}

void CBoardItem::saveTo(QTextStream &ts) const
{
    ts << metaObject()->className() << endl;
    ts << name << endl;
    ts << id << endl;
    QRect r(standardPos, standardSize);
    ts << r.x() << " " << r.y() << " " << r.width() << " " << r.height() << endl;
}

void CBoardItem::setContainer(CBoard *c)
{
    container = c;
}

CBoard * CBoardItem::getContainer() const
{
    return container;
}

void CBoardItem::setDefaultContainer(CBoard *container)
{
    defaultContainer = container;
}

void CBoardItem::scale(double factor)
{
    zoomFactor *= factor;
    if (!image.isNull())
        setPixmap(image.scaled(image.size()*zoomFactor, Qt::IgnoreAspectRatio, mode));
    if(!itemMask.isNull())
        QWidget::setMask(itemMask.scaled(itemMask.size()*zoomFactor));
    move(standardPos*zoomFactor);
    resize(standardSize*zoomFactor);
    emit scaled(factor);
}

void CBoardItem::scaleTo(double factor)
{
    scale(factor / zoomFactor);
}

void CBoardItem::update()
{
    style()->unpolish(this);
    style()->polish(this);
    QWidget::update();
}

QString CBoardItem::createToolTip()
{
    return QString("Type\t ") + metaObject()->className()
        + "\nItem id\t" + QString::number(id);
}

void CBoardItem::setStandardPos(int x, int y)
{
    setStandardPos(CPoint(x, y));
}

void CBoardItem::setStandardPos(const CPoint & p)
{
    standardPos = p;
    move(p*zoomFactor);
}
CPoint CBoardItem::getStandardPos() const
{
    return standardPos;
}

void CBoardItem::setStandardSize(int w, int h)
{
    setStandardSize(QSize(w, h));
}

void CBoardItem::setStandardSize(const QSize & s)
{
    standardSize = s;
    resize(s*zoomFactor);
}

QSize CBoardItem::getStandardSize() const
{
    return standardSize;
}

CPoint CBoardItem::getStandardMiddle() const
{
    return standardPos + standardSize / 2;
}
#include <QDebug>
void CBoardItem::setStandardMiddle(const CPoint &p)
{
    setStandardPos(p - standardSize / 2);
}
QPropertyAnimation* CBoardItem::createStandardMiddleAnim(const CPoint & newMiddle)
{
    QPropertyAnimation* animation = new QPropertyAnimation(this, "middlePos");
    animation->setStartValue(getStandardMiddle());
    animation->setEndValue(newMiddle);
    animation->setDuration(2000);
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    connect(animation, &QAbstractAnimation::stateChanged, this, &CBoardItem::raiseOnAnimation);
    return animation;
}

void CBoardItem::setStandardMiddleAnim(const CPoint & p)
{
    container->addAnimation(createStandardMiddleAnim(p));
}

void CBoardItem::setRealPos(int x, int y)
{
    setRealPos(CPoint(x, y));
}

void CBoardItem::setRealPos(const CPoint & p)
{
    setStandardPos(p / zoomFactor);
}

void CBoardItem::setRealSize(int w, int h)
{
    setRealSize(QSize(w, h));
}

void CBoardItem::setRealSize(const QSize & s)
{
    setStandardSize(s / zoomFactor);
}

void CBoardItem::paintEvent(QPaintEvent *event)
{
    if (pixmap() && !pixmap()->isNull()) {
        QPainter painter(this);
        double scaleY = cos(angleY*3.14159 / 180.0);
        QTransform translateTransform = QTransform().translate(width() / 2, height() / 2);
        if (scaleY > 0)
            painter.setTransform(QTransform().scale(scaleY, 1.0) * translateTransform);
        else
            painter.setTransform(QTransform().translate(width(), 0).scale(scaleY, 1.0) * translateTransform);
        QStyleOption opt; 
        opt.initFrom(this);
        QStyle *style = QWidget::style();
        QPixmap pix = isEnabled() ? *pixmap() : style->generatedIconPixmap(QIcon::Disabled, *pixmap(), &opt);
        painter.drawPixmap(-pixmap()->width() / 2, -pixmap()->height() / 2, pix);
    }
    else
        QLabel::paintEvent(event);
}

void CBoardItem::rotateY(double angle)
{
    angleY = angle;
    update();
}

void CBoardItem::rename()
{
    setObjectName(createObjectName());
}

QString CBoardItem::createObjectName() const
{
    return QString("CBoardItem %1").arg(name.isEmpty() ? " " : name);
}

QString CBoardItem::createObjectName(const QString & name)
{
    return QString("CBoardItem %1").arg(name.isEmpty() ? " " : name);
}

void CBoardItem::setImage(const QPixmap &i)
{
    image = i;
    if (image.isNull()) {
        QString txt = text();
        clear();
        if (!txt.isEmpty())
            setText(txt);
    }
    else
        setPixmap(image.scaled(image.size()*zoomFactor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    if(!image.isNull())
        setStandardSize(image.size());
}

void CBoardItem::setMask(const QBitmap & m)
{
    itemMask = m;
    if (itemMask.isNull())
        QWidget::setMask(itemMask);
    else
        QWidget::setMask(itemMask.scaled(itemMask.size()*zoomFactor, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void CBoardItem::cloneTo(CBoardItem *target) const
{
    target->standardSize = standardSize;
    target->standardPos = standardPos;
    target->setImage(image);
    target->setMask(itemMask);
    target->scaleTo(zoomFactor);
    target->setToolTip(toolTip());
}

void CBoardItem::scaleAnimationChanged(QAbstractAnimation::State state)
{
    if (state == QAbstractAnimation::Running)
        mode = Qt::FastTransformation;
    else if (state == QAbstractAnimation::Stopped)
        mode = Qt::SmoothTransformation;
    scale(1.0);
}

void CBoardItem::raiseOnAnimation(QAbstractAnimation::State state)
{
    if (state == QAbstractAnimation::Running)
        raise();
}