#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QNetworkReply>
#include <QListWidget>
#include "xml_storage.h"
#include "storage.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget * parent = 0);
    ~MainWindow();

private slots:
    void onOld();

    void on_editButton_clicked();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void exit_();

    void logOut();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_spinBox_valueChanged(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void exportToXml();

    void importFromXml();

    void on_printQR(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;

    int count_at_page;
    int size_o_stor = 0;
    int count_of_pages;
    int page = 1;

    Storage * storage_; // <-- STORAGE
    QTcpSocket  _socket;
};

#endif // MAINWINDOW_H
