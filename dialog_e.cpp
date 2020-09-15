#include "dialog_e.h"
#include "ui_dialog_e.h"

#include <QDebug>
#include <QSqlQuery>
#include <QFileDialog>

Q_DECLARE_METATYPE(Programmer);
Q_DECLARE_METATYPE(Programmer *);

Dialog_e::Dialog_e(Storage * storage, Language * t, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_e)
{
    ui->setupUi(this);
    this -> user = storage -> returnId();
    this -> change = t;
    this -> stor = storage;

    ui -> lineEdit -> setText(t -> name.c_str());
    ui -> lineEdit_2 -> setText(t -> type.c_str());
    ui -> lineEdit_3 -> setText(t -> author.c_str());
    ui -> lineEdit_4 -> setText(t -> photo.c_str());

    vector <Programmer> progs = this -> stor -> getProgrammersForLang(*change);

    for (int i = 0; i < progs.size(); i++) {
        QVariant qVariant;
        qVariant.setValue(progs[i]);
        QListWidgetItem * qListItem = new QListWidgetItem();
        qListItem -> setText(progs[i].name.c_str());
        qListItem -> setData(Qt::UserRole, qVariant);
        ui -> listProgs -> addItem(qListItem);

    }

    vector <Programmer> progs2 = this -> stor -> getAllProgrammer();
    vector <Programmer> progs3;

    for (int i = 0; i < progs2.size(); i++) {
        bool notGood = false;
        for (int j = 0; j < progs.size(); j++) {
            if (progs2[i].id == progs[j].id) {
                notGood = true;
                break;
            }
        }
        if (notGood == false) {
            Programmer l;
            l.id = progs2[i].id;
            l.name = progs2[i].name;
            l.stage = progs2[i].stage;
            l.date_of_start = progs2[i].date_of_start;

            progs3.push_back(l);
        }
    }

    for (int i = 0; i < progs3.size(); i++) {
        QVariant qVariant;
        qVariant.setValue(progs3[i]);
        QListWidgetItem * qListItem = new QListWidgetItem();
        qListItem -> setText(progs3[i].name.c_str());
        qListItem -> setData(Qt::UserRole, qVariant);
        ui -> listProgs_2 -> addItem(qListItem);
    }
}

Dialog_e::~Dialog_e() {
    qDebug() << "self destruct";
    delete ui;
}

void Dialog_e::on_buttonBox_accepted() {
    this -> str = ui -> lineEdit_4 -> text();
    change -> name = ui -> lineEdit -> text().toStdString();
    change -> type = ui -> lineEdit_2 -> text().toStdString();
    change -> author = ui -> lineEdit_3 -> text().toStdString();
    change -> user_id = this -> user;
    change -> photo = ui -> lineEdit_4 -> text().toStdString();
}

void Dialog_e::on_pushButton_clicked() {
    QList<QListWidgetItem *> item = ui -> listProgs_2 -> selectedItems();
    if (item.count() == 1) {
        QListWidgetItem * selectedItem = item.at(0);
        QVariant var = selectedItem -> data(Qt::UserRole);
        Programmer * prog = static_cast<Programmer *>(var.data());

        this -> stor -> insertProgrammerToLang(*change, *prog);

        QListWidgetItem * qStudentListItem = new QListWidgetItem();
        qStudentListItem -> setText(prog -> name.c_str());
        qStudentListItem -> setData(Qt::UserRole, var);
        ui -> listProgs -> addItem(qStudentListItem);

        QVariant var2 = selectedItem -> data(Qt::UserRole);
        Programmer * prog_del = static_cast<Programmer *>(var.data());

        int row_index = ui -> listProgs_2 -> row(selectedItem);
        ui -> listProgs_2 -> takeItem(row_index);

        delete  selectedItem;
    }
}

void Dialog_e::on_pushButton_2_clicked() {
    QList<QListWidgetItem *> item = ui -> listProgs -> selectedItems();
    if (item.count() == 1) {
        QListWidgetItem * selectedItem = item.at(0);
        QVariant var = selectedItem -> data(Qt::UserRole);
        Programmer * prog = static_cast<Programmer *>(var.data());

        QListWidgetItem * qStudentListItem = new QListWidgetItem();
        qStudentListItem -> setText(prog -> name.c_str());
        qStudentListItem -> setData(Qt::UserRole, var);
        ui -> listProgs_2 -> addItem(qStudentListItem);

        // QVariant var2 = selectedItem -> data(Qt::UserRole);
        // Programmer * prog_del = static_cast<Programmer *>(var.data());

        int row_index = ui -> listProgs -> row(selectedItem);
        ui -> listProgs -> takeItem(row_index);

        this -> stor -> removeProgrammerFromLang(*change, *prog);

        delete  selectedItem;
    }
}

void Dialog_e::on_listProgs_2_itemClicked(QListWidgetItem *item) {
    QVariant var = item -> data(Qt::UserRole);
    Programmer * prog = static_cast<Programmer *>(var.data());
}

void Dialog_e::on_listProgs_itemClicked(QListWidgetItem *item) {
    QVariant var = item -> data(Qt::UserRole);
    Programmer * prog = static_cast<Programmer *>(var.data());
}

void Dialog_e::on_pushButton_3_clicked() {
    QString fileName = QFileDialog::getOpenFileName(
         this,              // parent
         "Dialog Caption",  // caption
         "",                // directory to start with
         "PNG (*.png);;All Files (*)");  // file name filter
    this -> str = fileName;
    qDebug() << this -> str;
    ui -> lineEdit_4 -> setText(fileName);
}
