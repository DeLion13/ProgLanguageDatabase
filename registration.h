#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class registration;
}

class registration : public QDialog
{
    Q_OBJECT

public:
    explicit registration(Storage * stor, QWidget *parent = nullptr);
    int getId();
    ~registration();

    int status = 1;

private slots:
    void on_buttonBox_accepted();

    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

private:
    Ui::registration *ui;
    Storage * stor;
    int user;
};

#endif // REGISTRATION_H
