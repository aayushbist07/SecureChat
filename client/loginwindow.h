#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include "serverdiscoverer.h"
#include <QListWidgetItem>

namespace Ui {
class mainwindow1;
}

class mainwindow1 : public QDialog
{
    Q_OBJECT

public:
    explicit mainwindow1(QWidget *parent = nullptr);
    ~mainwindow1();


private slots:
    void on_Login_clicked();
    void setupDiscovery();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::mainwindow1 *ui;
    ServerDiscoverer *discoverer;

};

#endif // LOGINWINDOW_H
