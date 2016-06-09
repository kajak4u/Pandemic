#include "CExtendedSignalWidget.hpp"
#include <QMouseEvent>
#include <QStyle>
#include <QDebug>

CExtendedSignalWidget::CExtendedSignalWidget(QWidget *parent)
    : QLabel(parent), pressed(false), hover(false)
{
}

CExtendedSignalWidget::~CExtendedSignalWidget()
{
}

void CExtendedSignalWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
    emit mouseMove(event);
}

void CExtendedSignalWidget::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    pressed = true;
    switch (event->button()) {
    case Qt::LeftButton:
        emit leftButtonDown(event);
        break;
    case Qt::RightButton:
        emit rightButtonDown(event);
        break;
    case Qt::MiddleButton:
        emit middleButtonDown(event);
        break;
    }
}
void CExtendedSignalWidget::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
    pressed = false;
    switch (event->button()) {
    case Qt::LeftButton:
        emit leftButtonUp(event);
        break;
    case Qt::RightButton:
        emit rightButtonUp(event);
        break;
    case Qt::MiddleButton:
        emit middleButtonUp(event);
        break;
    }
}

void CExtendedSignalWidget::resizeEvent(QResizeEvent *event)
{
    emit resized(event->size());
}

void CExtendedSignalWidget::moveEvent(QMoveEvent *event)
{
    emit moved(event->pos());
}
