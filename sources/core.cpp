#include "include/core.hpp"
#include "include/mainwindow.hpp"
#include "include/network.hpp"
#include <QApplication>
#include <QTimer>
#include <QTime>
#include <QPair>

Core::~Core()
{
	if (_networkThread->isRunning())
		_networkThread->quit();
}

bool Core::ObjectsInit()
{
	_mw = new MainWindow(nullptr);

	if (!_mw) {
		return false;
	}

	if (!_timer) {
		_timer = std::make_shared<QTimer>(new QTimer());
	}

	connect(_mw, SIGNAL(exitProgram()), this, SLOT(onExitProgram()));
	connect(_mw, SIGNAL(startConnection(const QString&, const uint&)), this, SLOT(onStartConnection(const QString&, const uint&)));
	connect(_mw, SIGNAL(stopConnection()), this, SLOT(onStopConnection()));
	connect(this, SIGNAL(networkMessage(const QString&)), _mw, SLOT(onNetworkMessage(const QString&)));
	connect(_timer.get(), SIGNAL(timeout()), this, SLOT(onTimerAlarm()));

	if (_networkThread == nullptr)
		_networkThread = std::make_unique<QThread>();

	if (!_network.get()) {
		_network = std::make_shared<Network>(new Network());
		connect(_network.get(), SIGNAL(answerForServer(const QString&)), this, SLOT(onInputNTPPackage(const QString&)), Qt::AutoConnection);

		_network->moveToThread(_networkThread.get());
	}

	if(!_networkThread->isRunning())
		_networkThread->start();

	_timer->start(1000);

	_mw->show();

	return true;
}

void Core::onExitProgram()
{
	QApplication::exit(0);
}

void Core::onStartConnection(const QString &ServerName, const uint &port)
{
	if (_network) {
		QPair<bool, QString> isConnection = _network->Connect(ServerName, port);
	
		if (isConnection.first) {
			emit networkMessage(isConnection.second);
			emit networkMessage(_network->SendMessage().second);
		}
		else {
			emit networkMessage(isConnection.second);
		}
	}
}

void Core::onStopConnection()
{
	if (_network) {
		_network->Disconnect();
	}
}

void Core::onTimerAlarm()
{
	_mw->setCurrentTime(QTime::currentTime().toString("HH:mm:ss"));
}

void Core::onInputMessages(const uint &typeMessage, const QString &textMessage)
{
	//typeMessage = 0 : Critical
	//				1 : Error
	//				2 : Info

	switch (typeMessage)
	{
		case 0:

		break;

		case 1:

		break;

		case 2:
			emit networkMessage(textMessage);
		break;
	}

}

void Core::onInputNTPPackage(const QString &textServerAnswer)
{
	emit networkMessage(textServerAnswer);

}

