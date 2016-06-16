#pragma once

#include <QLabel>

class CExtendedSignalWidget : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(bool pressed MEMBER pressed)
public:
    CExtendedSignalWidget(QWidget *parent = nullptr);
    virtual ~CExtendedSignalWidget();
    virtual void mouseMoveEvent(QMouseEvent*) final;
    virtual void mousePressEvent(QMouseEvent*) final;
    virtual void mouseReleaseEvent(QMouseEvent*) final;
    virtual void resizeEvent(QResizeEvent*) final;
    virtual void moveEvent(QMoveEvent*) final;
private:
    bool pressed;

signals:
    void leftButtonDown(QMouseEvent*);
    void rightButtonDown(QMouseEvent*);
    void middleButtonDown(QMouseEvent*);
    void leftButtonUp(QMouseEvent*);
    void rightButtonUp(QMouseEvent*);
    void middleButtonUp(QMouseEvent*);
    void mouseMove(QMouseEvent*);
    void moved(QPoint);
    void resized(QSize);
};
