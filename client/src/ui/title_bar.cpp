#include "title_bar.h"

TitleBar::TitleBar(QWidget *parent) : QWidget(parent) {
    isDraggable = true;

    closeButton = new QPushButton();
    minimizeButton = new QPushButton();
    maximizeButton = new QPushButton();

    closeButton->setFixedSize(12, 12);
    minimizeButton->setFixedSize(12, 12);
    maximizeButton->setFixedSize(12, 12);

    closeButton->setStyleSheet("QPushButton { background-color: #D9534F; border: none; border-radius: 6px; } QPushButton:hover { background-color: #FF605C; }");
    minimizeButton->setStyleSheet("QPushButton { background-color: #E6A83D; border: none; border-radius: 6px; } QPushButton:hover { background-color: #FFBD44; }");
    maximizeButton->setStyleSheet("QPushButton { background-color: #24A838; border: none; border-radius: 6px; } QPushButton:hover { background-color: #28C940; }");

    connect(closeButton, &QPushButton::clicked, parent, &QWidget::close);
    connect(minimizeButton, &QPushButton::clicked, parent, &QWidget::showMinimized);
    connect(maximizeButton, &QPushButton::clicked, [this, parent]() {
        parent->showFullScreen();
        parent->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        isDraggable = false;
        hideBtn();
    });

    leftFrame = new QFrame(this);
    leftFrame->setFixedWidth(280);
    leftFrame->setStyleSheet("background-color: #F0F0F0; "
                            "border-top-left-radius: 10px; "
                            "border-top-right-radius: 0px; "
                            "border-bottom-left-radius: 0px; "
                            "border-bottom-right-radius: 0px; "
                            "border-right: none;"
                            "border-right: 1px solid #C7C7C7;");
    rightFrame = new QFrame(this);
    rightFrame->setStyleSheet("background-color: #FFFFFF; border-top-right-radius: 10px; ");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(8, 5, 8, 5);
    buttonLayout->setSpacing(8);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(minimizeButton);
    buttonLayout->addWidget(maximizeButton);

    buttonLayout->addStretch();

    leftFrame->setLayout(buttonLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(leftFrame);
    mainLayout->addWidget(rightFrame);

    setLayout(mainLayout);
    setFixedHeight(25);
}

void TitleBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isDraggable) {
        dragging = true;
        dragPosition = event->globalPosition().toPoint() - parentWidget()->parentWidget()->frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event) {
    if (dragging) {
        parentWidget()->parentWidget()->move(event->globalPosition().toPoint() - dragPosition);
    }
    QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void TitleBar::setColor() {
    closeButton->setStyleSheet("QPushButton { background-color: #D9534F; color: #D9534F; border-radius: 6px; } QPushButton:hover { background-color: #FF605C; color: #333333; }");
    minimizeButton->setStyleSheet("QPushButton { background-color: #E6A83D; color: #E6A83D; border-radius: 6px; } QPushButton:hover { background-color: #FFBD44; color: #333333; }");
    maximizeButton->setStyleSheet("QPushButton { background-color: #24A838; color: #24A838; border-radius: 6px; } QPushButton:hover { background-color: #28C940; color: #333333; }");
}

void TitleBar::unsetColor() {
    closeButton->setStyleSheet("QPushButton { background-color: #E0E0E0; border-radius: 6px; border: 0.5px solid #D3D3D3; color: #E0E0E0;}");
    minimizeButton->setStyleSheet("QPushButton { background-color: #E0E0E0; border-radius: 6px; border: 0.5px solid #D3D3D3; color: #E0E0E0;}");
    maximizeButton->setStyleSheet("QPushButton { background-color: #E0E0E0; border-radius: 6px; border: 0.5px solid #D3D3D3; color: #E0E0E0;}");
}

void TitleBar::hideBtn() {
    closeButton->hide();
    minimizeButton->hide();
    maximizeButton->hide();
}

void TitleBar::showBtn() {
    closeButton->show();
    minimizeButton->show();
    maximizeButton->show();
}

void TitleBar::setStyle(int style) {
    if (!style) {
        leftFrame->setStyleSheet("background-color: #F0F0F0; "
                            "border-top-left-radius: 10px; "
                            "border-top-right-radius: 10px; "
                            "border-bottom-left-radius: 0px; "
                            "border-bottom-right-radius: 0px; "
                            "border-right: none;"
                            "border-right: 0px solid #C7C7C7;");
        rightFrame->setStyleSheet("background-color: transparent;");
        maximizeButton->setEnabled(false);
        maximizeButton->setStyleSheet("QPushButton { background-color: #E0E0E0; border-radius: 6px; border: 0.5px solid #D3D3D3; color: #E0E0E0;}");
    } else if (style) {
        leftFrame->setStyleSheet("background-color: #F0F0F0; "
                            "border-top-left-radius: 10px; "
                            "border-top-right-radius: 0px; "
                            "border-bottom-left-radius: 0px; "
                            "border-bottom-right-radius: 0px; "
                            "border-right: none;"
                            "border-right: 1px solid #C7C7C7;");
        rightFrame->setStyleSheet("background-color: #FFFFFF; border-top-right-radius: 10px; ");
        maximizeButton->setEnabled(true);
        maximizeButton->setStyleSheet("QPushButton { background-color: #24A838; color: #24A838; border-radius: 6px; } QPushButton:hover { background-color: #28C940; color: #333333; }");
    }
}
