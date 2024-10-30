#include "settings_message_ui.h"

SettingsMessageUi::SettingsMessageUi(std::string nameUser, QWidget *parent)
    : QWidget(parent) {
    nameUser_ = nameUser;
    setWindowTitle("Réglage Conversation - " + QString::fromStdString(nameUser_));

    mainLayout = new QVBoxLayout(this);
    setObjectName("SettingsMP");
    setStyleSheet("#SettingsMP { background-color: white; }");

    initTop();
    initMiddle();
    initBottom();

    mainLayout->setAlignment(Qt::AlignCenter);
    setFixedSize(700, 500);
}

SettingsMessageUi::~SettingsMessageUi() {}

void SettingsMessageUi::initTop() {
    QVBoxLayout *topLayout = new QVBoxLayout();

    topLayout->addSpacerItem(new QSpacerItem(50, 50, QSizePolicy::Minimum, QSizePolicy::Expanding));
    QPushButton *iconAvatar = new QPushButton();
    iconAvatar->setFixedSize(100, 100);
    iconAvatar->setStyleSheet("border-radius: 50px; background-color: #E8E8E8;");
    #ifdef LOCAL_SERVER
        QIcon settingIcon("./client/Ressources/avatar_ghost.svg");
    #elif defined(DISTANT_SERVER)
        QIcon settingIcon(":/avatar_ghost.svg");
    #endif
    iconAvatar->setIcon(settingIcon);
    iconAvatar->setIconSize(QSize(100, 100));
    iconAvatar->setEnabled(true);

    QLabel *usernameLabel = new QLabel(QString::fromStdString(nameUser_), this);
    usernameLabel->setStyleSheet("font-size: 20px;");

    topLayout->addWidget(iconAvatar, 0, Qt::AlignCenter);
    topLayout->addWidget(usernameLabel, 0, Qt::AlignCenter);

    QFrame *topLine = new QFrame(this);
    topLine->setStyleSheet("background-color: rgb(221, 221, 221); border: none;");
    topLine->setFrameShape(QFrame::HLine);
    topLine->setFrameShadow(QFrame::Sunken);
    topLine->setFixedWidth(this->width() * 0.9);
    topLine->setFixedHeight(1);
    
    topLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    topLayout->addWidget(topLine, 0, Qt::AlignCenter);
    
    mainLayout->addLayout(topLayout);
}

