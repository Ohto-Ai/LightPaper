#include "SingleApplication.h"
#include <QWidget>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QFileInfo>

SingleApplication::SingleApplication(int& argc, char** argv)
    : QApplication{ argc, argv }
    , mainWindow{ nullptr }
    , bRunning{ false }
	, localServer{ nullptr }
{
    // ȡӦ�ó�������ΪLocalServer������
    serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    initLocalConnection();
}

bool SingleApplication::isRunning()
{
    return bRunning;
}


// ˵����
// ͨ��socketͨѶʵ�ֳ���ʵ�����У��������µ�����ʱ�����ú���

void SingleApplication::newLocalConnection() const
{
    QLocalSocket* socket = localServer->nextPendingConnection();
    if (!socket)
        return;
    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    //��������
    delete socket;
    if (mainWindow != nullptr)
    {
        //�����
        mainWindow->raise();
        mainWindow->activateWindow();
        mainWindow->setWindowState((mainWindow->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
        mainWindow->show();
    }
}


// ˵����
// ͨ��socketͨѶʵ�ֳ���ʵ�����У�
// ��ʼ���������ӣ�������Ӳ���server���򴴽��������˳�

void SingleApplication::initLocalConnection()
{
    bRunning = false;
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500))
    {
        bRunning = true;
        // ���������磺�������������͵������
        QTextStream stream(&socket);
        QStringList args = QCoreApplication::arguments();
        if (args.count() > 1)
            stream << args.last();
        else
            stream << QString();
        stream.flush();
        socket.waitForBytesWritten();

        return;
    }

    //���Ӳ��Ϸ��������ʹ���һ��
    newLocalServer();
}


// ˵����
// ����LocalServer

void SingleApplication::newLocalServer()
{
    localServer = new QLocalServer(this);
    connect(localServer, &QLocalServer::newConnection, this, &SingleApplication::newLocalConnection);
    if (!localServer->listen(serverName))
    {
        // ��ʱ����ʧ�ܣ������ǳ������ʱ,�������̷����µ�,�Ƴ�֮
        if (localServer->serverError() == QAbstractSocket::AddressInUseError)
        {
            QLocalServer::removeServer(serverName); // <-- �ص�
            localServer->listen(serverName); // �ٴμ���
        }
    }
}