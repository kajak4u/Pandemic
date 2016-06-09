#include "CHowToPlayWindow.hpp"
#include "CMainWindow.hpp"
#include <QDebug>
#include <QThread>
#include "CPixmapLoader.hpp"

CHowToPlayWindow::CHowToPlayWindow(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui.setupUi(this);
    int actualWidth = width() - 18;
    auto zoomPreview = [actualWidth](QLabel* label) {
        label->setPixmap(label->pixmap()->scaledToWidth(actualWidth, Qt::SmoothTransformation));
    };
    zoomPreview(ui.page0);
    zoomPreview(ui.page1);
    zoomPreview(ui.page2);
    zoomPreview(ui.page3);
    zoomPreview(ui.page4);
    zoomPreview(ui.page5);
    zoomPreview(ui.page6);
    zoomPreview(ui.page7);
    QThread *watek = new QThread(this);
    CPixmapLoader *loader = new CPixmapLoader(nullptr);
    loader->moveToThread(watek);
    (*loader).add(":/manual/img/instrukcja0.jpg", ui.page0)
        .add(":/manual/img/instrukcja1.jpg", ui.page1)
        .add(":/manual/img/instrukcja2.jpg", ui.page2)
        .add(":/manual/img/instrukcja3.jpg", ui.page3)
        .add(":/manual/img/instrukcja4.jpg", ui.page4)
        .add(":/manual/img/instrukcja5.jpg", ui.page5)
        .add(":/manual/img/instrukcja6.jpg", ui.page6)
        .add(":/manual/img/instrukcja7.jpg", ui.page7);
    connect(watek, &QThread::started, loader, &CPixmapLoader::start);
    connect(loader, &CPixmapLoader::finished, watek, &QObject::deleteLater);
    connect(loader, &CPixmapLoader::pixmapLoaded, [this](QLabel* label, const QPixmap& pxm) {
        this->pixmaps[label] = pxm;
        int actualWidth = width() - 18;
        label->setPixmap(pxm.scaledToWidth(actualWidth, Qt::SmoothTransformation));
    });
    watek->start();
    connect(ui.backBtn, &QPushButton::clicked, this, &CHowToPlayWindow::goBack);
}

CHowToPlayWindow::~CHowToPlayWindow()
{

}

void CHowToPlayWindow::goBack() {
    CMainWindow *newWindow = new CMainWindow(false);
    newWindow->showFullScreen();
    this->close();
}