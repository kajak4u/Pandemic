#pragma once

#include <QMainWindow>
class QPushButton;
class QLabel;

void reloadStyle();

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(bool animate=true);
    virtual ~CMainWindow();

private:
    void startingAnimation();
    void skipStartingAnimation();
    void setupWidgets();
    QPixmap pandemicPixmap;
    QLabel* content;
    QLabel* pandemicImage;
    QLabel* menu;
    QPushButton* newGameButton;
    QPushButton* loadGameButton;
    QPushButton* howToPlayButton;
    QPushButton* aboutButton;
    QPushButton* quitGameButton;
#ifdef _DEBUG
    QPushButton* styleButton;
#endif
private slots:
    void newGame();
    void loadGame();
    void howToPlay();
    void about();
};
