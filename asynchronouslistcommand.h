#ifndef ASYNCHRONOUSLISTCOMMAND_H
#define ASYNCHRONOUSLISTCOMMAND_H

#include "asynchronouscommand.h"

class AsynchronousListCommand : public AsynchronousCommand
{
    Q_OBJECT
public:
    explicit AsynchronousListCommand(QObject *parent, const QString &listDirectory, bool nameListOnly = false);
    ~AsynchronousListCommand();

private:
    void start(QTcpSocket *socket);

    QTcpSocket* socket;
    QString listDirectory;
    bool nameListOnly;
};

#endif // ASYNCHRONOUSLISTCOMMAND_H
