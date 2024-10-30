#ifndef ROOMUI_H
#define ROOMUI_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QFontMetrics>

class RoomUi : public QWidget {
    Q_OBJECT

public:
    explicit RoomUi(std::string nameRoom, QWidget *parent = nullptr);
    ~RoomUi();

private:
    void initaliseChatScrollArea();
    void initaliseInfoRoom();
    void initaliseSendMsg();

    QVBoxLayout *mainLayout;
    QVBoxLayout *scrollLayout;

    QLabel *creationDateRoom;
    QLabel *creationUserRoom;
    QLabel *nameRoom;
    std::string nameRoom_;

signals:
    void sendMsg(const std::string& nameRoom, const std::string& msg);
    void deleteRoom(const std::string& name);

public slots:
    void setInfoRoom(const std::string& nameRoom, const std::string& dateCreate, const std::string& userCreate);
    void setUserMsg(const std::string& nameRoom, const std::string& msg);
    void setMsg(const std::string& nameRoom, const std::string& msg);
};

#endif // ROOMUI_H
