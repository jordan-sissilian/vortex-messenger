#include "auth_page.h"

AuthPage::AuthPage(QWidget *parent) : QFrame(parent), switchButtonStatus(false) {
    this->setStyleSheet("background-color: #F0F0F0;");

    QLabel *iconLabel = new QLabel(this);
    #ifdef LOCAL_SERVER
        QPixmap pixmap("./client/Ressources/logo.png");
    #elif defined(DISTANT_SERVER)
        QPixmap pixmap(":/logo.png");
    #endif
    iconLabel->setPixmap(pixmap);
    iconLabel->setScaledContents(true);
    iconLabel->setFixedSize(100, 100); 

    QLabel *title = new QLabel("Vortex Messenger", this);
    title->setStyleSheet("font-size: 22px; color: black; font-weight: semibold;");

    QLabel *description = new QLabel("Messagerie Encrypter", this);
    description->setStyleSheet("font-size: 12px; color: rgba(0, 0, 0, 0.7);");
    description->setAlignment(Qt::AlignCenter);

    QVBoxLayout *titleLayout = new QVBoxLayout(this);
    titleLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    titleLayout->addWidget(title, 0, Qt::AlignCenter);
    titleLayout->addWidget(description, 0, Qt::AlignCenter);

    QWidget *titleWidget = new QWidget(this);
    titleWidget->setLayout(titleLayout);
    titleWidget->setStyleSheet("background-color: #F0F0F0;");


    QFrame *loginCard = new QFrame(this);

    switchButton = new QPushButton("Pas encore inscrit ? Inscrivez-vous.", loginCard);
    switchButton->setStyleSheet(
            "QPushButton {"
            "background-color: transparent; color: rgba(0, 0, 0, 0.8); font-size: 14px; text-decoration: underline;"
            "}"
            "QPushButton:hover {"
            "background-color: transparent; color: rgba(0, 0, 0, 1); font-size: 14px; text-decoration: underline;"
            "}"
    );
    switchButton->setCursor(QCursor(Qt::PointingHandCursor));

    usernameInput = new QLineEdit(loginCard);
    usernameInput->setPlaceholderText("Nom D'utilisateur");
    usernameInput->setStyleSheet(R"(
        QLineEdit {
            background-color: transparent;
            border: none;
            border-bottom: 0.5px solid rgba(0, 0, 0, 0.1);
            padding: 5px;
            font-size: 14px;
            color: black;
        }
        QLineEdit:focus {
            border-bottom: 0.5px solid rgba(0, 0, 0, 0.5);
            color: black;
        }
        QLineEdit::placeholder {
            color: rgba(0, 0, 0, 0.6); 
        }
    )");

    passwordInput = new QLineEdit(loginCard);
    passwordInput->setPlaceholderText("Mots de passe");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setStyleSheet(R"(
        QLineEdit {
            background-color: transparent;
            border: none;
            border-bottom: 0.5px solid rgba(0, 0, 0, 0.1);
            padding: 5px;
            font-size: 14px;
            color: black;
        }
        QLineEdit:focus {
            border-bottom: 0.5px solid rgba(0, 0, 0, 0.5);
            color: black;
        }
    )");

    sendButton = new QPushButton("Connexion", loginCard);
    sendButton->setStyleSheet(
            "QPushButton {"
            "background-color: transparent; color: rgba(0, 0, 0, 0.7); font-size: 16px;"
            "}"
            "QPushButton:hover {"
            "background-color: transparent; color: rgba(0, 0, 0, 1); font-size: 16px;"
            "}"
    );
    sendButton->setCursor(QCursor(Qt::PointingHandCursor));

    QVBoxLayout *cardLayout = new QVBoxLayout(loginCard);
    cardLayout->addWidget(usernameInput);
    cardLayout->addWidget(passwordInput);
    cardLayout->addSpacing(30);
    cardLayout->addWidget(sendButton, 0, Qt::AlignCenter);
    cardLayout->setAlignment(Qt::AlignCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addWidget(titleWidget, 0, Qt::AlignCenter);
    mainLayout->addWidget(loginCard, 0, Qt::AlignCenter);
    mainLayout->addWidget(switchButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    setLayout(mainLayout);

    connect(sendButton, &QPushButton::clicked, this, [this] {
        if (switchButtonStatus) {
            handleRegister();
        } else {
            handleLogin();
        }
    });

    connect(switchButton, &QPushButton::clicked, this, [this] {
        switchButtonStatus = !switchButtonStatus;
        if (switchButtonStatus) {
            sendButton->setText("Inscription");
            switchButton->setText("Déjà inscrit ? Connectez-vous.");
        } else {
            sendButton->setText("Connexion");
            switchButton->setText("Pas encore inscrit ? Inscrivez-vous.");
        }
        usernameInput->setText("");
        passwordInput->setText("");
    });
}

void AuthPage::handleLogin() {
    QString username = usernameInput->text();
    QString password = passwordInput->text();

    if (username.isEmpty() || password.isEmpty()) {
        usernameInput->setText("");
        passwordInput->setText("");
        return;
    }
    emit newLogin(username.toStdString(), password.toStdString());
}

void AuthPage::handleRegister() {
    QString username = usernameInput->text();
    QString password = passwordInput->text();

    if (username.isEmpty() || password.isEmpty()) {
        usernameInput->setText("");
        passwordInput->setText("");
        return;
    }
    emit newRegister(username.toStdString(), password.toStdString());
}

AuthPage::~AuthPage() {
    delete switchButton;
    delete usernameInput;
    delete passwordInput;
    delete sendButton;
}

void AuthPage::responseLogin(int code) {
    if (code == 250) {
        emit LoginOk();
        usernameInput->setText("");
        passwordInput->setText("");
        this->close();
        return;
    } else {
        usernameInput->setText("");
        passwordInput->setText("");
        QMessageBox::warning(this, "Erreur de connexion", "Erreur : Identifiant ou mot de passe incorrect.");
        return;
    }
}

void AuthPage::responseRegister(int code) {
    if (code == 250) {
        switchButtonStatus = !switchButtonStatus;
        switchButton->setText("Pas encore inscrit ? Inscrivez-vous.");
        sendButton->setText("Connexion");
        usernameInput->setText("");
        passwordInput->setText("");
        QMessageBox::information(this, "Inscription", "Inscription réussie !");
        return;
    } else {
        usernameInput->setText("");
        passwordInput->setText("");
        QMessageBox::warning(this, "Erreur d'inscription", "Erreur: Échec de l'inscription");
        return;
    }
}