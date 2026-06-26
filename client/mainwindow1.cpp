#include "mainwindow1.h"
#include "ui_mainwindow1.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QWidget>
#include <QLineEdit>

mainwindow1::mainwindow1(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::mainwindow1)
{

    ui->setupUi(this);
    this->QWidget::setWindowTitle("Secure Chat");
    QPixmap pix(":/images/images.jpg");
    ui->Images->setPixmap(pix);
    ui->Images->setScaledContents(true);
    //holder
    ui->lineEdit_username->setPlaceholderText("username");
    ui->lineEdit_username->setStyleSheet(
       "QLineEdit{"
        "placeholder-text-color:white;"
        "font-size: 20px;"
        "border:none;"
        "border-bottom:2px solid white;"
        "color:white;"
        "}"

        );
    ui->lineEdit_Port->setPlaceholderText("port");
    ui->lineEdit_Port->setStyleSheet(
        "QLineEdit{"
        "placeholder-text-color:white;"
        "font-size:20px;"
        "border:none;"
        "border-bottom:2px solid white;"
        "color:white;"
        "}"
        );


}

mainwindow1::~mainwindow1(){}

void mainwindow1::on_Login_clicked()
{
    QString username = ui->lineEdit_username->text();
    int Port = ui->lineEdit_Port->text().toInt();
    ClientMainWindow *client = new ClientMainWindow();
    if(!username.isEmpty())
    {
    //genius work with the port
        //using the process of non model method
        hide();
        client->setSessionUsername(username);
        client->show();
        close();

    }
    else QMessageBox::information(this , "Noob" , "uh , No username entered");

}
