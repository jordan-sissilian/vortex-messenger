#include "private_message_ui.h"

PrivateMessageUi::PrivateMessageUi(std::string nameUser, QWidget *parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    nameUser_ = nameUser;

    initaliseTop();
    initaliseChatScrollArea();
    initaliseSendMsg();
}

PrivateMessageUi::~PrivateMessageUi() { }

void PrivateMessageUi::setNewMp(const std::string& username) {
    nameUser_ = username;
    textNameUser->setText(QString::fromStdString(nameUser_));

    QLayoutItem* item = mainLayout->takeAt(1); 
    if (item) {
        QWidget* widget = item->widget();
        if (widget) {
            delete widget;
        }
        delete item;
    }

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(
        "QScrollBar:vertical {"
        "    background: transparent;"
        "    width: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: transparent;"
        "    border-radius: 0px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0;"
        "    width: 0;"
        "}"
    );

    QWidget *scrollWidget = new QWidget;
    scrollWidget->setStyleSheet("border: none;");
    scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(10);
    scrollLayout->setAlignment(Qt::AlignBottom);

    scrollArea->setWidget(scrollWidget);
    mainLayout->insertWidget(1, scrollArea);
    mainLayout->update();

    std::time_t currentTimestamp = std::time(nullptr);
    emit viewMsg(username, currentTimestamp);
}

