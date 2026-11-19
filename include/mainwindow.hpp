#pragma once

#include <QMainWindow>
#include <QWidget>

class QLabel;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QString;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setCurrentTime(const QString& cTime);
	
private:
    void CreateToolBar();

signals:
    void exitProgram();
    void startConnection(const QString &ServerName, const uint &port);
    void stopConnection();

public slots:
    void onNetworkMessage(const QString &message);

private slots:
    void onExitProgram();
    void onClearForm();
    void onStartConnection();
    
private:
    QLabel* _lbNameServer = nullptr;
    QLabel* _lbCurrTime = nullptr;
    QLineEdit* _leNameServer = nullptr;
    QTextEdit* _teReceivedData = nullptr;
    QPushButton* _btnStartConnectionServer = nullptr;
    bool _btnStartConnectionServerStatus = false;
};