#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TCP_Server = new QTcpServer(this);
    connect(TCP_Server, &QTcpServer::newConnection, this, &MainWindow::newConnection);

    if(TCP_Server->listen(QHostAddress::Any, 8080)){
        ui->textEdit_Client_Messages->append("=== Server started successfully on port 8080 ===");
    } else {
        ui->textEdit_Client_Messages->append("!!! Server failed to start !!!");
    }
}

MainWindow::~MainWindow()
{
    // Clean up dynamic structures to avoid memory leaks
    qDeleteAll(UserRegistry);
    UserRegistry.clear();
    delete ui;
}

void MainWindow::newConnection(){
    while (TCP_Server->hasPendingConnections()){
        Add_New_Client_Connection(TCP_Server->nextPendingConnection());
    }
}

void MainWindow::Add_New_Client_Connection(QTcpSocket* socket)
{
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::Read_Data_From_Socket);

    // --- ADD THIS CONNECT LINE ---
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::Client_Disconnected);

    QJsonObject packet101;
    packet101["type"] = 101;
    packet101["p_value"] = DH_P;
    packet101["g_value"] = DH_G;

    QJsonDocument doc(packet101);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    ui->textEdit_Client_Messages->append("New unauthenticated client connected.");
}

void MainWindow::Read_Data_From_Socket()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    // Read everything currently sitting in the network socket's buffer
    QByteArray rawIncomingStream = socket->readAll();

    // FIXED: Split data packages explicitly by our newline boundary marker
    // This stops TCP from blending multiple JSON documents into a single un-parsable block
    QList<QByteArray> segmentedPackets = rawIncomingStream.split('\n');

    for (const QByteArray& rawPacket : segmentedPackets) {
        // Clean off any lingering spacing adjustments or blank loops
        QByteArray cleanPacket = rawPacket.trimmed();
        if (cleanPacket.isEmpty()) continue;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(cleanPacket, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            ui->textEdit_Client_Messages->append("Parsing Warning: Dropped malformed segment: " + QString(cleanPacket));
            continue;
        }

        QJsonObject json = doc.object();
        int messageType = json["type"].toInt();

        // STEP 2: Handle Incoming Client Registration Information
        if (messageType == 102) {
            QString name = json["username"].toString();
            int clientPubKey = json["public_key"].toInt();

            if (name.isEmpty()) continue;

            // If a client reconnects, clean up their old entry first
            if (UserRegistry.contains(name)) {
                delete UserRegistry.take(name);
            }

            // Provision a fresh tracking token inside our master layout map
            ClientInfo* info = new ClientInfo();
            info->username = name;
            info->socket = socket;
            info->publicKey = clientPubKey;
            UserRegistry[name] = info;

            // Dynamically refresh the targeting interface controls
            ui->comboBox_Client_List->clear();
            ui->comboBox_Client_List->addItems(UserRegistry.keys());

            ui->textEdit_Client_Messages->append(QString("Successfully registered: %1 (Key: %2)").arg(name).arg(clientPubKey));

            // CRITICAL FIX: Explicitly check if our active leader is dead, missing, or unassigned
            if (leaderUsername.isEmpty() || !UserRegistry.contains(leaderUsername)) {
                leaderUsername = name;
                ui->textEdit_Client_Messages->append(QString("+++ SYSTEM APPOINTMENT: %1 designated as Room Leader. +++").arg(leaderUsername));
            }

            // Trigger structural updates out to our processing core
            sendLeaderUpdatedDirectory();
        }

        // STEP 4: Handle Distributed Cryptographic Arrays arriving from our Leader
        else if (messageType == 105) {
            int leaderPubKey = json["leader_public_key"].toInt();
            QJsonArray keyPayloads = json["key_payloads"].toArray();

            ui->textEdit_Client_Messages->append("Leader delivered processing bundles. Route mapping processing...");

            for (int i = 0; i < keyPayloads.size(); ++i) {
                QJsonObject payload = keyPayloads[i].toObject();
                QString targetUser = payload["target_user"].toString();
                QString encryptedKeyHex = payload["encrypted_key"].toString();

                // Look up the targeted profile in our client register database map
                if (UserRegistry.contains(targetUser)) {
                    QJsonObject dispatchPacket;
                    dispatchPacket["type"] = 105;
                    dispatchPacket["leader_public_key"] = leaderPubKey;
                    dispatchPacket["encrypted_key"] = encryptedKeyHex;

                    QJsonDocument dispatchDoc(dispatchPacket);
                    QByteArray outboundWireData = dispatchDoc.toJson(QJsonDocument::Compact) + "\n";

                    UserRegistry[targetUser]->socket->write(outboundWireData);
                }
            }
        }
    }
}

