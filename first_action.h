#ifndef FIRST_ACTION_H
#define FIRST_ACTION_H

#include <QDialog>
#include "user.h"

namespace Ui {
class first_action;
}

class first_action : public QDialog
{
    Q_OBJECT

public:
    explicit first_action(struct Status * stat, QWidget *parent = nullptr);
    ~first_action();
    int status = 0;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::first_action *ui;
    struct Status * stat;
};

#endif // FIRST_ACTION_H
