#ifndef DIALOG_E_H
#define DIALOG_E_H

#include "mainwindow.h"

#include <QDialog>
#include <QListWidget>

namespace Ui {
class Dialog_e;
}

class Dialog_e : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_e(Storage * stor, Language * t, QWidget *parent = nullptr);
    ~Dialog_e();
    QString str;

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_listProgs_2_itemClicked(QListWidgetItem *item);

    void on_listProgs_itemClicked(QListWidgetItem *item);

    void on_pushButton_3_clicked();

private:
    Ui::Dialog_e *ui;
    Language * change;
    Storage * stor;
    int user;
};

#endif // DIALOG_E_H
