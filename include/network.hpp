#pragma once
#include <memory>
#include <QWidget>
#include <QPair>
#include <QMap>
#include <string>
#include <QThread>

class QUdpSocket;
class QByteArray;
class QTimer;

#define NTP_CLIENT_MODE 3
#define NTP_BROADCAST_MODE 5

class DTime: public QObject {
    
    Q_OBJECT

public:
    DTime(QObject* parent = nullptr) {};
    ~DTime() {};
    QString GetTimeZoneID();
    int GetTimeOffsetFromUTC();
    QPair<qint64, qint64> GetCurrentTimeUTC();
    QString ConvertInt64ToTimeUTC(const qint64&);
};

//class TimerTimeDelayReseption: public QObject {
//
//    Q_OBJECT
//
//public:
//    TimerTimeDelayReseption(QObject* parent = nullptr) {};
//    ~TimerTimeDelayReseption() {};
//    int GetTime();
//    void Init();
//
//protected:
//    void timerEvent(QTimerEvent* event) override;
//
//public slots:
//    void onStart(const int& delay);
//    void onStop();
//    void onReset();
//    void onGetTime();
//
//private slots:
//    void onCounterTime();
//
//signals:
//    void sTimeDelayReseption(const int &msec);
//
//private:
//    std::shared_ptr<QTimer> _timer = nullptr;
//    int _time = 0;
//    int _delay = 0;
//    int _idTimer = 0;
//};

struct ntp_package {
    qint8 leapIndicator = 0; // Индикатор коррекции (0 - Нет предупреждения, 1 - Последняя минута дня содержит 61 секунду, 
                                // 2 - Последняя минута дня содержит 59 секунд, 3 - Неизвестно (время не синхронизировано))
    qint8 versionNumber = 3; // Целое число, представляющее версию протокола
    qint8 mode = 3; // Целое число, представляющее режим (0 - Зарезервировано, 1 - Симметричный активный режим,
                        // 2 - Симметричный пассивный режим, 3 - Клиент, 4 - Сервер, 5 - Широковещательный режим, 6 - Контрольное сообщение NTP)
    qint8 stratum = 0; // Часовой слой (0 - Не определено или недопустим, 1 - Первичный сервер, 2-15 - Вторичный сервер, использующий NTP,
                            // 16 - Не синхронизировано, 17-255 - Зарезервировано)

    qint8 poll = 4; // Максимальный интервал между последовательными сообщениями (Значение равно двоичному логарифму секунд.)
                        // Предлагаемые по умолчанию пределы на минимальные и максимальные опросы — 6 и 10
    qint8 precision = -6; // Точность системных часов. Значение равно двоичному логарифму секунд. Например, значению −18 будет соответствовать точность около 1 мкс.
    qint32 rootDelay = 0; // Задержка. Общее время распространения сигнала в обе стороны в коротком формате NTP.
    qint32 rootDispersion = 0; // Общая дисперсия для источника времени в коротком формате NTP.
    qint32 referenceID = 0; // Код источника синхронизации. Зависит от значения в поле Часовой слой.
                                // Для слоя 0 — это четыре ASCII символа, называемые «kiss code», используются для отладки и мониторинга.
                                // Для слоя 1 — это четыре октета ASCII символов, дополненные слева нулями, назначенные для опорного времени.
                                // Для слоя 2 и выше — это идентификатор сервера и может быть использован для фиксирования временных петель.
    qint64 refTimestamp = 0; // Время, когда система последний раз устанавливала или корректировала время.
    qint64 originTimestamp = 0; // Время клиента, когда запрос отправляется серверу (заполняет сервер).
    qint64 receiveTimestamp = 0; // Время сервера, когда запрос приходит от клиента.
    qint64 transmitTimestamp = 0; // Время сервера, когда запрос отправляется клиенту.
    qint64 originClientTimestamp = 0; // Время клиента, когда запрос отправляется серверу (заполняется клиентом) !!! Убрать
    //qint64 receiveClientTimeCounter = 0; //Счетчик мсек клиента, останавливается когда запрос пришёл от сервера
    qint64 offsetTimestamp = 0; //Время задержки запроса
    QString note = QString::fromLocal8Bit("Отсутствует"); //Примечание для пользователя

    QByteArray PackageToByte();
    QByteArray ConvertTimeToByte(const qint64&, const qint64&);
    //QPair<qint64, qint64> GetCurrentTimeUTC();
    quint32 ConvertByteToUInt32(QByteArray);
    //QString ConvertInt64ToTimeUTC(const qint64&);
    std::shared_ptr<DTime> _dt = nullptr;
};

class Network : public QObject//QWidget
{
    Q_OBJECT

public:
    /*explicit*/ Network(/*QWidget* parent = nullptr*/QObject* parent = nullptr) {};
    QPair<bool, QString> Connect(const QString& nameServer, const uint& port);
    void SetConnectionWait(const uint& sec);
    void SetAnswerWait(const uint& sec);
    void SetPortListen(const uint& port);
    void SetNTPMode(const uint& ntpMode);
    QPair<bool, QString> SendMessage();
    QPair<bool, QString> Disconnect();
    ~Network();

private:
    void SocketInit();
    void ParseAndCheckReceiveNtpPackage(const QByteArray&);

signals:
    void answerForServer(/*const QByteArray&*/const QString&);
    void startTimerTimeDelayReceiv(const int &delay);
    void stopTimerTimeDelayReceiv();
    void getTimeDelayReceiv();

private slots:
    void onReadData();
    void onDisconnected();
    //void onTimeDelayReceiv(const int& timeDelay);

//public slots:
//    void onnTimerAlarm();

private:
    std::shared_ptr<QUdpSocket> _socket = nullptr;
    std::shared_ptr<QUdpSocket> _socketListen = nullptr;
    uint _connectionWait = 10; //Время ожидания соединения с сервером
    uint _answerWait = 10000; //Время ожидания ответа от сервера
    uint _portListen = 123;
    bool _isConnected = false;
    ntp_package _ntpPackage;
    std::shared_ptr<DTime> _dt = nullptr;
    uint _ntpMode = NTP_CLIENT_MODE;
    //std::shared_ptr<TimerTimeDelayReseption> _timer = nullptr;
    std::unique_ptr<QThread> _timerThread = nullptr;
};