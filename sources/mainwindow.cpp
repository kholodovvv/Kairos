#include "include/mainwindow.hpp"
#include <QLayout>
#include <QPointer>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QToolBar>
#include <QAction>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle(":: Kairos SNTP client ::");
	setWindowIcon(QPixmap(":/images/favicon.png"));
	setFixedSize(800, 600);

	QPointer<QWidget> widget = new QWidget(this);
	setCentralWidget(widget);

	QVBoxLayout* mainLayot = new QVBoxLayout();
	mainLayot->setContentsMargins(5, 5, 0, 10);
	widget->setLayout(mainLayot);

	QFont grbFont;
	grbFont.setFamily("Arial");
	grbFont.setPointSize(12);

	QFont lbFont;
	lbFont.setFamily("Arial");
	lbFont.setPointSize(10);

	_lbCurrTime = new QLabel(widget);
	_lbCurrTime->setFont(lbFont);
	_lbCurrTime->setContentsMargins(0, 0, 5, 0);

	CreateToolBar();
	
	{
		QGroupBox* grbConnectionToServer = new QGroupBox(widget);
		grbConnectionToServer->setTitle(QString::fromLocal8Bit("Сетевое подключение"));
		grbConnectionToServer->setAlignment(Qt::AlignHCenter);
		grbConnectionToServer->setFixedWidth(MainWindow::width() - 10);
		grbConnectionToServer->setFixedHeight((MainWindow::height() / 10) + (MainWindow::height() / 10));
		grbConnectionToServer->setContentsMargins(5, 0, 0, 0);
		grbConnectionToServer->setFont(grbFont);

		QHBoxLayout *onSevrerLayout = new QHBoxLayout();
		onSevrerLayout->setContentsMargins(0, 0, 0, 0);
		onSevrerLayout->setAlignment(Qt::AlignLeft);
		QVBoxLayout *nameServerLayot = new QVBoxLayout();
		nameServerLayot->setContentsMargins(10, 0, 0, 0);
		nameServerLayot->setAlignment(Qt::AlignLeft);
		QHBoxLayout *startButtonLayot = new QHBoxLayout();
		
		_lbNameServer = new QLabel(widget);
		_lbNameServer->setText(QString::fromLocal8Bit("Имя сервера:порт"));
		_lbNameServer->setFont(lbFont);
		_lbNameServer->setScaledContents(true);
		_lbNameServer->setFixedHeight((MainWindow::height() / 10) / 1.5);

		startButtonLayot->setContentsMargins(0, 0, 0, 0);
		startButtonLayot->setAlignment(Qt::AlignLeft);

		_leNameServer = new QLineEdit(widget);
		_leNameServer->setFixedWidth((MainWindow::width() / 4) + (MainWindow::width() / 10));
		_leNameServer->setFixedHeight(MainWindow::height() / 18);
		_leNameServer->setMaxLength(30);
		_leNameServer->setFont(lbFont);

		_btnStartConnectionServer = new QPushButton(widget);
		_btnStartConnectionServer->setIcon(QPixmap(":/images/play_64.png"));
		_btnStartConnectionServer->setIconSize(QSize(24, 24));
		_btnStartConnectionServer->setFixedWidth(_leNameServer->height());
		_btnStartConnectionServer->setFixedHeight(_leNameServer->height());
		_btnStartConnectionServer->setFont(lbFont);
		connect(_btnStartConnectionServer, &QPushButton::clicked, this, &MainWindow::onStartConnection);

		nameServerLayot->addWidget(_lbNameServer);
		nameServerLayot->addLayout(startButtonLayot);
		startButtonLayot->addWidget(_leNameServer);
		startButtonLayot->setSpacing(5);
		nameServerLayot->addStretch();
		onSevrerLayout->addLayout(nameServerLayot);
		onSevrerLayout->setSpacing(2);

		startButtonLayot->addWidget(_btnStartConnectionServer);
		mainLayot->addWidget(grbConnectionToServer);
		grbConnectionToServer->setLayout(onSevrerLayout);
		mainLayot->addStretch();
	}

	{
		QGroupBox* grbReceivedData = new QGroupBox(widget);
		grbReceivedData->setTitle(QString::fromLocal8Bit("Полученные данные"));
		grbReceivedData->setAlignment(Qt::AlignHCenter);
		grbReceivedData->setFixedWidth(MainWindow::width() - 10);
		grbReceivedData->setContentsMargins(5, 5, 5, 10);
		grbReceivedData->setFont(grbFont);

		QVBoxLayout* receivedDataLayot = new QVBoxLayout();
		receivedDataLayot->setContentsMargins(10, 10, 10, 10);
		receivedDataLayot->setAlignment(Qt::AlignLeft);

		_teReceivedData = new QTextEdit(widget);
		_teReceivedData->setFixedHeight((MainWindow::height() / 2) + (MainWindow::height() / 12));
		_teReceivedData->setFont(lbFont);
		_teReceivedData->setReadOnly(true);

		receivedDataLayot->addWidget(_teReceivedData);
		grbReceivedData->setLayout(receivedDataLayot);
		mainLayot->addWidget(grbReceivedData);
	}

}

