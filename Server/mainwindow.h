#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Structure to hold everything about a single user
struct ClientInfo {
    QString username;
    QTcpSocket* socket = nullptr;
    int publicKey = 0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void newConnection();
    void Read_Data_From_Socket();
    void on_push_button_Send_clicked();
    void Client_Disconnected();

private:
    void Add_New_Client_Connection(QTcpSocket* socket);
    void sendLeaderUpdatedDirectory();

private:
    Ui::MainWindow *ui;
    QTcpServer *TCP_Server;

    // --- ADD THESE THREE LINES IN YOUR VARIABLES SECTION ---
    QMap<QString, ClientInfo*> UserRegistry; // Maps names to their structural info
    QString leaderUsername = "";            // Tracks who the room leader is!

    const int DH_P = 23;                    // Global Diffie-Hellman base parameters
    const int DH_G = 5;
};
#endif // MAINWINDOW_H