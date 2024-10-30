#pragma once

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QFrame>
#include <QCloseEvent>

#ifdef __linux__
#define CONTACT_MP_DIR_PATH "/usr/share/clientcryptomadnessvortex/dir_user/conv/mp/"
#elif __APPLE__
#define CONTACT_MP_DIR_PATH "/Users/Shared/ClientCryptoMadnessVortex/dir_user/conv/mp/"
#endif
#define SETTING_MP_PATH(user) std::string(CONTACT_MP_DIR_PATH + user + "/config")

class SettingsMessageUi : public QWidget {
    Q_OBJECT

public:
    explicit SettingsMessageUi(std::string nameUser, QWidget *parent = nullptr);
    ~SettingsMessageUi();

    std::string nameUser_;

private:
    void initTop();
    void initMiddle();
    void initBottom();

    QVBoxLayout *mainLayout;

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void windowClosed();

};
