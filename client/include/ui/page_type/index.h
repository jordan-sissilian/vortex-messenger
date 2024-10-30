#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>

class IndexUi : public QWidget {
    Q_OBJECT

public:
    explicit IndexUi(QWidget *parent = nullptr);
    ~IndexUi() {};
};

#endif // INDEX_H