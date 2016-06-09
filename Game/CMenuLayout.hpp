#pragma once

#include <QVBoxLayout>
#include <QVector>
#include <QPushButton>

class CMenuLayout : public QVBoxLayout
{
    Q_OBJECT
public:
    CMenuLayout(QWidget *parent=nullptr);
    ~CMenuLayout();
    template <typename Func2>
    void addButton(const QString& text, const QObject *receiver, Func2 slot)
    {
        QPushButton* item = new QPushButton(text);
        connect(item, &QPushButton::clicked, receiver, slot);
        addWidget(item);
        objects += item;
    }
    template <typename Func2>
    void addButton(const QString& text, const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot)
    {
        QPushButton* item = new QPushButton(text);
        connect(item, &QPushButton::clicked, receiver, slot);
        addWidget(item);
        objects += item;
    }

private:
    QVector<QWidget*> objects;
};
