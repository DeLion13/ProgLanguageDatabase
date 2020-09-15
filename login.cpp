#include "login.h"
#include "ui_login.h"
#include "user.h"
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDebug>

LogIn::LogIn(Storage * stor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogIn)
{
    ui->setupUi(this);
    this -> stor_ = stor;
}

LogIn::~LogIn()
{
    delete ui;
}



void LogIn::on_buttonBox_accepted() {
    this -> user_id = -1;
    vector<User> users;
    QSqlQuery query("SELECT * FROM users");
    while (query.next()) {
       int id = query.value("id").toInt();
       string username = query.value("username").toString().toStdString();
       string password_hash = query.value("password_hash").toString().toStdString();

       User u;
       u.id = id;
       u.username = username;
       u.password_hash = password_hash;

       users.push_back(u);
    }

    bool isTrue = false;

        QCryptographicHash hash(QCryptographicHash::Md5);
        QString password = ui -> lineEdit_2 -> text();
        hash.addData(password.toUtf8());
        QString hashStr(hash.result().toHex());

        for (int i = 0; i < users.size(); i++) {
            if (ui -> lineEdit -> text().toStdString() == users[i].username && hashStr.toStdString() == users[i].password_hash) {
                isTrue = true;
                this -> user_id = users[i].id;
                break;
            }
        }

        if (!isTrue) {
            QMessageBox::information(
                   this,
                   "Logging In",
                   "False username or password!");
        } else {
            QMessageBox::information(
                   this,
                   "Logging In",
                   "Successful entry!");
        }
}


int LogIn::getUserId() {
    qDebug() << this -> user_id;
    return  this -> user_id;
}
