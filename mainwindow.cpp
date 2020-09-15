#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xml_storage.h"
#include "sqlite_storage.h"
#include "dialog.h"
#include "dialog_e.h"
#include "first_action.h"
#include "login.h"
#include "registration.h"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QBitmap>
#include <QSqlError>

Q_DECLARE_METATYPE(Language);
Q_DECLARE_METATYPE(Language *);
Q_DECLARE_METATYPE(Programmer);
Q_DECLARE_METATYPE(Programmer *);
Q_DECLARE_METATYPE(User);
Q_DECLARE_METATYPE(User *);

User getUserById(int id) {
    vector<User> users;

    QSqlQuery query("SELECT * FROM users");
    while (query.next()) {
       int id_v = query.value("id").toInt();
       string username = query.value("username").toString().toStdString();
       string password_hash = query.value("password_hash").toString().toStdString();
       User u;
       u.id = id_v;
       u.username = username;
       u.password_hash = password_hash;

       users.push_back(u);
    }

    for (int i = 0; i < users.size(); i++) {
        if (id == users[i].id) {
            qDebug() << users[i].id;
            qDebug() << users[i].username.c_str();
            qDebug() << users[i].password_hash.c_str();
            User us;
            us.id = users[i].id;
            us.username = users[i].username.c_str();
            us.password_hash = users[i].password_hash.c_str();
            return  us;
        }
    }
}

Language copy(Language * l) {
    Language l_copy;
    l_copy.id = l -> id;
    l_copy.name = l -> name;
    l_copy.type = l -> type;
    l_copy.author = l -> author;
    l_copy.user_id = l -> user_id;
    l_copy.photo = l -> photo;

    return l_copy;
}

Programmer copy_p(Programmer * l) {
    Programmer l_copy;
    l_copy.id = l -> id;
    l_copy.name = l -> name;
    l_copy.stage = l -> stage;
    l_copy.date_of_start = l -> date_of_start;

    return l_copy;
}

void getCode_Qr(QString website, const char * slot, QObject * parent)
{
    QString text = QUrl::toPercentEncoding(website);
    QNetworkAccessManager * manager = new QNetworkAccessManager(parent);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), parent, slot);
    QUrl url("http://api.qrserver.com/v1/create-qr-code/?data=" + text + "&size=180x180");
    QNetworkRequest request(url);
    manager->get(request);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui -> setupUi(this);

    this -> storage_ = nullptr;

    connect(ui -> actionOpen_storage, &QAction::triggered, this, &MainWindow::onOld);
    connect(ui -> actionExit, &QAction::triggered, this, &MainWindow::exit_);
    connect(ui -> actionLogOut, &QAction::triggered, this, &MainWindow::logOut);
    connect(ui -> actionExport_to_XML, &QAction::triggered, this, &MainWindow::exportToXml);
    connect(ui -> actionImport_from_XML, &QAction::triggered, this, &MainWindow::importFromXml);

    this -> setFixedSize(size());
    this -> count_at_page = 3;

    ui -> pushButton_2 -> setDisabled(true);
    ui -> pushButton_3 -> setDisabled(true);

    ui -> spinBox -> setDisabled(true);
    ui -> lineEdit -> setDisabled(true);

    ui -> label_12 -> setFixedSize(21, 16);

    QPixmap pm("/Users/dd26/Projects/progbase3/src/data/photo/find.png");
    QSize pictureSize = pm.size();
    ui->label_12->setPixmap(pm);
    ui->label_12->setScaledContents(true);
    QSize lableSize = ui->label_12->size();
    pictureSize.scale(lableSize, Qt::KeepAspectRatio);
    ui->label_12->setFixedSize(pictureSize.width(), pictureSize.height());
    ui -> actionExport_to_XML -> setDisabled(true);
    ui -> actionImport_from_XML -> setDisabled(true);

    ui -> addButton -> setDisabled(true);
    ui -> editButton -> setDisabled(true);
    ui -> removeButton -> setDisabled(true);
    ui -> actionLogOut -> setDisabled(true);

    qDebug() << "ctor()";
}

MainWindow::~MainWindow() {
    qDebug() << "~ctor()";
    if (this -> storage_ != nullptr) {
        qDebug() << "Deleting of storage";
        this -> storage_ -> close();
        delete storage_;
    }
    delete ui;
}