void SettingsMessageUi::initMiddle() {
    QVBoxLayout *middleLayout = new QVBoxLayout();

    QWidget *middleWidget = new QWidget(this);
    QHBoxLayout *middleWidgetLayout = new QHBoxLayout(middleWidget);

    QLabel *logoLabel = new QLabel; 
    #ifdef LOCAL_SERVER
        QIcon iconTimeMsg("./client/Ressources/recent.svg");
    #elif defined(DISTANT_SERVER)
        QIcon iconTimeMsg(":/recent.svg");
    #endif
    logoLabel->setPixmap(iconTimeMsg.pixmap(25, 25));

    QVBoxLayout *msgEphemeresLayout = new QVBoxLayout();
    QLabel *messages = new QLabel("Messages éphémères", this);
    messages->setStyleSheet("font-size: 13px;");
    QLabel *descriptionMessages = new QLabel("Si vous activez cette option, les messages que vous enverrez dans cette conversation disparaîtrons une fois lue.", this);
    descriptionMessages->setStyleSheet("font-size: 12px; color: gray;");
    descriptionMessages->setFixedSize(380, 30);
    descriptionMessages->setWordWrap(true);
    msgEphemeresLayout->addWidget(messages, 0, Qt::AlignLeft);
    msgEphemeresLayout->addWidget(descriptionMessages, 0, Qt::AlignCenter);

    QComboBox *comboBox = new QComboBox(this);
    comboBox->setStyleSheet(
        "QComboBox {"
        "   background-color: white;" 
        "   border: 1px solid rgb(221, 221, 221);"
        "   border-radius: 5px;"
        "   padding: 5px 20px;"
        "   text-align: right;"
        "   color: #333;"
        "}"
        "QComboBox::drop-down {"
        "   border-left: 1px solid transparent;"
        "   width: 10px;"
        "}"
        "QComboBox:hover {"
        "   border: 1px solid rgb(180, 180, 180);" 
        "}"
        "QComboBox:focus {"
        "   background-color: #f0f8ff;"
        "}"
    );
    comboBox->addItem("Desactiver");
    comboBox->addItem("30 Seconde");
    comboBox->addItem("5 Minute");
    comboBox->addItem("1 Heur");
    comboBox->addItem("8 Heur");
    comboBox->addItem("1 Jour");

    middleWidgetLayout->addWidget(logoLabel, 0, Qt::AlignLeft);
    middleWidgetLayout->addItem(new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    middleWidgetLayout->addLayout(msgEphemeresLayout);
    middleWidgetLayout->addItem(new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    middleWidgetLayout->addWidget(comboBox, 0, Qt::AlignRight);

    middleLayout->addWidget(middleWidget, 0, Qt::AlignCenter);

    QFrame *middleLine = new QFrame(this);
    middleLine->setStyleSheet("background-color: rgb(221, 221, 221); border: none;");
    middleLine->setFrameShape(QFrame::HLine);
    middleLine->setFrameShadow(QFrame::Sunken);
    middleLine->setFixedWidth(this->width() * 0.9);
    middleLine->setFixedHeight(1);

    middleLayout->addWidget(middleLine, 0, Qt::AlignCenter);

    mainLayout->addLayout(middleLayout);
}

void SettingsMessageUi::initBottom() {
    QVBoxLayout *bottomLayout = new QVBoxLayout();
    bottomLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QPushButton *deleteMsgButton = new QPushButton(this);
    deleteMsgButton->setFixedWidth(this->width() * 0.9);
    deleteMsgButton->setFixedHeight(40);
    deleteMsgButton->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   border-radius: 5px;"
        "   background-color: transparent;" 
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F1F1F1;"
        "}"
    );
    QHBoxLayout *buttonDeleteMsgLayout = new QHBoxLayout(deleteMsgButton);
    QLabel *deleteMsgLabel = new QLabel("Supprimer les Message", this);
    deleteMsgLabel->setStyleSheet("color: rgb(211, 69, 57);");
    buttonDeleteMsgLayout->setSpacing(20);
    buttonDeleteMsgLayout->addWidget(deleteMsgLabel, 0, Qt::AlignLeft);
    deleteMsgButton->setLayout(buttonDeleteMsgLayout);

    QPushButton *deleteUserButton = new QPushButton(this);
    deleteUserButton->setFixedWidth(this->width() * 0.9);
    deleteUserButton->setFixedHeight(40);
    deleteUserButton->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   border-radius: 5px;"
        "   background-color: transparent;" 
        "   font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F1F1F1;"
        "}"
    );
    QHBoxLayout *buttonDeleteUserLayout = new QHBoxLayout(deleteUserButton);
    QLabel *deleteUserLabel = new QLabel("Supprimer l'utilisateur", this);
    deleteUserLabel->setStyleSheet("color: rgb(211, 69, 57);");
    buttonDeleteUserLayout->setSpacing(20);
    buttonDeleteUserLayout->addWidget(deleteUserLabel, 0, Qt::AlignLeft);
    deleteUserButton->setLayout(buttonDeleteUserLayout);

    bottomLayout->addWidget(deleteMsgButton, 0, Qt::AlignCenter);
    bottomLayout->setSpacing(10);
    bottomLayout->addWidget(deleteUserButton, 0, Qt::AlignCenter);
    bottomLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    mainLayout->addLayout(bottomLayout);
}

void SettingsMessageUi::closeEvent(QCloseEvent *event) {
    emit windowClosed();
    event->accept();
}