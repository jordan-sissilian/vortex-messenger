#include "home_page.h"

extern QButtonGroup *globalButtonGroup;

HomePage::HomePage(QWidget *parent) : QWidget(parent), actualMp("") {
    globalButtonGroup->setExclusive(true);
    setStyleSheet("background-color: rgb(255, 255, 255);");

    horizontalLayout_ = new QHBoxLayout;
    horizontalLayout_->setContentsMargins(0, 0, 0, 0);

    QWidget *SelectRoomsWidget = new QWidget;
    QVBoxLayout *leftVerticalLayout = new QVBoxLayout;
    leftVerticalLayout->setContentsMargins(0, 0, 0, 0);

    initTopWidget();
    initBottomWidget();

    leftVerticalLayout->setSpacing(0);
    leftVerticalLayout->addWidget(topWidget);
    leftVerticalLayout->addWidget(conversationsWidget);
    SelectRoomsWidget->setLayout(leftVerticalLayout);
    SelectRoomsWidget->setFixedWidth(280);
    SelectRoomsWidget->setObjectName("SelectRoomsWidget");
    SelectRoomsWidget->setStyleSheet("#SelectRoomsWidget { padding: 0px; margin: 0px; background-color: white; border-right: 1px solid #C7C7C7; }");

    rightWidget_ = new QWidget;

    stackedWidget_ = new QStackedWidget(rightWidget_);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget_);

    index = new IndexUi();
    stackedWidget_->addWidget(index);

    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->addWidget(stackedWidget_);

    // Layout horizontal
    horizontalLayout_->setSpacing(0);
    horizontalLayout_->addWidget(SelectRoomsWidget);
    horizontalLayout_->addWidget(rightWidget_);
    setLayout(horizontalLayout_);

    QDir directory_contact(CONTACT_DIR_PATH);
    QStringList files = directory_contact.entryList(QDir::Files);
    for (const QString& fileName : files) {
        addConv(ConvType::PM, fileName);
    }

    QDir directory_room(ROOM_DIR_PATH);
    QStringList folders = directory_room.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& folderName : folders) {
        addConv(ConvType::RM, folderName);
    }
}

HomePage::~HomePage() {
    delete conversationsButtons_;
    delete rightWidget_;
    delete horizontalLayout_;
    delete stackedWidget_;
}

