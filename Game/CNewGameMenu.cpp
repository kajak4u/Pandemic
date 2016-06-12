#include "CNewGameMenu.hpp"
#include "CMainWindow.hpp"
#include "enums.h"
#include "CGameWindow.hpp"

CNewGameMenu::CNewGameMenu(QWidget *parent) {
    ui.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    playersCombobox << ui.playerRoleComboBox_1 << ui.playerRoleComboBox_2 << ui.playerRoleComboBox_3 << ui.playerRoleComboBox_4;
    playerRows << (QVector<QWidget*>() << ui.playerId_1 << ui.playerName_1 << ui.playerRoleComboBox_1 << ui.roleDescriptionLabel_1);
    playerRows << (QVector<QWidget*>() << ui.playerId_2 << ui.playerName_2 << ui.playerRoleComboBox_2 << ui.roleDescriptionLabel_2);
    playerRows << (QVector<QWidget*>() << ui.playerId_3 << ui.playerName_3 << ui.playerRoleComboBox_3 << ui.roleDescriptionLabel_3);
    playerRows << (QVector<QWidget*>() << ui.playerId_4 << ui.playerName_4 << ui.playerRoleComboBox_4 << ui.roleDescriptionLabel_4);
    roleDescriptionLabels << ui.roleDescriptionLabel_1 << ui.roleDescriptionLabel_2 << ui.roleDescriptionLabel_3 << ui.roleDescriptionLabel_4;

    connect(ui.backButton, &QPushButton::clicked, this, &CNewGameMenu::exit);
    void (QComboBox::*indexChanged)(int) = &QComboBox::currentIndexChanged;
    //auto applyChangeTo = [](QLabel* receiver) {
    //    return [receiver](int i) {
    //        if (i == -1)
    //            receiver->setText("[Empty]");
    //        else
    //            receiver->setText(RoleDescriptions_SL[i]);
    //    };
    //};
    for (int i = 0; i < playersCombobox.size(); ++i) {
        playersCombobox[i]->clear();
        //connect(playersCombobox[i], indexChanged, applyChangeTo(roleDescriptionLabels[i]));
        connect(playersCombobox[i], indexChanged, this, &CNewGameMenu::filterRoles);
        playersCombobox[i]->insertItems(0, PlayerRole_SL);
        playersCombobox[i]->setCurrentIndex(-1);
        playersCombobox[i]->setCurrentIndex(0);
    }
    void (QSpinBox::*valueChanged)(int) = &QSpinBox::valueChanged;
    connect(ui.numberOfPlayersBox, valueChanged, [this](int i) {
        for (const QVector<QWidget*>& row : playerRows)
            if (--i < 0)
                for (QWidget* item : row)
                    item->hide();
            else
                for (QWidget* item : row)
                    item->show();
    });
    emit ui.numberOfPlayersBox->valueChanged(2);
    connect(ui.startGameButton, &QPushButton::clicked, this, &CNewGameMenu::newGame);
}

CNewGameMenu::~CNewGameMenu()
{
}

void CNewGameMenu::filterRoles(int i)
{
    QComboBox* source = dynamic_cast<QComboBox*>(sender());
    QLabel* target = roleDescriptionLabels[playersCombobox.indexOf(source)];
    if (i == -1)
        target->setText("[Empty]");
    else
        target->setText(RoleDescriptions_SL[i]);
    for (QComboBox* role : playersCombobox) {
        for (int i = 0; i < role->count(); ++i)
            role->setItemData(i, 33, Qt::UserRole - 1);
        for (QComboBox* role2 : playersCombobox)
            if (role != role2 && role2->currentIndex() != 0)
                role->setItemData(role2->currentIndex(), 0, Qt::UserRole - 1);
    }
}

void CNewGameMenu::exit()
{
    CMainWindow *newWindow = new CMainWindow(false);
    newWindow->showFullScreen();
    this->close();
}

void CNewGameMenu::newGame()
{
    int numberOfPlayers = ui.numberOfPlayersBox->value();
    Difficulty difficulty = static_cast<Difficulty>(ui.difficultyComboBox->currentIndex());
    QVector<QPair<QString, PlayerRole> > players;
    for (int i = 0; i < numberOfPlayers; ++i) {
        QString nick = dynamic_cast<QLineEdit*>(playerRows[i][1])->text();
        if (nick.isEmpty())
            nick = QString("player ") + QString::number(i);
        PlayerRole role = static_cast<PlayerRole>(dynamic_cast<QComboBox*>(playerRows[i][2])->currentIndex());
        players << qMakePair(nick, role);
    }
    CGameWindow *newWindow = new CGameWindow(difficulty, players);
    newWindow->showFullScreen();
    this->close();
}