void MainWindow::onOld() {
    if (this -> storage_ == nullptr) {
        QString fileName = QFileDialog::getOpenFileName(
             this,              // parent
             "Dialog Caption",  // caption
             "",                // directory to start with
             "SQL (*.sqlite);;All Files (*)");  // file name filter

        qDebug() << fileName;
        if (this -> storage_ != nullptr) {
            ui -> listWidget -> clear();
            delete storage_;
        }

        //XmlStorage * xml_ptr = new XmlStorage(fileName.toStdString());
        SqliteStorage * sql_ptr = new SqliteStorage(fileName.toStdString());
        this -> storage_ = sql_ptr;
        bool opened = false;
        struct Status * stat = new struct Status;
        bool cool = false;
        int status = 0;
        if (fileName != "") {
            opened = this -> storage_ -> open();
            do {
                first_action * first = new first_action(stat);
                first -> setWindowTitle("DB opening");
                first -> exec();
                status = first -> status;
                delete  first;
                if (status == 1) {
                    break;
                }

                if (stat -> login == 1) {
                    if (opened) {
                        cool = this -> storage_ -> login_pass();
                        break;
                    }
                    stat -> login = 0;
                }
                if (stat -> registration == 1) {
                    if (opened) {
                        registration * reg = new registration(this -> storage_);
                        reg -> setWindowTitle("Registration");
                        reg -> exec();
                        status = reg -> status;
                        qDebug() << status;
                        delete  reg;
                    }
                    stat -> registration = 0;
                }
            } while (status != 1);
        }


        if (cool) {
            vector<Language> langs = this -> storage_ -> getYourLangs(ui -> spinBox -> text().toInt(), this -> count_at_page, ui -> lineEdit -> text());

            for (int i = 0; i < langs.size(); i++) {
                QVariant qVariant;
                qVariant.setValue(langs[i]);
                QListWidgetItem * qStudentListItem = new QListWidgetItem();
                qStudentListItem -> setText(langs[i].name.c_str());
                qStudentListItem -> setData(Qt::UserRole, qVariant);
                ui -> listWidget -> addItem(qStudentListItem);
            }
    //-------------------------
            this -> size_o_stor = this -> storage_ -> getSizeOfStor(ui -> lineEdit -> text());

            float size_1 = (float)this -> size_o_stor / (float)this -> count_at_page;
            int size_2 = (int)this -> size_o_stor / (int)this -> count_at_page;

            qDebug() << (int)size_1;

            int size = 0;

            if ((float)size_2 <= size_1) {
                if ((float)size_2 == (float)size_1 && size_1 != 0) {
                    size = size_2;
                } else {
                    size = size_2 + 1;
                }
            } else {
                size = size_2 + 2;
            }

            this -> count_of_pages = size;
            qDebug() << "!!!";
            qDebug() << this -> count_of_pages;

            ui -> label_11 -> setText(QString::number(size));
            ui -> spinBox -> setValue(1);
            ui -> spinBox -> setMinimum(1);
            ui -> spinBox -> setMaximum(this -> count_of_pages);
            ui -> spinBox -> setDisabled(false);
            if (size != 1) {
                ui -> pushButton_3 -> setDisabled(false);
            }
            ui -> lineEdit -> setDisabled(false);
            ui -> actionExport_to_XML -> setDisabled(false);
            ui -> actionImport_from_XML -> setDisabled(false);
            ui -> addButton -> setDisabled(false);
            ui -> editButton -> setDisabled(false);
            ui -> removeButton -> setDisabled(false);
            ui -> actionLogOut -> setDisabled(false);
    //-------------------------
            User u = getUserById(this -> storage_ -> returnId());

            ui -> label_7 -> setText(u.username.c_str());
        } else {
            this -> storage_ = nullptr;
            delete sql_ptr;
        }
    } else {
        QMessageBox::information(
               this,
               "Not ok",
               "You logged in now!");
    }
}

void MainWindow::on_editButton_clicked() {
    QList<QListWidgetItem *> item = ui -> listWidget -> selectedItems();
    if (item.count() == 1) {
        QListWidgetItem * selectedItem = item.at(0);
        QVariant var = selectedItem -> data(Qt::UserRole);
        Language * lang = static_cast<Language *>(var.data());

        Dialog_e * dialog =  new Dialog_e(this -> storage_, lang);
        dialog -> setWindowTitle("Edit");
        int status = dialog -> exec();
        QString & copy_str = dialog -> str;

        if (status == 1) {
            QString q = copy_str;
            delete  dialog;
            lang -> photo = q.toStdString();

            if (lang -> name.length() != 0 && lang -> type.length() != 0 && lang -> author.length() != 0 && lang -> photo.length() != 0) {
                var.setValue(copy(lang));
                selectedItem -> setText(lang -> name.c_str());
                selectedItem -> setData(Qt::UserRole, var);
                this -> storage_ -> updateLanguage(copy(lang));
            } else {
                QMessageBox::information(
                       this,
                       "Ooops...",
                       "Ooops... Write at all fields!");
            }
        }
    }
}

