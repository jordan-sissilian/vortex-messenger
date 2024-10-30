#include "add_conversation_window.h"

AddConversationWindow::AddConversationWindow(QWidget *parent)
    : QWidget(parent) {

    setWindowTitle("Ajouter une Conversation");
    setFixedSize(500, 180);
    setStyleSheet("background-color: white; border-radius: 5px;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    QLabel *middleText = new QLabel("Nouvelle Conversation", this);
    middleText->setStyleSheet("font: 18px;");
    middleText->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(middleText);

    QLabel *bottomText = new QLabel("Pour initialiser un MP avec l'utilisateur spécifié @NomUtilisateur.\n\n Pour créer/rejoindre une salle #NomSalle.\nSi votre salle nécessite un mot de passe, utilisez #NomSalle:Password.", this);
    bottomText->setStyleSheet("color: gray; font: 13px;");
    bottomText->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(bottomText);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputField = new QLineEdit(this);
    inputField->setPlaceholderText("@Username or #Room:password");
    inputField->setFixedHeight(23);
    inputField->setStyleSheet(
        "QLineEdit {"
        "    border: 1px solid gray;"
        "    border-radius: 5px;"
        "    padding: 2px;"
        "    font-size: 12px;"
        "    text-align: center;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #B0B0B0;"
        "    font-style: italic;"
        "}"
    );
    inputField->setAlignment(Qt::AlignCenter);
    inputLayout->addWidget(inputField);

    QPushButton *submitButton = new QPushButton("+", this);
    submitButton->setCursor(Qt::PointingHandCursor);
    submitButton->setFixedWidth(45);
    submitButton->setFixedHeight(23);
    submitButton->setObjectName("btnAdd");
    submitButton->setStyleSheet(
        "#btnAdd { background-color: #E2E2E2; color: black; text-align: center; padding: 0px; font-size: 15px; border-radius: 5px; }"
        "#btnAdd:hover { background-color: #F1F1F1; }"
        "#btnAdd:pressed { background-color: #F6F6F6; color: rgba(0, 0, 0, 0.3);}"
    );
    inputLayout->addWidget(submitButton);
    mainLayout->addLayout(inputLayout);

    connect(submitButton, &QPushButton::clicked, this, &AddConversationWindow::onSubmitButtonClicked);
}

void AddConversationWindow::onSubmitButtonClicked() {
    QString inputText = inputField->text();
    std::string inputUsernameOrRoom = inputText.mid(1).toStdString();

    if (inputText.startsWith("@")) {
        emit setNewConvUser(inputUsernameOrRoom);
        close();
    } else if (inputText.startsWith("#")) {
        emit setNewConvRoom(inputUsernameOrRoom);    
        close();
    } else {
        QMessageBox::warning(this, "Entrée invalide", "L'entrée doit commencer par @(utilisateur) ou #(salle).");
    }

    inputField->clear();
}