void PrivateMessageUi::initaliseTop() {
    QWidget *topWidget = new QWidget;
    topWidget->setFixedHeight(50);

    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(20, 0, 20, 0);
    topLayout->setSpacing(5);


    QPushButton *iconAvatar = new QPushButton();
    iconAvatar->setFixedSize(34, 34);
    iconAvatar->setStyleSheet("border-radius: 17px; background-color: #E8E8E8;");
    #ifdef LOCAL_SERVER
        QIcon settingIcon("./client/Ressources/avatar_ghost.svg");
    #elif defined(DISTANT_SERVER)
        QIcon settingIcon(":/avatar_ghost.svg");
    #endif
    iconAvatar->setIcon(settingIcon);
    iconAvatar->setIconSize(QSize(34, 34));
    iconAvatar->setEnabled(false);

    QSpacerItem* spacer5 = new QSpacerItem(5, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

    textNameUser = new QLabel(QString::fromStdString(nameUser_));
    textNameUser->setStyleSheet("background-color: transparent; color: black; font-weight: 550; font-size: 13px;");

    QLabel *labelTimeMsg = new QLabel; 
    #ifdef LOCAL_SERVER
        QIcon iconTimeMsg("./client/Ressources/recent.svg");
    #elif defined(DISTANT_SERVER)
        QIcon iconTimeMsg(":/recent.svg");
    #endif
    labelTimeMsg->setPixmap(iconTimeMsg.pixmap(13, 13));

    QLabel *timeMsg = new QLabel("Desactiver");
    timeMsg->setStyleSheet("color: black;");

    QPushButton *btnParam = new QPushButton();
    btnParam->setCursor(Qt::PointingHandCursor);
    btnParam->setFixedWidth(30);
    btnParam->setFixedHeight(30);
    btnParam->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnParam->setObjectName("btnParamsUser");
    btnParam->setStyleSheet(
        "#btnParamsUser { background-color: transparent; padding: 0px; font-size: 15px; border-radius: 5px; }"
        "#btnParamsUser:hover { background-color: #F6F6F6; }"
        "#btnParamsUser:pressed { background-color: #E8E8E8; }"
    );
    #ifdef LOCAL_SERVER
        QIcon addIcon("./client/Ressources/more.svg");
    #elif defined(DISTANT_SERVER)
        QIcon addIcon(":/more.svg");
    #endif
    btnParam->setIcon(addIcon);
    btnParam->setIconSize(QSize(20, 20));

    QObject::connect(btnParam, &QPushButton::clicked, [=]() {
        settingsMsg = new SettingsMessageUi(nameUser_);

        QObject::connect(settingsMsg, &SettingsMessageUi::windowClosed, [=]() {
            qDebug() << "SettingsMessageUi window closed! : " << nameUser_;
            //todo rafrachir les donner (message ephemer)
            settingsMsg.clear();
        });
        settingsMsg->show();
    });

    QSpacerItem* spacer15 = new QSpacerItem(15, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

    topLayout->addWidget(iconAvatar);
    topLayout->addSpacerItem(spacer5);
    topLayout->addWidget(textNameUser);
    topLayout->addStretch();
    topLayout->addWidget(labelTimeMsg);
    topLayout->addWidget(timeMsg);
    topLayout->addSpacerItem(spacer15);
    topLayout->addWidget(btnParam);

    mainLayout->addWidget(topWidget);
}

void PrivateMessageUi::initaliseChatScrollArea() {
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    scrollArea->setStyleSheet(
        "QScrollBar:vertical {"
        "    background: transparent;"
        "    width: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: transparent;"
        "    border-radius: 0px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0;"
        "    width: 0;"
        "}"
    );

    QWidget *scrollWidget = new QWidget;
    scrollWidget->setStyleSheet("border: none;");
    scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(10);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
}

void PrivateMessageUi::initaliseSendMsg() {
    bottomWidget = new QWidget;
    bottomWidget->setFixedHeight(50);
    bottomWidget->setObjectName("bottomWidget");
    bottomWidget->setStyleSheet("#bottomWidget { background-color: white; }");

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(20, 0, 20, 0);

    QLineEdit *inputMsg = new QLineEdit;
    inputMsg->setFixedHeight(34);
    inputMsg->setStyleSheet("border: none; color: #6E6E6E; border-radius: 17px; background-color: white; border: 1px solid #E8E8E8; padding-left: 15px; padding-right: 15px;");
    inputMsg->setPlaceholderText("Envoyer un message...");
    inputMsg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLabel *msgLimit = new QLabel("0/190");
    msgLimit->setFixedHeight(34);
    msgLimit->setFixedWidth(40);
    msgLimit->setStyleSheet("color: gray;");
    msgLimit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(inputMsg, &QLineEdit::textChanged, [this, inputMsg, msgLimit]() {
        int charCount = inputMsg->text().length();
        msgLimit->setText(QString::number(charCount) + "/190");

        if (charCount > 190) {
            msgLimit->setStyleSheet("color: red;");
        } else {
            msgLimit->setStyleSheet("color: gray;");
        }
    });

    connect(inputMsg, &QLineEdit::returnPressed, [this, inputMsg, msgLimit]() {
        int charCount = inputMsg->text().length();
        if (charCount <= 190) {
            std::string message = inputMsg->text().toStdString();
            if (!message.empty()) {
                emit sendMp(nameUser_, message);
                inputMsg->setText("");
                msgLimit->setText("0/190");
                msgLimit->setStyleSheet("color: gray;");
            }
        }
    });

    bottomLayout->addWidget(inputMsg);
    bottomLayout->addWidget(msgLimit);
    mainLayout->addWidget(bottomWidget);
}

void PrivateMessageUi::createMessageWidget(QVBoxLayout* scrollLayout, Message msg, Qt::Alignment alignment) {
    QVBoxLayout* messageContainerLayout = new QVBoxLayout();
    messageContainerLayout->setContentsMargins(0, 0, 0, 0);
    messageContainerLayout->setSpacing(0);

    QHBoxLayout* msgLayout = new QHBoxLayout();
    QLabel* msgLabel = new QLabel(QString::fromStdString(msg.content));
    int maxWidth = 450;

    QString bgColor, color;
    if (alignment == Qt::AlignLeft) {
        bgColor = QString("#E2E2E2");
        color = QString("black");
    } else {
        bgColor = QString("#FD7014");
        color = QString("white");
    }

    msgLabel->setStyleSheet("background-color: " + bgColor + "; font-size: 12px; color: " + color + "; padding: 10px; border-radius: 8px;");
    msgLabel->setMaximumWidth(maxWidth);

    QFontMetrics metrics(msgLabel->font());
    int textWidth = metrics.horizontalAdvance(msgLabel->text());

    if (textWidth >= maxWidth) {
        msgLabel->setWordWrap(true);
        msgLabel->setFixedWidth(maxWidth);
    } else {
        msgLabel->setWordWrap(false);
    }

    int totalHeight = metrics.height() * 2;
    if (msgLabel->wordWrap()) {
        int numLines = (textWidth / maxWidth) + 2;
        totalHeight = numLines * metrics.height();
    }

    msgLabel->setFixedHeight(totalHeight);
    msgLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    msgLayout->addWidget(msgLabel, 0, alignment);
    msgLayout->setContentsMargins(20, 10, 20, 0);
    msgLayout->addSpacing(5);

    QHBoxLayout* infoLayout = new QHBoxLayout();
    if (msg.isFromMe) {
        QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        infoLayout->addSpacerItem(spacer);
    }

    QString statusStr;
    switch (msg.status) {
        case MessageStatus::Sent:
            statusStr = "Envoyer.";
            break;
        case MessageStatus::Received:
            statusStr = "Recu.";
            break;
        case MessageStatus::Read:
            statusStr = "Lu.";
            break;
        default:
            statusStr = "";
            break;
    }
    QLabel* status = new QLabel(statusStr);
    status->setStyleSheet("font-size: 10px; color: grey;");

    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(msg.timestamp));
    QString formattedTime = dateTime.toString("HH:mm");
    QLabel* timestampLabel = new QLabel(formattedTime);
    timestampLabel->setStyleSheet("font-size: 10px; color: grey;");

    QLabel* timeDurationLabel = new QLabel("");
    if (msg.ephemeralDuration != -1) {
        timeDurationLabel->setText(QString::number(msg.ephemeralDuration / 60) + " min");
        timeDurationLabel->setStyleSheet("font-size: 10px; color: grey;");
        if (msg.isFromMe) {
            timeDurationLabel->setStyleSheet("font-size: 10px; color: transparent;");
        }
    }
    if (msg.isFromMe) {
        infoLayout->addWidget(timeDurationLabel, 0, alignment);
        infoLayout->addSpacing(5);
        infoLayout->addWidget(status, 0, alignment);
        infoLayout->addSpacing(5);
    }
    infoLayout->addWidget(timestampLabel, 0, alignment);
    infoLayout->setContentsMargins(20, 3, 20, 10);
    if (!msg.isFromMe) {
        infoLayout->addSpacing(5);
        infoLayout->addWidget(timeDurationLabel, 0, alignment);
        QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        infoLayout->addSpacerItem(spacer);
    }

    messageContainerLayout->addLayout(msgLayout);
    messageContainerLayout->addLayout(infoLayout);

    msgQT msgStruct{messageContainerLayout, msgLabel, status, timestampLabel, timeDurationLabel, msg.timestamp, msg.isFromMe};
    labelList.append(qMakePair(msg.id, msgStruct));

    scrollLayout->addLayout(messageContainerLayout);
}

void PrivateMessageUi::setUserMsg(const std::string& nameMp, int msgId) {
    const QString& directoryPath = CONTACT_MP_DIR_PATH;
    const QString& user = QString::fromStdString(nameUser_);
    QString messageFilePath = directoryPath + user + "/messages.bin";
    MessageManager msgManager(messageFilePath.toStdString());
    Message msg = msgManager.getMessage(msgId, false);

    createMessageWidget(scrollLayout, msg, Qt::AlignLeft);

    QScrollArea* scrollArea = static_cast<QScrollArea*>(mainLayout->itemAt(1)->widget());
    scrollArea->update();

    QTimer::singleShot(10, [this, scrollArea]() {
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    });
}

void PrivateMessageUi::setMsg(int msgId) {
    const QString& directoryPath = CONTACT_MP_DIR_PATH;
    const QString& user = QString::fromStdString(nameUser_);
    QString messageFilePath = directoryPath + user + "/messages.bin";
    MessageManager msgManager(messageFilePath.toStdString());
    Message msg = msgManager.getMessage(msgId, true);

    createMessageWidget(scrollLayout, msg, Qt::AlignRight);

    QScrollArea* scrollArea = static_cast<QScrollArea*>(mainLayout->itemAt(1)->widget());
    scrollArea->update();

    QTimer::singleShot(10, [this, scrollArea]() {
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());
    });
}

