#include "ftpserver.h"

#include "sslserver.h"

#include <QDebug>
#include <QNetworkInterface>
#include <QSslSocket>

FtpServer::FtpServer(QObject *parent, const QString &rootPath, int port, const QString &userName, const QString &password, bool readOnly, bool onlyOneIpAllowed) :
    QObject(parent)
{
    this->userName = userName;
    this->password = password;
    this->rootPath = rootPath;
    this->readOnly = readOnly;
    this->port = port;
    this->onlyOneIpAllowed = onlyOneIpAllowed;
}

void FtpServer::start()
{
    server = new SslServer(this);

    // In Qt4, QHostAddress::Any listens for IPv4 connections only, but as of
    // Qt5, it now listens on all available interfaces, and
    // QHostAddress::AnyIPv4 needs to be used if we want only IPv4 connections.
#if QT_VERSION >= 0x050000

    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    QHostAddress ip;
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost && address.toString().section( ".",-1,-1 ) != "1") {
            ip = address;
            break;
        }
    }

    if(ip.toString().isEmpty()) {
        ip = localhost;
    }

    server->listen(ip, port);
#else
    server->listen(QHostAddress::Any, port);
#endif

    connect(server, SIGNAL(newConnection()), this, SLOT(startNewControlConnection()));
}

bool FtpServer::isListening()
{
    return server->isListening();
}

QString FtpServer::lanIp()
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
            return address.toString();
        }
    }
    return "";
}

void FtpServer::startNewControlConnection()
{
    QSslSocket *socket = (QSslSocket *) server->nextPendingConnection();

    // If this is not a previously encountered IP emit the newPeerIp signal.
    QString peerIp = socket->peerAddress().toString();
    qDebug() << "connection from" << peerIp;
    if (!encounteredIps.contains(peerIp)) {
        // If we don't allow more than one IP for the client, we close
        // that connection.
        if (onlyOneIpAllowed && !encounteredIps.isEmpty()) {
            delete socket;
            return;
        }

        emit newPeerIp(peerIp);
        encounteredIps.insert(peerIp);
    }

    // Create a new FTP control connection on this socket.
    emit newConnection(new FtpControlConnection(this, socket, rootPath, userName, password, readOnly));
}
