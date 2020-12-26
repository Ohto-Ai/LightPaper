#include "SingleApplication.h"
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QFileInfo>

SingleApplication::SingleApplication(int& argc, char** argv)
	: QApplication{ argc, argv }
	, isInstanceRunning{ false }
	, localServer{ nullptr }
	, serverName{ QFileInfo(applicationFilePath()).fileName() }
{
	initLocalConnection();
}

// ʵ����������
bool SingleApplication::instanceRunning() const
{
	return isInstanceRunning;
}

// ͨ��socketͨѶʵ�ֳ���ʵ�����У��������µ�����ʱ�����ú���
void SingleApplication::receiveNewLocalConnection()
{
	QLocalSocket* socket = localServer->nextPendingConnection();
	if (!socket)
		return;
	socket->waitForReadyRead(1000);
	QTextStream stream(socket);
	emit newInstanceStartup(stream.readAll());
	socket->deleteLater();
}

// ͨ��socketͨѶʵ�ֳ���ʵ�����У���ʼ���������ӣ�������Ӳ���server���򴴽��������˳�
void SingleApplication::initLocalConnection()
{
	isInstanceRunning = false;
	QLocalSocket socket;
	socket.connectToServer(serverName);
	if (socket.waitForConnected(500))
	{
		isInstanceRunning = true;
		// �������������͵������
		QTextStream stream(&socket);
		stream << arguments().join(' ');
		stream.flush();
		socket.waitForBytesWritten();
		return;
	}
	// ���Ӳ��Ϸ��������ʹ���һ��
	createLocalServer();
}

// ����LocalServer
void SingleApplication::createLocalServer()
{
	localServer->deleteLater();
	localServer = new QLocalServer(this);
	connect(localServer, &QLocalServer::newConnection, this, &SingleApplication::receiveNewLocalConnection);
	if (!localServer->listen(serverName) && localServer->serverError() == QAbstractSocket::AddressInUseError)
	{
		QLocalServer::removeServer(serverName);
		localServer->listen(serverName);
	}
}