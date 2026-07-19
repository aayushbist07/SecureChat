#include "authclient.h"

#include "authclient.h"
#include <QJsonObject>
#include <QJsonDocument>

void AuthClient::sendToken(QTcpSocket* socket, const QString& token) {
    QJsonObject obj;
    obj["type"] = "auth";
    obj["token"] = token;

    socket->write(QJsonDocument(obj).toJson());
}
