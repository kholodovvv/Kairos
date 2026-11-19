#pragma once
#include <QObject>
#include <memory>
#include <QThread>

class MainWindow;
class MainMenu;
class Network;
class QTimer;

class Core: QObject
{
	Q_OBJECT

public:
	Core(void) {};
	bool ObjectsInit();
	~Core();

private slots:
	void onExitProgram();
	void onStartConnection(const QString &ServerName, const uint &port);
	void onStopConnection();
	void onTimerAlarm();
	void onInputMessages(const uint&, const QString&);
	void onInputNTPPackage(const QString&);

signals:
	void networkMessage(const QString &message);

private:
	MainWindow* _mw = nullptr;
	std::shared_ptr<QTimer> _timer = nullptr;
	std::shared_ptr<Network> _network = nullptr;
	std::unique_ptr<QThread> _networkThread = nullptr;
};