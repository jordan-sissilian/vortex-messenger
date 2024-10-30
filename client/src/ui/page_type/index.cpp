#include "index.h"

IndexUi::IndexUi(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *iconLabel = new QLabel(this);
    #ifdef LOCAL_SERVER
        QPixmap pixmap("./client/Ressources/logo.png");
    #elif defined(DISTANT_SERVER)
        QPixmap pixmap(":/logo.png");
    #endif
    iconLabel->setPixmap(pixmap);
    iconLabel->setScaledContents(true);
    iconLabel->setFixedSize(100, 100); 

    QLabel *topLabel = new QLabel("Bienvenue sur Vortex Messenger", this);
    topLabel->setStyleSheet(
        "font-size: 15px;"
        "font-weight: bold;"
        "color: #333;"
    );

    QLabel *bottomLabel = new QLabel("Envoyez et recevez des messages de manière sécurisée.", this);
    bottomLabel->setStyleSheet(
        "font-size: 13px;"
        "color: #A0A0A0;" 
    );

    mainLayout->addStretch();
    mainLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(topLabel, 0, Qt::AlignCenter);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(bottomLabel, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    setLayout(mainLayout);
}