void PrivateMessageUi::changeStatus(int msgId, const std::string& status, unsigned int msgTimestamp) {
    /*if (status == "Read") { Todo
        std::time_t timestampRecive = static_cast<std::time_t>(msgTimestamp);
        
        for (const auto& entry : labelList) {
            if (entry.second.isFromMe && entry.second.status->text() != "Read" && entry.second.timestamp < timestampRecive) {
                entry.second.status->setText(QString::fromStdString(status));
                entry.second.timeDurationLabel->setStyleSheet("font-size: 10px; color: gray;");
                scrollLayout->update();
            }
        }
        return;
    }*/
    for (const auto& entry : labelList) {
        if (entry.first == msgId) {
            entry.second.status->setText(QString::fromStdString(status));
            scrollLayout->update();
            break;
        }
    }
    scrollLayout->update();
}

void PrivateMessageUi::deleteMsg(int msgId) {
    for (int i = labelList.size() - 1; i >= 0; --i) {
        if (labelList[i].first == msgId) {
            labelList[i].second.deleteMsg();
            labelList[i].second.messageContainerLayout->deleteLater();
            labelList.removeAt(i);
            scrollLayout->update();
            return;
        }
    }
}

void PrivateMessageUi::processFiles() {
    const QString& directoryPath = CONTACT_MP_DIR_PATH;
    const QString& user = QString::fromStdString(nameUser_);

    QString messageFilePath = directoryPath + user + "/messages.bin";
    if (!QFile::exists(messageFilePath)) {
        qDebug() << "Le fichier binaire des messages n'existe pas.";
        return;
    }

    MessageManager msgManager(messageFilePath.toStdString());
    std::vector<Message> messages = msgManager.readMessages();

    for (const auto& msg : messages) {
        if (msg.isFromMe) {
            setMsg(msg.id);
        } else {
            setUserMsg(nameUser_, msg.id);
        }
    }
}

