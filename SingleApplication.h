#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QObject>
#include <QApplication>

class QWidget;
class QLocalServer;

class SingleApplication : public QApplication
{
	Q_OBJECT
public:
	SingleApplication(int& argc, char** argv);
	bool isRunning();
	QWidget* mainWindow;
private slots:
	void newLocalConnection() const;
private:
	// ��ʼ����������
	void initLocalConnection();
	// ���������
	void newLocalServer();
	bool bRunning;                  // �Ƿ��Ѿ���ʵ��������
	QLocalServer* localServer;      // ����socket Server
	QString serverName;             // ��������
};

#endif // SINGLEAPPLICATION_H