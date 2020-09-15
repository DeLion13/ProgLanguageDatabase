#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class LogIn;
}

class LogIn : public QDialog
{
    Q_OBJECT

public:
    explicit LogIn(Storage * stor, QWidget *parent = nullptr);
    ~LogIn();
    int getUserId();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::LogIn *ui;
    Storage * stor_;
    int user_id;
};

#endif // LOGIN_H
