#include "authmanager.h"

AuthManager::AuthManager() {
    validTokens = {"ABC123", "JOIN99", "HELLO1"};
}

bool AuthManager::handleAuth(QTcpSocket* socket, const QString& token) {
    if (validTokens.contains(token)) {
        authStatus[socket] = true;
        validTokens.remove(token); // one-time use 🔥
        return true;
    }
    return false;
}

bool AuthManager::isAuthenticated(QTcpSocket* socket) {
    return authStatus.value(socket, false);
}