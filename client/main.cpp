#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    mainwindow1 login;
    login.show();
    return app.exec();
}   