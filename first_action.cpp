#include "first_action.h"
#include "ui_first_action.h"

#include <QDebug>

first_action::first_action(struct Status * stat, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::first_action)
{
    ui->setupUi(this);
    this -> stat = stat;
    this -> status = 1;
}

first_action::~first_action() {
    delete ui;
}

void first_action::on_pushButton_clicked() {
    this -> status = 0;
    this -> stat -> registration = 1;
    qDebug() << "REG";
    this -> close();
}

void first_action::on_pushButton_2_clicked() {
    this -> stat -> login = 1;
    this -> status = 0;
    qDebug() << "LOG";
    this -> close();
}

void first_action::on_pushButton_3_clicked() {
    this -> status = 1;
    qDebug() << "CANCEL";
    this -> close();
}