void HomePage::initTopWidget() {
    topWidget = new QWidget;
    topWidget->setFixedHeight(70);
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(0, 0, 0, 30);

    QSpacerItem *leftSpacer = new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    topLayout->addSpacerItem(leftSpacer);

    QPushButton *iconSettingButton = new QPushButton();
    iconSettingButton->setCursor(Qt::PointingHandCursor);
    iconSettingButton->setFixedWidth(30); 
    iconSettingButton->setFixedHeight(30); 
    iconSettingButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    iconSettingButton->setObjectName("topButtonSettingWidget");
    iconSettingButton->setStyleSheet(
        "#topButtonSettingWidget { background-color: transparent; color: black; text-align: center; padding: 0px; font-size: 15px; border-radius: 5px; }"
        "#topButtonSettingWidget:hover { background-color: #E1E1E1; }"
        "#topButtonSettingWidget:pressed { background-color: #C4C4C4; }"
        "#topButtonSettingWidget:checked { background-color: #E1E1E1; }"
    );
    #ifdef LOCAL_SERVER
        QIcon settingIcon("./client/Ressources/settings.svg");
    #elif defined(DISTANT_SERVER)
        QIcon settingIcon(":/settings.svg");
    #endif
    iconSettingButton->setIcon(settingIcon);
    iconSettingButton->setIconSize(QSize(20, 20));
    QObject::connect(iconSettingButton, &QPushButton::clicked, [=]() {
        globalButtonGroup->button(2)->setChecked(true);
        setWidget(PageType::Setting);
    });
    iconSettingButton->setCheckable(true);
    globalButtonGroup->addButton(iconSettingButton, 2);

    QLabel *textLabel = new QLabel("Conversations");
    textLabel->setObjectName("allConv");
    textLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    textLabel->setStyleSheet(
        " #allConv {padding: 0px; font-size: 19px; margin: 0px; background-color: transparent; color: black; font-weight: 550; }"
    );

    addConvButton = new QPushButton();
    addConvButton->setCursor(Qt::PointingHandCursor);
    addConvButton->setFixedWidth(30);
    addConvButton->setFixedHeight(30);
    addConvButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addConvButton->setObjectName("btnAdd");
    addConvButton->setStyleSheet(
        "#btnAdd { background-color: transparent; padding: 0px; font-size: 15px; border-radius: 5px; }"
        "#btnAdd:hover { background-color: #E1E1E1; }"
        "#btnAdd:pressed { background-color: #C4C4C4; }"
        "#btnAdd:checked { background-color: #E1E1E1; }"
    );
    #ifdef LOCAL_SERVER
        QIcon addIcon("./client/Ressources/compose.svg");
    #elif defined(DISTANT_SERVER)
        QIcon addIcon(":/compose.svg");
    #endif
    addConvButton->setIcon(addIcon);
    addConvButton->setIconSize(QSize(20, 20));
    QObject::connect(addConvButton, &QPushButton::clicked, [=]() {
        AddConversationWindow *addConvWindow = new AddConversationWindow();
        QObject::connect(addConvWindow, &AddConversationWindow::setNewConvUser, this, &HomePage::setNewConvUserSlot);
        QObject::connect(addConvWindow, &AddConversationWindow::setNewConvRoom, this, &HomePage::setNewConvRoomSlots);
        addConvWindow->show();
    });
    topLayout->addWidget(textLabel); 

    topLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    topLayout->addWidget(addConvButton);
    topLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    topLayout->addWidget(iconSettingButton);
    topLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));

    topWidget->setLayout(topLayout);
    topWidget->setObjectName("topWidget");
    topWidget->setStyleSheet(" #topWidget { background-color: #F0F0F0; border-right: 1px solid #C7C7C7; }");
}

