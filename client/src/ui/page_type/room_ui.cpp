#include "room_ui.h"

RoomUi::RoomUi(std::string nameRoom, QWidget *parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    nameRoom_ = nameRoom;

    initaliseChatScrollArea();
    initaliseInfoRoom();
    initaliseSendMsg();
}

RoomUi::~RoomUi() { }

void RoomUi::initaliseChatScrollArea() {
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    scrollArea->setStyleSheet(
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

    QWidget *scrollWidget = new QWidget;
    scrollWidget->setStyleSheet("border: none;");
    scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(10);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
}

void RoomUi::initaliseInfoRoom() {
    QWidget *infoWidget = new QWidget;
    infoWidget->setFixedHeight(130);
    infoWidget->setObjectName("infoWidgets");
    infoWidget->setStyleSheet(" #infoWidgets { border-bottom: 1px solid #D3D3D3; }");

    QVBoxLayout *mainLayout1 = new QVBoxLayout(infoWidget);

    QHBoxLayout *hLayout1 = new QHBoxLayout();
    QLabel* conversationStarted = new QLabel("Conversation Started");
    conversationStarted->setStyleSheet("font-size: 12px; color: gray;");
    hLayout1->addWidget(conversationStarted);
    creationUserRoom = new QLabel("");
    creationUserRoom->setStyleSheet("font-size: 12px; color: gray;");
    hLayout1->addWidget(creationUserRoom, 0, Qt::AlignRight);
    mainLayout1->addLayout(hLayout1);

    QHBoxLayout *hLayout2 = new QHBoxLayout();
    creationDateRoom = new QLabel("");
    creationDateRoom->setStyleSheet("font-size: 12px; color: gray;");
    hLayout2->addWidget(creationDateRoom, 0, Qt::AlignRight);
    mainLayout1->addLayout(hLayout2);

    QHBoxLayout *hLayout3 = new QHBoxLayout();
    nameRoom = new QLabel(QString::fromStdString(nameRoom_));
    nameRoom->setStyleSheet("padding-left: 20px; font-size: 25px; color: black;");
    hLayout3->addWidget(nameRoom, 0, Qt::AlignLeft);
    mainLayout1->addLayout(hLayout3);

    QHBoxLayout *hLayout4 = new QHBoxLayout();
    QLabel *messageInfo = new QLabel("Communication is always encrypted.\nEverything you send and receive in this conversation is accessible to you and server room.\nPlease still be careful with whom you share sensitive information.");
    messageInfo->setStyleSheet("font-size: 10px; color: gray; padding-left: 5px;");
    hLayout4->addWidget(messageInfo);
    mainLayout1->addLayout(hLayout4);

    QPushButton *deleteButton = new QPushButton;
    deleteButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setStyleSheet(" border: none; ");
    #ifdef LOCAL_SERVER
        deleteButton->setIcon(QIcon("./client/Ressources/deleteImg.png"));
    #elif defined(DISTANT_SERVER)
        deleteButton->setIcon(QIcon(":/deleteImg.png"));
    #endif
    deleteButton->setIconSize(QSize(20, 20));
    deleteButton->setFixedSize(25, 25);
    hLayout4->addWidget(deleteButton, 0, Qt::AlignRight | Qt::AlignTop);

    connect(deleteButton, &QPushButton::clicked, [this]() {
        emit deleteRoom(nameRoom_);
    });

    scrollLayout->addWidget(infoWidget, 0, Qt::AlignTop);
}

void RoomUi::initaliseSendMsg() {
    QWidget *bottomWidget = new QWidget;
    bottomWidget->setFixedHeight(50);
    bottomWidget->setObjectName("bottomWidget");
    bottomWidget->setStyleSheet(" #bottomWidget { background-color: white; border-top: 1px solid #D3D3D3; }");

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(20, 0, 20, 0);

    QLineEdit *inputMsg = new QLineEdit;
    inputMsg->setFixedHeight(40);
    inputMsg->setStyleSheet(" border: none; ");
    inputMsg->setPlaceholderText("Send a message...");
    inputMsg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton *sendBtn = new QPushButton;
    sendBtn->setCursor(Qt::PointingHandCursor);
    sendBtn->setStyleSheet(" border: none; ");
    #ifdef LOCAL_SERVER
        sendBtn->setIcon(QIcon("./client/Ressources/sendMsgBtn.png"));
    #elif defined(DISTANT_SERVER)
        sendBtn->setIcon(QIcon(":/sendMsgBtn.png"));
    #endif
    sendBtn->setIconSize(QSize(40, 40));
    sendBtn->setFixedSize(45, 45);

    connect(sendBtn, &QPushButton::clicked, [this, inputMsg]() {
        emit sendMsg(nameRoom->text().toStdString(), inputMsg->text().toStdString());
        inputMsg->setText("");
    });

    bottomLayout->addWidget(inputMsg);
    bottomLayout->addWidget(sendBtn);
    mainLayout->addWidget(bottomWidget);
}


void RoomUi::setInfoRoom(const std::string& roomName, const std::string& dateCreate, const std::string& userCreate) {
    creationDateRoom->setText(QString::fromStdString(dateCreate));
    creationUserRoom->setText(QString::fromStdString(userCreate));
    nameRoom->setText(QString::fromStdString(roomName));
}

void RoomUi::setUserMsg(const std::string& nameUser, const std::string& msg) {
    QVBoxLayout *msgLayout = new QVBoxLayout();
    QLabel *userLabel = new QLabel(QString::fromStdString(nameUser));
    userLabel->setStyleSheet("font-size: 10px; color: black; padding: 0;");
    userLabel->setAlignment(Qt::AlignLeft);
    QLabel *msgLabel = new QLabel(QString::fromStdString(msg));
    int maxWidth = 450;
    msgLabel->setStyleSheet("background-color: #FD7014; font-size: 12px; color: white; padding: 10px; border-radius: 5px;");
    msgLabel->setMaximumWidth(maxWidth);
    QFontMetrics metrics(msgLabel->font());
    int textWidth = metrics.horizontalAdvance(msgLabel->text());
    if (textWidth > maxWidth) {
        msgLabel->setWordWrap(true);
        msgLabel->setFixedWidth(maxWidth);
    } else {
        msgLabel->setWordWrap(false);
    }
    int numLines = 0;
    int totalHeight = metrics.height() * 2;
    if (msgLabel->wordWrap()) {
        numLines = (textWidth / maxWidth) + 2;
        totalHeight = numLines * metrics.height();
    }
    msgLabel->setFixedHeight(totalHeight);
    msgLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    msgLayout->addWidget(userLabel, 0, Qt::AlignLeft);
    msgLayout->addWidget(msgLabel, 0, Qt::AlignLeft);
    msgLayout->setContentsMargins(20, 10, 20, 10);
    scrollLayout->addLayout(msgLayout);
}

void RoomUi::setMsg(const std::string& nameRoom, const std::string& msg) {
    QVBoxLayout *msgLayout = new QVBoxLayout();
    QLabel *userLabel = new QLabel(QString::fromStdString(nameRoom));
    userLabel->setStyleSheet("font-size: 10px; color: black; padding: 0;");
    userLabel->setAlignment(Qt::AlignCenter);
    QLabel *msgLabel = new QLabel(QString::fromStdString(msg));
    int maxWidth = 450;
    msgLabel->setStyleSheet("background-color: #E2E2E2; font-size: 12px; color: black; padding: 10px; border-radius: 5px;");
    msgLabel->setMaximumWidth(maxWidth);
    QFontMetrics metrics(msgLabel->font());
    int textWidth = metrics.horizontalAdvance(msgLabel->text());
    if (textWidth > maxWidth) {
        msgLabel->setWordWrap(true);
        msgLabel->setFixedWidth(maxWidth);
    } else {
        msgLabel->setWordWrap(false);
    }
    int numLines = 0;
    int totalHeight = metrics.height() * 2;
    if (msgLabel->wordWrap()) {
        numLines = (textWidth / maxWidth) + 2;
        totalHeight = numLines * metrics.height();
    }
    msgLabel->setFixedHeight(totalHeight);
    msgLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    msgLayout->addWidget(userLabel, 0, Qt::AlignRight); 
    msgLayout->addWidget(msgLabel, 0, Qt::AlignRight);
    msgLayout->setContentsMargins(20, 10, 20, 10);
    scrollLayout->addLayout(msgLayout);
}
