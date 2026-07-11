#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QTcpSocket>
#include <QSet>
#include <QMap>

    class AuthManager {
public:
    AuthManager();

    bool handleAuth(QTcpSocket* socket, const QString& token);
    bool isAuthenticated(QTcpSocket* socket);

private:
    QSet<QString> validTokens;
    QMap<QTcpSocket*, bool> authStatus;
};
#endif // AUTHMANAGER_H