void MainWindow::on_addButton_clicked() {
    if (this -> storage_ != nullptr) {
        Language lang;
        Dialog dialog(&lang, this -> storage_ -> returnId());
        dialog.setWindowTitle("Add");
        int status = dialog.exec();
        QString photo = dialog.str;
        if (status == 1) {
            if (lang.name.length() != 0 && lang.type.length() != 0 && lang.author.length() != 0 && lang.photo.length() != 0) {

                int id = this -> storage_ -> insertLanguage(lang, photo);
                lang.id = id;
                QVariant var;
                var.setValue(lang);

                this -> size_o_stor++;

                qDebug() << "COUNT:" + QString::number(this -> count_of_pages);

                if (this -> size_o_stor % this -> count_at_page == 1 && count_of_pages != 1) {
                    this -> count_of_pages++;
                    ui -> spinBox -> setMaximum(this -> count_of_pages);
                    ui -> label_11 -> setNum(this -> count_of_pages);
                    ui -> pushButton_3 -> setDisabled(false);
                } else {
                    if (this -> size_o_stor == this -> count_at_page + 1  && count_of_pages == 1) {
                        this -> count_of_pages++;
                        ui -> spinBox -> setMaximum(this -> count_of_pages);
                        ui -> label_11 -> setNum(this -> count_of_pages);
                        ui -> pushButton_3 -> setDisabled(false);
                    }
                }

                if (this -> page == this -> count_of_pages) {
                    QListWidgetItem * qStudentListItem = new QListWidgetItem();
                    qStudentListItem -> setText(lang.name.c_str());
                    qStudentListItem -> setData(Qt::UserRole, var);
                    ui -> listWidget -> addItem(qStudentListItem);
                 }
            } else {
                QMessageBox::information(
                       this,
                       "Ooops...",
                       "Ooops... Write at all fields!");
            }
        }
    }
}

void MainWindow::on_removeButton_clicked() {
    QList<QListWidgetItem *> items = ui -> listWidget -> selectedItems();
    if (items.count() != 0 && this -> storage_ != nullptr) {
        QMessageBox::StandardButton reply;

           reply = QMessageBox::question(
               this,
               "On delete",
               "Are you sure?",
               QMessageBox::Yes|QMessageBox::No);

           if (reply == QMessageBox::Yes) {
               if (items.count() != 0) {
                   foreach (QListWidgetItem * selectedItem, items) {
                       QVariant var = selectedItem -> data(Qt::UserRole);
                       Language * lang = static_cast<Language *>(var.data());

                       this -> storage_ -> removeLanguage(lang -> id);
                       int row_index = ui -> listWidget -> row(selectedItem);
                       ui -> listWidget -> takeItem(row_index);

                       this -> size_o_stor--;

                       if (this -> size_o_stor % this -> count_at_page == 0) {
                           if (this -> count_of_pages > 1) {
                               this -> count_of_pages--;
                           }
                           ui -> spinBox -> setMaximum(this -> count_of_pages);
                           ui -> label_11 -> setNum(this -> count_of_pages);
                           ui -> pushButton_3 -> setDisabled(true);
                       }

                       ui -> listWidget -> clear();

                       vector<Language> langs = this -> storage_ -> getYourLangs(ui -> spinBox -> text().toInt(), this -> count_at_page, ui -> lineEdit -> text());

                       for (int i = 0; i < langs.size(); i++) {
                           QVariant qVariant;
                           qVariant.setValue(langs[i]);
                           QListWidgetItem * qStudentListItem = new QListWidgetItem();
                           qStudentListItem -> setText(langs[i].name.c_str());
                           qStudentListItem -> setData(Qt::UserRole, qVariant);
                           ui -> listWidget -> addItem(qStudentListItem);
                       }


                       delete  selectedItem;
                   }
               }
           }
      }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem * item) {
    QVariant var = item -> data(Qt::UserRole);
    Language * lang = static_cast<Language *>(var.data());

    ui -> nameLabel -> setText(lang -> name.c_str());
    ui -> typeLabel -> setText(lang -> type.c_str());
    ui -> authorLabel -> setText(lang -> author.c_str());

    QString users;
    vector <Programmer> progs = this -> storage_ -> getProgrammersForLang(*lang);

    if (progs.size() != 0) {
        users += progs[0].name.c_str();
        users += " (+";
        users += QString::number(progs.size() - 1);
        users += ")";
    } else {
        users = "Empty";
    }

    ui -> label_9 -> setText(users);

    ui->label_10 -> setFixedSize(221, 201);

    QPixmap pm(lang -> photo.c_str());
    QSize pictureSize = pm.size();
    ui->label_10->setPixmap(pm);
    ui->label_10->setScaledContents(true);
    QSize lableSize = ui->label_10->size();
    pictureSize.scale(lableSize, Qt::KeepAspectRatio);
    ui->label_10->setFixedSize(pictureSize.width(), pictureSize.height());


    QString language = lang -> name.c_str();
    language += "_";
    language += lang -> type.c_str();
    language += "_";
    language += lang -> author.c_str();

    getCode_Qr(language, SLOT(on_printQR(QNetworkReply*)), this);
}

