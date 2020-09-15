#include "registration.h"
#include "ui_registration.h"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

registration::registration(Storage * stor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registration)
{
    ui->setupUi(this);
    this -> stor = stor;
}

registration::~registration()
{
    delete ui;
}

void registration::on_buttonBox_accepted() {
    if (ui -> lineEdit_2 -> text() == ui -> lineEdit_3 -> text() && ui -> lineEdit_3 -> text().length() != 0 && ui -> lineEdit -> text().length() != 0) {
        QCryptographicHash hash(QCryptographicHash::Md5);
        QString password = ui -> lineEdit_2 -> text();
        hash.addData(password.toUtf8());
        QString hashStr(hash.result().toHex());
        int user_id = this -> stor -> insertNewUser(ui -> lineEdit -> text(), hashStr);
        this -> user = user_id;
        qDebug().noquote() << "ID:";
        qDebug() << this -> user;
        if (this -> user > 0) {
            QMessageBox::information(
                   this,
                   "Registrating done!",
                   "Congratulations! You are in DB of languages!");
        } else {
            QMessageBox::information(
                   this,
                   "Registrating uncompleted!",
                   "This username exists in DB now or f-pass and l-pass are conflicted!");
        }
    } else if (ui -> lineEdit_3 -> text().length() == 0 || ui -> lineEdit -> text().length() == 0) {
        QMessageBox::information(
               this,
               "Registrating uncompleted!",
               "Feel all fields!");
    } else {
        QMessageBox::information(
               this,
               "Registrating uncompleted!",
               "This username exists in DB now or f-pass and l-pass are conflicted!");
    }
    this -> status = 0;
}

void registration::on_lineEdit_3_textChanged(const QString &arg1) {
    if (arg1 != ui -> lineEdit_2 -> text()) {
        ui -> label_4 -> setText("Conflict of passwords!");
        ui -> label_4 -> setStyleSheet("color: red");
    } else {
        ui -> label_4 -> setText("");
    }
}

void registration::on_lineEdit_2_textChanged(const QString &arg1) {
    if (arg1 != ui -> lineEdit_3 -> text()) {
        ui -> label_4 -> setText("Conflict of passwords!");
        ui -> label_4 -> setStyleSheet("color: red");
    } else {
        ui -> label_4 -> setText("");
    }
}

int registration::getId() {
    return this -> user;
}
