#include "dialog.h"
#include "ui_dialog.h"

#include <QDebug>
#include <QFileDialog>


Dialog::Dialog(Language * t, int user_id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this -> user = user_id;
    this -> str = "NULL";

    change = t;
}

Dialog::~Dialog()
{

    delete ui;
}

void Dialog::on_buttonBox_accepted() {
    change -> name = ui -> lineEdit -> text().toStdString();
    change -> type = ui -> lineEdit_2 -> text().toStdString();
    change -> author = ui -> lineEdit_3 -> text().toStdString();
    change -> user_id = this -> user;
    change -> photo = this -> str.toStdString();
}

void Dialog::on_pushButton_clicked() {
    QString fileName = QFileDialog::getOpenFileName(
         this,              // parent
         "Dialog Caption",  // caption
         "",                // directory to start with
         "PNG (*.png);;All Files (*)");  // file name filter
    if (fileName.length() != 0) {
        this -> str = fileName;
        qDebug() << this -> str;
        ui -> lineEdit_4 -> setText(fileName);
    }
}