// STEP 3: Automatically broadcast updating membership tracking logs back out to the designated room master
void MainWindow::sendLeaderUpdatedDirectory() {
    // Structural Guard: Fail safely if the designated leader profile is corrupted or missing
    if (leaderUsername.isEmpty() || !UserRegistry.contains(leaderUsername)) {
        ui->textEdit_Client_Messages->append("Directory sync skipped: Group Leader is missing or disconnected.");
        return;
    }

    QJsonObject packet104;
    packet104["type"] = 104;

    QJsonArray participantsArray;
    // Iterate across our operational data register map
    for (auto it = UserRegistry.begin(); it != UserRegistry.end(); ++it) {
        // Optimization Guard: The leader doesn't need to perform cryptographic tasks against their own key
        if (it.key() == leaderUsername) continue;

        QJsonObject peer;
        peer["username"] = it.value()->username;
        peer["public_key"] = it.value()->publicKey;
        participantsArray.append(peer);
    }

    packet104["participants"] = participantsArray;

    // Route the transaction direct down the leader's active connection channel
    QTcpSocket* leaderSocket = UserRegistry[leaderUsername]->socket;
    QJsonDocument doc(packet104);
    QByteArray packageBytes = doc.toJson(QJsonDocument::Compact) + "\n";

    leaderSocket->write(packageBytes);
    ui->textEdit_Client_Messages->append(QString("Sent updated member listing to Leader (%1).").arg(leaderUsername));
}

void MainWindow::on_push_button_Send_clicked()
{
    QString rawMessage = ui->lineEdit_send_message->text();
    QString receiver = ui->comboBox_Client_List->currentText();

    if (rawMessage.isEmpty()) return;

    // Format chat messages neatly as JSON packages so they don't break the client's parser
    QJsonObject regularMessageObj;
    regularMessageObj["type"] = 1; // Arbitrary type value for standard messages
    regularMessageObj["payload"] = rawMessage;

    QJsonDocument msgDoc(regularMessageObj);
    QByteArray msgPayload = msgDoc.toJson(QJsonDocument::Compact) + "\n";

    if (ui->combobox2_Send_Message_type->currentText() == "All") {
        for (auto client : UserRegistry.values()) {
            client->socket->write(msgPayload);
        }
    } else {
        if (UserRegistry.contains(receiver)) {
            UserRegistry[receiver]->socket->write(msgPayload);
        }
    }
    ui->lineEdit_send_message->clear();
}


void MainWindow::Client_Disconnected()
{
    // Find out which socket just disconnected
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString disconnectedUser = "";

    // Search our registry map to find who owned this socket pointer
    for (auto it = UserRegistry.begin(); it != UserRegistry.end(); ++it) {
        if (it.value()->socket == socket) {
            disconnectedUser = it.key();
            break;
        }
    }

    if (!disconnectedUser.isEmpty()) {
        ui->textEdit_Client_Messages->append(QString("!!! Client disconnected: %1 !!!").arg(disconnectedUser));

        // Remove them from our registry map and delete their allocated structure memory
        delete UserRegistry.take(disconnectedUser);

        // Refresh our UI dropdown list
        ui->comboBox_Client_List->clear();
        ui->comboBox_Client_List->addItems(UserRegistry.keys());

        // CHOSEN AUTOMATION: If the person who left was our leader, assign a new leader right now!
        if (leaderUsername == disconnectedUser) {
            leaderUsername.clear(); // Clear the old dead leader name

            if (!UserRegistry.isEmpty()) {
                // Pick the first remaining person left standing in our database map (e.g., Bob!)
                leaderUsername = UserRegistry.firstKey();
                ui->textEdit_Client_Messages->append(QString("+++ LEADER LEFT. New leader appointed: %1 +++").arg(leaderUsername));

                // Tell the new leader to generate fresh room keys for anyone remaining
                sendLeaderUpdatedDirectory();
            } else {
                ui->textEdit_Client_Messages->append("The room is now completely empty.");
            }
        }
    }

    socket->deleteLater(); // Safely clear out the socket memory from the operating system
}