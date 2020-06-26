QFtpServer - an FTP server written in Qt
----------------------------------------
### Changes
It now emits signals whenever a file transfer starts and finishes.
```FtpServer()``` constructor no longer starts a new connection, you should call ```start()``` and get a new connection from signal:
```
FtpServer *ftpServer = new FtpServer(this, "/home/user/ftplocation", 21, "Username", "Pass");
ftpServer->start();


connect(ftpServer, &FtpServer::newConnection, this, [=] (FtpControlConnection *connection) {

	// don't forget to save a new connection because 
	// it will automatically disconnect after some time and no signals will be emmited
	ftpConnection = connection;							

	// connect to fileTransferStarted signal if you wish
     connect(ftpConnection, &FtpControlConnection::fileTransferStarted, this, &TestClass::waitForFtpTransfer);
});
```
```fileTransferStarted``` contains file name in its argument. After it is emmited, you can connect to ```fileTransferFinished```:
void TestClass::waitForFtpTransfer(QString fileName)
{
	connect(ftpConnection, &FtpControlConnection::fileTransferFinished, this, [=]{
		// do something
	});	
}
    ```

### Description

QFtpServer is an event-driven server which is written entirely in Qt. It is non-blocking and supports multiple connections.

This project started because I couldn't find a free FTP server for my Symbian phone so I decided to try and make one of my own.  I've tested it against Filezilla, Chrome, Firefox, Safari, Windows Explorer, and others.

### Features

It supports:

* Active connections (PORT command).
* Passive connections (PASV command).
* Random access for files (APPE, RETR), so stopped uploads and downloads can be continued.
* Secure FTP, also known as FTP-ES which uses SSL/TLS sockets.

It doesn't support:
* IPv6.
* FTPS (this standard is obsolete and FTP-ES should be used instead).
* Multiple user accounts.

### Example usage:

    server = new FtpServer(this, "/ftp/root/path", 21, "MyUserName", "MyPassword", false, false);