void MainWindow::exit_() {
    QMessageBox::StandardButton reply;
       reply = QMessageBox::question(
           this,
           "On delete",
           "Are you sure?",
           QMessageBox::Yes|QMessageBox::No);

       if (reply == QMessageBox::Yes) {
           close();
       } else {
           return;
       }
}

void MainWindow::logOut() {
    if (this -> storage_ != nullptr) {
        QMessageBox::StandardButton reply;
           reply = QMessageBox::question(
               this,
               "LogOut",
               "Are you sure?",
               QMessageBox::Yes|QMessageBox::No);

           if (reply == QMessageBox::Yes) {
              ui -> listWidget -> clear();
              ui -> nameLabel -> clear();
              ui -> typeLabel -> clear();
              ui -> authorLabel -> clear();
              ui -> label_9 -> clear();
              ui -> label_7 -> clear();
              ui -> label_10 -> clear();
              this -> storage_ -> close();
              delete storage_;
              this -> storage_ = nullptr;

              ui -> label_11 -> clear();
              ui -> pushButton_2 -> setDisabled(true);
              ui -> pushButton_3 -> setDisabled(true);
              ui -> spinBox -> setDisabled(true);
              ui -> actionExport_to_XML -> setDisabled(true);
              ui -> actionImport_from_XML -> setDisabled(true);

              ui -> lineEdit -> setDisabled(true);
              ui -> qr_label -> clear();

              ui -> addButton -> setDisabled(true);
              ui -> editButton -> setDisabled(true);
              ui -> removeButton -> setDisabled(true);
              ui -> actionLogOut -> setDisabled(true);
           } else {
               return;
           }
     }
}

void MainWindow::on_pushButton_2_clicked() {
    int n = ui -> spinBox -> text().toInt();
    n--;
    ui -> spinBox -> setValue(n);
}

void MainWindow::on_pushButton_3_clicked() {
    int n = ui -> spinBox -> text().toInt();
    n++;
    ui -> spinBox -> setValue(n);
}