void MainWindow::onExitProgram()
{
	emit exitProgram();
}

void MainWindow::onClearForm()
{
	_leNameServer->clear();
	_teReceivedData->clear();
}

void MainWindow::setCurrentTime(const QString &cTime)
{
	if (_lbCurrTime)
		_lbCurrTime->setText(QString::fromLocal8Bit("Время: %1").arg(cTime));
}

void MainWindow::onStartConnection()
{
	if (!_btnStartConnectionServerStatus) {
		if (_leNameServer) {
			if (_leNameServer->text().length() > 0) {
				QString nameServer = QString(_leNameServer->text()).section(":", 0, 0);
				uint port = QString(_leNameServer->text()).section(":", 1, 1).toInt();

				emit startConnection(nameServer, port);

				_btnStartConnectionServerStatus = true;
				_btnStartConnectionServer->setIcon(QPixmap(":/images/stop_64.png"));
				_btnStartConnectionServer->setIconSize(QSize(24, 24));
			}
			else {
				QMessageBox::critical(this, QString::fromLocal8Bit("Ошибка!"), QString::fromLocal8Bit("Не заполнено поле \"Имя сервера\"!"));
			}
		}
	}
	else {
		emit stopConnection();

		_btnStartConnectionServerStatus = false;
		_btnStartConnectionServer->setIcon(QPixmap(":/images/play_64.png"));
	}
	
}

void MainWindow::onNetworkMessage(const QString &message) {
	//_teReceivedData->append(message + "\n");
	_teReceivedData->append(message);
}

void MainWindow::CreateToolBar()
{
	QToolBar* tBar = addToolBar("ToolBar");
	tBar->setFixedHeight(MainWindow::height() / 10);
	tBar->setMovable(false);
	tBar->setIconSize(QSize(64, 64));
	tBar->addSeparator();

	QAction* actClear = tBar->addAction(QString::fromLocal8Bit("Clear"));
	actClear->setIcon(QPixmap(":/images/clear_64.png"));
	actClear->setToolTip(QString::fromLocal8Bit("Отчистить форму"));

	tBar->addSeparator();

	QAction* actSettings = tBar->addAction(QString::fromLocal8Bit("Settings"));
	actSettings->setIcon(QPixmap(":/images/set_72.png"));
	actSettings->setToolTip(QString::fromLocal8Bit("Настройки"));

	tBar->addSeparator();

	QAction* actExit = tBar->addAction(QString::fromLocal8Bit("Exit"));
	actExit->setIcon(QPixmap(":/images/exit_72.png"));
	actExit->setToolTip(QString::fromLocal8Bit("Выход"));

	tBar->addSeparator();
	tBar->addSeparator();

	QAction* actAbout = tBar->addAction(QString::fromLocal8Bit("About"));
	actAbout->setIcon(QPixmap(":/images/about_72.png"));
	actAbout->setToolTip(QString::fromLocal8Bit("О программе"));

	
	_lbCurrTime->setText(QString::fromLocal8Bit("Время: %1").arg("00:00:00"));
	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	tBar->addWidget(spacer);
	tBar->addWidget(_lbCurrTime);
	

	connect(actExit, &QAction::triggered, this, &MainWindow::onExitProgram);
	connect(actClear, &QAction::triggered, this, &MainWindow::onClearForm);
}

MainWindow::~MainWindow()
{

}