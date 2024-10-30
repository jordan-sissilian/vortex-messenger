#ifndef TITLEBAR_HPP
#define TITLEBAR_HPP

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QFrame>

class TitleBar : public QWidget {
    Q_OBJECT

public:
    TitleBar(QWidget *parent = nullptr);
    ~TitleBar() {};

    void setColor();
    void unsetColor();
    void hideBtn();
    void showBtn();
    void setStyle(int style);

    bool isDraggable;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool dragging = false;
    QPoint dragPosition;

    QPushButton *closeButton;
    QPushButton *minimizeButton;
    QPushButton *maximizeButton;

    QFrame *leftFrame;
    QFrame *rightFrame;
};

#endif // TITLEBAR_HPP