void MainWindow::on_spinBox_valueChanged(const QString &arg1) {
    this -> page = arg1.toInt();

    if (ui -> spinBox -> text().toInt() == 1) {
        ui -> pushButton_2 -> setDisabled(true);
    }
    if (ui -> spinBox -> text().toInt() != 1) {
        ui -> pushButton_2 -> setDisabled(false);
    }
    if (ui -> spinBox -> text().toInt() == this -> count_of_pages) {
        ui -> pushButton_3 -> setDisabled(true);
    }
    if (ui -> spinBox -> text().toInt() != this -> count_of_pages) {
        ui -> pushButton_3 -> setDisabled(false);
    }

    ui -> listWidget -> clear();

    vector<Language> langs = this -> storage_ -> getYourLangs(ui -> spinBox -> text().toInt(), this -> count_at_page, ui -> lineEdit -> text());

    for (int i = 0; i < langs.size(); i++) {
        QVariant qVariant;
        qVariant.setValue(langs[i]);
        QListWidgetItem * qStudentListItem = new QListWidgetItem();
        qStudentListItem -> setText(langs[i].name.c_str());
        qStudentListItem -> setData(Qt::UserRole, qVariant);
        ui -> listWidget -> addItem(qStudentListItem);
    }

    qDebug() << "spin:" + arg1;
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1) {
    ui -> listWidget -> clear();

    vector<Language> langs = this -> storage_ -> getYourLangs(ui -> spinBox -> text().toInt(), this -> count_at_page, arg1);

        for (int i = 0; i < langs.size(); i++) {
            QVariant qVariant;
            qVariant.setValue(langs[i]);
            QListWidgetItem * qStudentListItem = new QListWidgetItem();
            qStudentListItem -> setText(langs[i].name.c_str());
            qStudentListItem -> setData(Qt::UserRole, qVariant);
            ui -> listWidget -> addItem(qStudentListItem);
        }

        this -> size_o_stor = this -> storage_ -> getSizeOfStor(arg1);

        float size_1 = (float)this -> size_o_stor / (float)this -> count_at_page;
        int size_2 = (int)this -> size_o_stor / (int)this -> count_at_page;

        int size = 0;

        if ((float)size_2 <= size_1) {
            if ((float)size_2 == (float)size_1) {
                size = size_2;
            } else {
                size = size_2 + 1;
            }
        } else {
            size = size_2 + 2;
        }

        this -> count_of_pages = size;

        ui -> label_11 -> setText(QString::number(size));
        ui -> spinBox -> setDisabled(false);
        ui -> spinBox -> setMinimum(1);
        if (this -> count_of_pages != 0) {
            ui -> spinBox -> setMaximum(this -> count_of_pages);
            ui -> label_11 -> setText(QString::number(size));
        } else {
            this -> count_of_pages++;
            ui -> pushButton_3 -> setDisabled(true);
            ui -> spinBox -> setMaximum(this -> count_of_pages);
            ui -> label_11 -> setText("1");
        }
        ui -> spinBox -> setValue(1);

        if (this -> page != 1) {
            ui -> pushButton_3 -> setDisabled(false);
        }

        qDebug() << "line:" + arg1;

        if (arg1 == "" && count_of_pages != 1) {
            this -> page = 1;
            ui -> pushButton_3 -> setDisabled(false);
        } else if (arg1 == "" && count_of_pages != 1) {
            ui -> pushButton_3 -> setDisabled(true);
        }

        if (count_of_pages > 1 && this -> page != count_of_pages) {
            ui -> pushButton_3 -> setDisabled(false);
        }

        if (this -> count_of_pages == this -> page) {
             ui -> pushButton_3 -> setDisabled(true);
        }
}

void MainWindow::exportToXml() {
    QList<QListWidgetItem *> item = ui -> listWidget -> selectedItems();

    if (item.count() == 1) {
        QListWidgetItem * selectedItem = item.at(0);
        QVariant var = selectedItem -> data(Qt::UserRole);
        Language * lang = static_cast<Language *>(var.data());

        QString name = lang -> name.c_str();

        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::Directory);
        QString current_dir = QDir::currentPath();
        QString default_name = "programmers_" + name + ".xml";
        QString folder_path = dialog.getSaveFileName(
               this,
               "Select New Storage Folder",
               current_dir + "/" + default_name,
               "Folders");
        XmlStorage * xml_ptr = new XmlStorage(folder_path.toStdString());

        if (folder_path.length() != 0) {
            vector<Programmer> prog = this -> storage_ -> getProgrammersForLang(*lang);

            xml_ptr -> close();
            xml_ptr -> open();

            for (int i = 0; i < prog.size(); i++) {
                xml_ptr -> insertProgrammer(prog[i]);
            }
            xml_ptr -> close();
            QMessageBox::information(
                   this,
                   "Ok",
                   "Exported to:" + folder_path);
        }

        delete  xml_ptr;
    } else {
        QMessageBox::information(
               this,
               "Not Ok",
               "Exporting enable just for one LANGUAGE!");
    }

}

void MainWindow::importFromXml() {
    QString fileName = QFileDialog::getOpenFileName(
         this,              // parent
         "Dialog Caption",  // caption
         "",                // directory to start with
         "XML (*.xml);;All Files (*)");
    XmlStorage * xml = new XmlStorage(fileName.toStdString());
    if (fileName.length() != 0) {
        xml -> open();
        vector<Programmer> lang = xml -> getAllProgrammer();

        for (int i = 0; i < lang.size(); i++) {
            this -> storage_ -> insertProgrammer(lang[i]);
        }

        QMessageBox::information(
               this,
               "Ok",
               "Imported from:" + fileName);
    }

    delete xml;
}

void MainWindow::on_printQR(QNetworkReply * reply) {
    QVariant statusCode;
    statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusCode.isValid()) {
        QMessageBox box;
        box.critical(0, "HTTP ERROR", "Status code is not valid");
        return;
    }
    if (statusCode.toInt() != 200) {
        QString error = "Error on " + reply->url().toString() + "\n";
        error += reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        QMessageBox box;
        box.critical(0, "HTTP ERROR", error);
        return;
    }
    QByteArray pngData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(pngData);
    pixmap = pixmap.scaled(ui -> qr_label -> size(), Qt::KeepAspectRatio);
    ui -> qr_label -> show();
    ui -> qr_label -> setPixmap(pixmap);
}
