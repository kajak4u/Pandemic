#pragma once

#include <QObject>
#include <QLabel>

class CPixmapLoader : public QObject
{
    Q_OBJECT

public:
    CPixmapLoader(QObject *parent);
    ~CPixmapLoader();
    CPixmapLoader& add(const QString& path, QLabel* target);
private:
    QVector<QPair<QString, QLabel*>> queue;
public slots :
    void start();
signals:
    void finished();
    void pixmapLoaded(QLabel*, const QPixmap&);
};
