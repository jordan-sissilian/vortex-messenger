#include "main_window.h"

extern QButtonGroup *globalButtonGroup;

MainWindow::MainWindow(UserController *userController, QWidget *parent)
    : QMainWindow(parent), authPage(new AuthPage()), homePage(new HomePage()), userController(userController)
{
    globalButtonGroup->setExclusive(true);
    globalButtonGroup->addButton(new QPushButton(), 1);

    setWindowFlags(Qt::FramelessWindowHint);

    QWidget *centralWidget = new QWidget();
    centralWidget->setObjectName("centralWidget");
    centralWidget->setContentsMargins(0, 0, 0, 0);
    centralWidget->setStyleSheet("#centralWidget { background-color: white; border-radius: 10px;}"); 
    setCentralWidget(centralWidget);

    titleBar = new TitleBar(this);
    titleBar->resize(280, 25);

    bottomBorderWidget = new QWidget(this);
    bottomBorderWidget->setFixedHeight(10);
    bottomBorderWidget->setStyleSheet(
        "background-color: transparent; "
        "border-bottom: 2px solid transparent; "
        "padding: 0px;"
    );

    QHBoxLayout *layout = new QHBoxLayout(bottomBorderWidget);
    leftFrame = new QWidget();
    leftFrame->setFixedWidth(250);
    leftFrame->setStyleSheet("background-color: #F0F0F0; border-bottom-left-radius: 10px;");
    rightFrame = new QWidget();
    rightFrame->setStyleSheet("background-color: #F0F0F0; border-bottom-right-radius: 10px;");

    layout->addWidget(leftFrame);
    layout->addWidget(rightFrame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    bottomBorderWidget->setLayout(layout);

    titleBar->hide();
    bottomBorderWidget->hide();

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(authPage);
    mainLayout->addWidget(homePage);
    mainLayout->addWidget(bottomBorderWidget);

    authPage->hide();
    homePage->hide();

    QWidget *loadingWidget = new QWidget();
    QLabel *iconLabel = new QLabel(loadingWidget);
    #ifdef LOCAL_SERVER
        QPixmap pixmap("./client/Ressources/logo.png");
    #elif defined(DISTANT_SERVER)
        QPixmap pixmap(":/logo.png");
    #endif
    iconLabel->setPixmap(pixmap);
    iconLabel->setScaledContents(true);
    iconLabel->setFixedSize(150, 150);

    QVBoxLayout *loadingLayout = new QVBoxLayout(loadingWidget);
    loadingLayout->addWidget(iconLabel);
    loadingLayout->setAlignment(Qt::AlignCenter);
    loadingWidget->setLayout(loadingLayout);
    loadingWidget->setStyleSheet("background-color: white;");

    mainLayout->addWidget(loadingWidget);
    authPage->resize(280, 430);

    setFixedSize(280, 430);

    connect(userController, &UserController::connectToServer, this, [this, loadingWidget] {
        loadingWidget->hide();
        authPage->show();
        setFixedSize(280, 430);
        titleBar->resize(280, 25);
        titleBar->setStyleSheet("background-color: white; border-radius: 10px;");
        titleBar->setStyle(0);

        titleBar->show();
        bottomBorderWidget->show();
    });

    connect(authPage, &AuthPage::LoginOk, this, [this, loadingWidget] {
        homePage->show();
        authPage->hide();
        loadingWidget->hide();
        setFixedSize(1280, 720);
        resize(1280, 720);
        setMinimumSize(800, 720);
        titleBar->resize(1280, 25);
        titleBar->setStyleSheet("background-color: white; border-radius: 10px;");
        titleBar->show();
        bottomBorderWidget->show();
        titleBar->setStyle(1);

        leftFrame->setFixedWidth(280);
        leftFrame->setStyleSheet("background-color: #F0F0F0; border-bottom-left-radius: 10px; border-right: 1px solid #C7C7C7;");
        rightFrame->setStyleSheet("background-color: white; border-bottom-right-radius: 10px;");


        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();
        move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);
    });

    connect(authPage, &AuthPage::newLogin, userController, &UserController::sendLogin);
    connect(authPage, &AuthPage::newRegister, userController, &UserController::sendRegister);
    connect(userController, &UserController::sendLoginCode, authPage, &AuthPage::responseLogin);
    connect(userController, &UserController::sendRegisterCode, authPage, &AuthPage::responseRegister);
    connect(userController, &UserController::recvMpMsgEmit, homePage, &HomePage::recvMpMsg);
    connect(userController, &UserController::mpMsgEmit, homePage, &HomePage::mpMsg);
    connect(userController, &UserController::mpChangeStatusEmit, homePage, &HomePage::mpChangeStatus);
    connect(userController, &UserController::mpDeleteMsgEmit, homePage, &HomePage::mpDeleteMsg);
    connect(&homePage->actualMp, &PrivateMessageUi::sendMp, userController, &UserController::sendMpMsg);
    connect(&homePage->actualMp, &PrivateMessageUi::viewMsg, userController, &UserController::viewMsg);
    connect(homePage, &HomePage::setNewConvUser, userController, &UserController::joinMp);
    connect(userController, &UserController::setNewConv, homePage, &HomePage::newConv);
    connect(userController, &UserController::setNewUserJoin, homePage, &HomePage::newConvAccept);
    connect(homePage, &HomePage::newMpStatus, userController, &UserController::sendJointMpStatus);
    connect(userController, &UserController::setUserJoinStatus, homePage, &HomePage::recvRespJoinMp);

    setStyleSheet("background-color: transparent;");
}

MainWindow::~MainWindow() {
    delete authPage;
    delete homePage;
}

void MainWindow::handleNewConnection() {
    homePage->show();
}

void MainWindow::enterEvent(QEnterEvent *event) {
    titleBar->setColor();
    QMainWindow::enterEvent(event);
}

void MainWindow::leaveEvent(QEvent *event) {
    titleBar->unsetColor();
    QMainWindow::leaveEvent(event);
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        titleBar->showBtn();
        setMaximumSize(1280, 720);
        titleBar->isDraggable = true;
    }

    QMainWindow::changeEvent(event);
}