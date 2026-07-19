#ifndef AUTHCLIENT_H
#define AUTHCLIENT_H

#include <QTcpSocket>

class AuthClient {
public:
    static void sendToken(QTcpSocket* socket, const QString& token);
};

#endif // AUTHCLIENT_H
