#include "CMainWindow.hpp"
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include "CMenuLayout.hpp"
#include "CNewGameMenu.hpp"
#include "CHowToPlayWindow.hpp"
#include <QFile>

void reloadStyle() {
    QFile styleFile(":/style.qss");
    styleFile.open(QFile::ReadOnly);
    qApp->setStyleSheet(styleFile.readAll());
}

CMainWindow::CMainWindow(bool animate)
{
    reloadStyle();
    setAttribute(Qt::WA_DeleteOnClose);
    setupWidgets();
    if (animate)
        startingAnimation();
    else
        skipStartingAnimation();
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::startingAnimation()
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint); //FramelessWindowHint wymagane do przezroczystego t³a
    setAttribute(Qt::WA_TranslucentBackground, true);
    QRect screenRect = QApplication::desktop()->screenGeometry();
    QSequentialAnimationGroup *group = new QSequentialAnimationGroup(this);
    QPropertyAnimation* fadeIn = new QPropertyAnimation(this, "windowOpacity", this);
    fadeIn->setDuration(2000);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    group->addAnimation(fadeIn);
    QParallelAnimationGroup *moveGroup = new QParallelAnimationGroup(this);
    QPropertyAnimation* imageRight = new QPropertyAnimation(pandemicImage, "geometry", this);
    imageRight->setStartValue(pandemicImage->geometry());
    imageRight->setEndValue(pandemicImage->geometry().translated(screenRect.width() - pandemicImage->geometry().right(), 0));
    imageRight->setDuration(1000);
    imageRight->setEasingCurve(QEasingCurve::InOutCubic);
    moveGroup->addAnimation(imageRight);
    QPropertyAnimation* menuLeft = new QPropertyAnimation(menu, "geometry", this);
    menuLeft->setStartValue(menu->geometry());
    menuLeft->setEndValue(QRect(0,0, screenRect.width()-pandemicImage->width()+1, menu->height()));
    menuLeft->setDuration(1000);
    menuLeft->setEasingCurve(QEasingCurve::InOutCubic);
    moveGroup->addAnimation(menuLeft);
    group->addAnimation(moveGroup);
    group->start();
    connect(group, &QSequentialAnimationGroup::finished, [this]() {
        content->setObjectName("body");
    });
}
void CMainWindow::skipStartingAnimation() {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint); //FramelessWindowHint wymagane do przezroczystego t³a
    setAttribute(Qt::WA_TranslucentBackground, true);
    setProperty("windowOpacity", 1.0);
    QRect screenRect = QApplication::desktop()->screenGeometry();
    pandemicImage->setGeometry(pandemicImage->geometry().translated(screenRect.width() - pandemicImage->geometry().right(), 0));
    menu->setGeometry(QRect(0, 0, screenRect.width() - pandemicImage->width() + 1, menu->height()));
    content->setObjectName("body");
}

void CMainWindow::setupWidgets()
{
    QRect screenRect = QApplication::desktop()->screenGeometry();
    pandemicPixmap = QPixmap("img\\cover.jpg").scaledToHeight(screenRect.height(), Qt::SmoothTransformation);

    content = new QLabel(this);
    setCentralWidget(content);
    pandemicImage = new QLabel(content);
    pandemicImage->resize(pandemicPixmap.size());
    pandemicImage->setPixmap(pandemicPixmap);
    pandemicImage->move(screenRect.width() / 2 - pandemicImage->width() / 2, 0);
    menu = new QLabel(content);
    menu->setObjectName("menu");
    menu->setGeometry(pandemicImage->x(), 0, 1, pandemicImage->height());
    menu->stackUnder(pandemicImage);
    CMenuLayout *menuLayout = new CMenuLayout;
    menu->setLayout(menuLayout);
    menuLayout->setMargin(50);
    menuLayout->addButton("New game", this, &CMainWindow::newGame);
    menuLayout->addButton("Load game", this, &CMainWindow::loadGame);
    menuLayout->addButton("How to play", this, &CMainWindow::howToPlay);
    menuLayout->addButton("About", this, &CMainWindow::about);
    menuLayout->addButton("Quit game", qApp, &QApplication::quit);
}


void CMainWindow::newGame()
{
    CNewGameMenu* item = new CNewGameMenu();
    item->showFullScreen();
    this->close();
    return;
}

void CMainWindow::loadGame()
{
}

void CMainWindow::howToPlay()
{
    CHowToPlayWindow *newWindow = new CHowToPlayWindow();
    newWindow->showFullScreen();
    this->close();
    return;
}

void CMainWindow::about()
{
}
