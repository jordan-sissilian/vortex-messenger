#ifndef ADD_CONVERSATION_WINDOW_HPP
#define ADD_CONVERSATION_WINDOW_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

class AddConversationWindow : public QWidget {
    Q_OBJECT

public:
    AddConversationWindow(QWidget *parent = nullptr);

private slots:
    void onSubmitButtonClicked();

signals:
    void setNewConvUser(std::string);
    void setNewConvRoom(std::string);

private:
    QLineEdit *inputField;
};

#endif // ADD_CONVERSATION_WINDOW_HPP
