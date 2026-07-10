#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QWidget>
#include <QLineEdit>
#include <QIcon>
#include <QAction>
mainwindow1::mainwindow1(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::mainwindow1)
{

    ui->setupUi(this);
    this->QWidget::setWindowTitle("Secure Chat");

}
void mainwindow1::on_Login_clicked()
{
    QString username = ui->lineEdit_username->text();
    QString address = ui->lineEdit_address->text();
    if((username.isEmpty() && address.isEmpty()))

        QMessageBox::information(this,"Error" , "No username and address entered");
    else if(username.isEmpty())
        QMessageBox::information(this,"Error" , "No username entered");
    else if(address.isEmpty())
        QMessageBox::information(this,"Error" , "No address entered");
    else
    {
        //using the process of non model method
        ClientMainWindow *client = new ClientMainWindow();
        hide();
        client->setSessionUsername(username,address);
        client->show();
        close();
    }



}

mainwindow1::~mainwindow1(){}