void HomePage::initBottomWidget() {
    conversationsWidget = new QWidget;

    conversationsButtons_ = new QVBoxLayout;
    conversationsButtons_->setContentsMargins(0, 0, 0, 0);
    conversationsButtons_->setSpacing(5);
    conversationsButtons_->setAlignment(Qt::AlignTop);

    scrollArea_ = new QScrollArea;
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setContentsMargins(0, 0, 1, 0);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setStyleSheet(
        "QScrollBar:vertical {"
        "    background: transparent;"
        "    width: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: rgba(0, 0, 0, 0.4);"
        "    border-radius: 2px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0;"
        "    width: 0;"
        "}"
    );

    innerWidgetBtn_ = new QWidget;
    innerWidgetBtn_->setContentsMargins(0, 0, 1, 0);
    innerWidgetBtn_->setStyleSheet("margin: 0; padding: 0; background-color: #F0F0F0;");
    innerWidgetBtn_->setLayout(conversationsButtons_);
    innerWidgetBtn_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    scrollArea_->setWidget(innerWidgetBtn_);

    QVBoxLayout *mainLayout = new QVBoxLayout(conversationsWidget);
    mainLayout->setContentsMargins(0, 0, 1, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(scrollArea_);

    conversationsWidget->setObjectName("conversationsWidget");
    conversationsWidget->setStyleSheet("#conversationsWidget { padding: 0; margin: 0; border: none; border-right: 1px solid #D3D3D3; }");
}

void HomePage::setWidget(PageType pageType, QString name) {
    while (stackedWidget_->count() > 0) {
        QWidget *widget = stackedWidget_->widget(0);
        stackedWidget_->removeWidget(widget);
    }

    if (pageType == PageType::Index) {
        stackedWidget_->addWidget(index);
        viewName = "index";
    } else if (pageType == PageType::Setting) {
        std::cout << "Setting" << std::endl;
        // stackedWidget_->addWidget();
        viewName = "";
    } else if (pageType == PageType::PrivateMessage) {
        actualMp.setNewMp(name.toStdString());
        actualMp.processFiles();
        stackedWidget_->addWidget(&actualMp);
        setButtonCountNotif(name, 0);
        viewName = name.toStdString();
    } else if (pageType == PageType::Room) {
        actualRoom = new RoomUi(name.toStdString());
        stackedWidget_->addWidget(actualRoom); //todo remove *
        viewName = name.toStdString();
    }
}

void HomePage::setWidget(PageType pageType) {
    setWidget(pageType, "");
}

int HomePage::getButtonCountNotif(const QString& username) {
    auto it = userButtonMap_.find(username.toStdString());
    if (it != userButtonMap_.end()) {
        QPushButton* button = it->second;
        QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(button->layout());
        if (layout) {
            QVBoxLayout* notificationLayout = qobject_cast<QVBoxLayout*>(layout->itemAt(layout->count() - 1)->layout());
            if (notificationLayout) {
                QLabel* numberLabel = qobject_cast<QLabel*>(notificationLayout->itemAt(1)->widget());
                if (numberLabel) {
                    return numberLabel->text().toInt();
                }
            }
        }
    }
    return -1;
}

void HomePage::setButtonCountNotif(const QString& username, int newCount) {
    auto it = userButtonMap_.find(username.toStdString());
    if (it != userButtonMap_.end()) {
        QPushButton* button = it->second;
        QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(button->layout());
        if (layout) {
            QVBoxLayout* notificationLayout = qobject_cast<QVBoxLayout*>(layout->itemAt(layout->count() - 1)->layout());
            if (notificationLayout) {
                QLabel* numberLabel = qobject_cast<QLabel*>(notificationLayout->itemAt(1)->widget());
                if (numberLabel) {
                    numberLabel->setText(QString::number(newCount));
                    if (newCount == 0) {
                        numberLabel->setStyleSheet("color: transparent; background-color: transparent;");
                    } else {
                        numberLabel->setStyleSheet("color: #707070; background-color: transparent; font-weight: 500;");
                    }
                }
            }
        }
    }
}

void HomePage::addConv(ConvType convType, QString data) {
    QPushButton *button = new QPushButton();
    button->setFixedHeight(70);
    button->setFixedWidth(250);
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(
        "QPushButton {"
        "border-radius: 10px;"
        "background-color: transparent;"
        "border: none; "
        "color: black; "
        "font-weight: 550;"
        "font-size: 15px;"
        "}"
        "QPushButton:hover {"
        "background-color: #E8E8E8;"
        "}"
        "QPushButton:checked { background-color: #DDDDDD; }"
    );

    QHBoxLayout *buttonLayout = new QHBoxLayout(button);
    buttonLayout->setContentsMargins(10, 0, 0, 0);
    buttonLayout->setSpacing(10);

    QPushButton *iconButton = new QPushButton();
    iconButton->setFixedSize(50, 50);
    iconButton->setStyleSheet("border-radius: 25px; background-color: #E8E8E8;");
    #ifdef LOCAL_SERVER
        QIcon settingIcon("./client/Ressources/avatar_ghost.svg");
    #elif defined(DISTANT_SERVER)
        QIcon settingIcon(":/avatar_ghost.svg");
    #endif
    iconButton->setIcon(settingIcon);
    iconButton->setIconSize(QSize(50, 50));
    iconButton->setEnabled(false);
    buttonLayout->addWidget(iconButton);

    QString name(data);
    QLabel *textLabel = new QLabel(data);
    textLabel->setContentsMargins(5, 0, 0, 0);
    textLabel->setStyleSheet("background-color: transparent; color: black; font-weight: 550; font-size: 15px;");
    buttonLayout->addWidget(textLabel, 0, Qt::AlignLeft);

    QVBoxLayout *notificationLayout = new QVBoxLayout();
    notificationLayout->setContentsMargins(0, 0, 5, 5);

    QString dateString("");
#ifdef __linux__
    QFileInfo fileInfo("/usr/share/clientcryptomadnessvortex/dir_user/conv/mp/" + name + "/messages.bin");
#elif __APPLE__
    QFileInfo fileInfo("/Users/Shared/ClientCryptoMadnessVortex/dir_user/conv/mp/" + name + "/messages.bin");
#endif
    if (fileInfo.exists()) {
        QDateTime lastModified = fileInfo.lastModified();
        QDateTime currentDateTime = QDateTime::currentDateTime();
        if (lastModified.date() == currentDateTime.date()) {
            dateString = lastModified.toString("HH:mm");
        } else {
            dateString = lastModified.toString("dd/MM");
        }
    }
    QLabel *timeLabel = new QLabel(dateString); //todo mise a jour dynamique a chaque notif/msg recu
    timeLabel->setStyleSheet("color: gray; background-color: transparent");
    timeLabel->setFixedWidth(40);
    notificationLayout->addWidget(timeLabel, 0, Qt::AlignRight);

    QLabel *countLabel = new QLabel(QString::number(0));
    countLabel->setStyleSheet("color: transparent; background-color: transparent; border-radius: 20px;");
    countLabel->setFixedWidth(20);
    notificationLayout->addWidget(countLabel, 0, Qt::AlignRight);

    buttonLayout->addLayout(notificationLayout, 0);

    conversationsButtons_->addWidget(button, 0, Qt::AlignCenter);
    userButtonMap_[data.toStdString()] = button;
    button->setCheckable(true);
    int index = 2 + userButtonMap_.size() + 1;
    globalButtonGroup->addButton(button, index);

    QObject::connect(button, &QPushButton::clicked, [=]() {
        globalButtonGroup->button(index)->setChecked(true);
        if (convType == ConvType::PM) {
            setWidget(PageType::PrivateMessage, name);
        } else if (convType == ConvType::RM) {
            setWidget(PageType::Room, name);
        }
    });
}

void HomePage::recvMpMsg(const std::string &username, int msgId) {
    if (viewName == username) {
        actualMp.setUserMsg(username, msgId);
    } else {
        setButtonCountNotif(QString::fromStdString(username), getButtonCountNotif(QString::fromStdString(username)) + 1);
    }
}

void HomePage::mpMsg(int msgId) {
    actualMp.setMsg(msgId);
}

void HomePage::newConv(const std::string &username) {
    QPushButton *userJoined = new QPushButton();
    userJoined->setFixedHeight(70);
    userJoined->setFixedWidth(250);
    userJoined->setCursor(Qt::PointingHandCursor);
    userJoined->setStyleSheet(
        "QPushButton {"
        "border-radius: 10px;"
        "background-color: transparent;"
        "border: none; "
        "color: black; "
        "font-weight: 550;"
        "font-size: 15px;"
        "}"
        "QPushButton:hover {"
        "background-color: #E8E8E8;"
        "}"
        "QPushButton:checked { background-color: #DDDDDD; }"
    );
    conversationsButtons_->addWidget(userJoined);

    QString name = QString::fromStdString(username);
    userJoined->setText(name);

    QIcon icon = userJoined->style()->standardIcon(QStyle::SP_MessageBoxWarning);
    userJoined->setIcon(icon);
    userJoined->setIconSize(QSize(16, 16));

    userWaitButtonMap_[username] = userJoined;

    innerWidgetBtn_->adjustSize();
    innerWidgetBtn_->updateGeometry();
    scrollArea_->widget()->updateGeometry();
    conversationsButtons_->update();
}

void HomePage::newConvAccept(const std::string &username) {
    QPushButton *userJoined = new QPushButton();
    userJoined->setFixedHeight(70);
    userJoined->setFixedWidth(250);
    userJoined->setStyleSheet(
        "QPushButton {"
        "border-radius: 10px;"
        "background-color: transparent;"
        "border: none; "
        "color: black; "
        "font-weight: 550;"
        "font-size: 15px;"
        "}"
        "QPushButton:hover {"
        "background-color: #E8E8E8;"
        "}"
        "QPushButton:checked { background-color: #DDDDDD; }"
    );

    QHBoxLayout *buttonLayout = new QHBoxLayout(userJoined);
    buttonLayout->setContentsMargins(20, 0, 20, 0);
    buttonLayout->setSpacing(10);

    QString name = QString::fromStdString(username);
    QLabel *textLabel = new QLabel(name);
    textLabel->setContentsMargins(20, 0, 0, 0);
    textLabel->setStyleSheet("background-color: transparent; color: black; font-weight: 550; font-size: 15px;");
    buttonLayout->addWidget(textLabel, 0, Qt::AlignLeft);
    buttonLayout->addStretch();

    QPushButton *crossButton = new QPushButton();
    crossButton->setFixedHeight(24);
    crossButton->setFixedWidth(24);
    crossButton->setCursor(Qt::PointingHandCursor);
    crossButton->setStyleSheet(
        "QPushButton { background-color: transparent; color: black; text-align: center; padding: 0px; font-size: 15px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #E1E1E1; }"
        "QPushButton:pressed { background-color: #C4C4C4; }"
    );
    #ifdef LOCAL_SERVER
        QIcon crossIcon("./client/Ressources/block.svg");
    #elif defined(DISTANT_SERVER)
        QIcon crossIcon(":/block.svg");
    #endif
    crossButton->setIcon(crossIcon);
    crossButton->setIconSize(QSize(15, 15));

    QPushButton *checkmarkButton = new QPushButton();
    checkmarkButton->setFixedHeight(24);
    checkmarkButton->setFixedWidth(24);
    checkmarkButton->setCursor(Qt::PointingHandCursor);
    checkmarkButton->setStyleSheet(
        "QPushButton { background-color: transparent; color: black; text-align: center; padding: 0px; font-size: 15px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #E1E1E1; }"
        "QPushButton:pressed { background-color: #C4C4C4; }"
    );
    #ifdef LOCAL_SERVER
        QIcon checkmarkIcon("./client/Ressources/check.svg");
    #elif defined(DISTANT_SERVER)
        QIcon checkmarkIcon(":/check.svg");
    #endif
    checkmarkButton->setIcon(checkmarkIcon);
    checkmarkButton->setIconSize(QSize(15, 15));

    buttonLayout->addWidget(checkmarkButton, 0, Qt::AlignRight);
    buttonLayout->addWidget(crossButton, 0, Qt::AlignRight);

    conversationsButtons_->addWidget(userJoined, 0, Qt::AlignCenter);
    
    innerWidgetBtn_->adjustSize();
    innerWidgetBtn_->updateGeometry();
    scrollArea_->widget()->updateGeometry();
    conversationsButtons_->update();

    connect(crossButton, &QPushButton::clicked, this, [this, userJoined, username]() {
        emit newMpStatus(0, username);
        userJoined->deleteLater();
    });

    connect(checkmarkButton, &QPushButton::clicked, this, [this, userJoined, username]() {
        emit newMpStatus(1, username);
        userJoined->deleteLater();
    });
}

void HomePage::recvRespJointRoom(int code, const std::string &nameroom) {
    if (code != 250) {
        std::cout << "error" << std::endl;
        return;
    }

    addConv(ConvType::RM, QString::fromStdString(nameroom));
}

void HomePage::recvRespJoinMp(int code, const std::string &username) {

    auto it = userWaitButtonMap_.find(username);
    if (it != userWaitButtonMap_.end()) {
        it->second->deleteLater();
    }
    if (code != 1) {
        return;
    }

    addConv(ConvType::PM, QString::fromStdString(username));
}

void HomePage::mpChangeStatus(int msgId, const std::string &status, unsigned int msgTimestamp) {
    actualMp.changeStatus(msgId, status, msgTimestamp);
}

void HomePage::mpDeleteMsg(int msgId) {
    actualMp.deleteMsg(msgId);
}

void HomePage::setNewConvUserSlot(std::string user) {
    bool isUserButtonExists = (userButtonMap_.find(user) != userButtonMap_.end());
    bool isUserWaitButtonExists = (userWaitButtonMap_.find(user) != userWaitButtonMap_.end());
    if (isUserButtonExists || isUserWaitButtonExists) {
        QMessageBox::information(this, "Contact", "Contact already exists!");
    } else {
        emit setNewConvUser(user);
        newConv(user);
    }
}
void HomePage::setNewConvRoomSlots(std::string room) {
    emit setNewConvRoomSlots(room);
}
