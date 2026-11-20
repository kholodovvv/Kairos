#include "include/network.hpp"
#include <QUdpSocket>
#include <QDateTime>
#include <cmath>
#include <QTimeZone>
#include <QTimer>

constexpr auto FROM_JAN1900_TO_JAN1970 = (2208988800UL);

    /*Network::Network(QWidget* parent) : QWidget(parent)
    {

    }*/

    QPair<bool, QString> Network::Connect(const QString& nameServer, const uint& port)
    {
        if (!_socket)
            SocketInit();

        if (_socket && !_socket->isOpen()) {
            port != 0 ? _socket->connectToHost(nameServer, port, QIODevice::ReadWrite) : _socket->connectToHost(nameServer, _portListen, QIODevice::ReadWrite);

            if (!_socket->waitForConnected(_connectionWait)) {
                _socket->close();
                return QPair(false, QString::fromLocal8Bit("Соединение с сервером не установлено!"));
            }

        }

        _isConnected = true;
        return QPair(true, QString::fromLocal8Bit("Соединение установлено!"));
    }

    void Network::SetConnectionWait(const uint& sec)
    {
        _connectionWait = sec;
    }

    void Network::SetAnswerWait(const uint& sec)
    {
        _answerWait = sec;
    }

    void Network::SetPortListen(const uint& port)
    {
        _portListen = port;
    }

    void Network::SetNTPMode(const uint& ntpMode)
    {
        switch (ntpMode)
        {
        case NTP_CLIENT_MODE:
            _ntpMode = NTP_CLIENT_MODE;
            break;

        case NTP_BROADCAST_MODE:
            _ntpMode = NTP_BROADCAST_MODE;
            break;
        }
    }

    QPair<bool, QString> Network::Disconnect()
    {
        onDisconnected();
        return QPair(true, QString::fromLocal8Bit("Соединение закрыто!"));
    }

    QPair<bool, QString> Network::SendMessage()
    {
        QByteArray ntp_package;
        ntp_package = _ntpPackage.PackageToByte();

        if (_socket && _socket->isOpen() && _isConnected) {
            //if (_timer != nullptr)
                //emit startTimerTimeDelayReceiv(1);
                //_timer->start(1); //Запуск счетчика времени
            _time_send = std::chrono::steady_clock::now();

            if(_socket->write(ntp_package, ntp_package.length()) == ntp_package.length())
                return QPair(true, QString::fromLocal8Bit("Запрос успешно отправлен!"));
            else
                return QPair(false, QString::fromLocal8Bit("Неудачная попытка отправки запроса серверу!"));
            //_ntpPackage.originTimestamp = _dt->GetCurrentTimeUTC().first; //Фиксируем время отправки сообщения серверу
            //_ntpPackage.originClientTimestamp = _ntpPackage.originTimestamp;

            //if (_socket->waitForReadyRead(_answerWait))
            //    return QPair(true, QString::fromLocal8Bit("Запрос успешно отправлен!")); //<-- Исправить!!!

        }

        return QPair(false, QString::fromLocal8Bit("Невозможно отправить запрос серверу. Соединение разорвано!"));
    }

    void Network::onDisconnected()
    {
        if (_socket && _socket->isOpen()) {
            _socket->close();
            _isConnected = false;
        }
    }

    //void Network::onTimeDelayReceiv(const int &timeDelay)
    //{
    //    _ntpPackage.offsetTimestamp = timeDelay / 1000;
    //    
    //    if(_timerThread->isRunning())
    //        _timerThread->quit();
    //}

 //   void Network::onnTimerAlarm()
 //   {
 //       _ntpPackage.receiveClientTimeCounter += 1;
 //   }

    void Network::onReadData()
    {
        QByteArray buf;
        _time_receiv = std::chrono::steady_clock::now();

        if (_ntpMode == NTP_CLIENT_MODE && _socket != nullptr) {
            while (_socket->bytesAvailable() > 0) {
                buf = _socket->readAll();
            }

            if (buf.length() == 48) {
                //if (_timer != nullptr)
                    //emit stopTimerTimeDelayReceiv();
                    //_timer->stop(); //Фиксируем время получения пакета, оставливаем счетчик
                ParseAndCheckReceiveNtpPackage(buf);
            }
        }
        else if (_ntpMode == NTP_BROADCAST_MODE && _socketListen != nullptr) {
            while (_socketListen->bytesAvailable() > 0) {
                buf = _socketListen->readAll();
            }

            if (buf.length() == 48) {
                //emit stopTimerTimeDelayReceiv();
                //_timer->stop(); //Фиксируем время получения пакета, оставливаем счетчик
                ParseAndCheckReceiveNtpPackage(buf);
            }
        }
    }

    void Network::SocketInit()
    {
        if (_ntpMode == NTP_CLIENT_MODE) {
            if (!_socket) {
                _socket = std::make_shared<QUdpSocket>(new QUdpSocket);

                connect(_socket.get(), &QUdpSocket::readyRead, this, &Network::onReadData);
                connect(_socket.get(), &QUdpSocket::disconnected, this, &Network::onDisconnected);
            }
        }
        else if (_ntpMode == NTP_BROADCAST_MODE) {
            if (!_socketListen) {
                _socketListen = std::make_shared<QUdpSocket>(new QUdpSocket);
                _socketListen->bind(_portListen);

                connect(_socketListen.get(), &QUdpSocket::readyRead, this, &Network::onReadData);
                connect(_socketListen.get(), &QUdpSocket::disconnected, this, &Network::onDisconnected);
            }
        }

        if (_dt == nullptr)
            _dt = std::make_shared<DTime>(new DTime());

        //if (_timer == nullptr) {
        //    _timer = std::make_shared<TimerTimeDelayReseption>(new TimerTimeDelayReseption());
        //    _timer->Init();
            //_timer->setTimerType(Qt::PreciseTimer);
            //connect(_timer.get(), &QTimer::timeout, this, &Network::onnTimerAlarm);
        //}

        //if (_timerThread == nullptr) {
        //    _timerThread = std::make_unique<QThread>();
        //    _timer->moveToThread(_timerThread.get());

        //    //connect(_timer.get(), &TimerTimeDelayReseption::sTimeDelayReseption, this, &Network::onTimeDelayReceiv, Qt::AutoConnection);
        //    connect(this, &Network::startTimerTimeDelayReceiv, _timer.get(), &TimerTimeDelayReseption::onStart, Qt::AutoConnection);
        //    connect(this, &Network::stopTimerTimeDelayReceiv, _timer.get(), &TimerTimeDelayReseption::onStop, Qt::AutoConnection);
        //    //connect(this, &Network::getTimeDelayReceiv, _timer.get(), &TimerTimeDelayReseption::onGetTime, Qt::AutoConnection);

        //    _timerThread->start();
        //}
    }

    void Network::ParseAndCheckReceiveNtpPackage(const QByteArray& bArray)
    {
        _ntpPackage.leapIndicator = quint8(bArray[0] >> 6 & 0x3);
        _ntpPackage.versionNumber = quint8(bArray[0] >> 3 & 0x7);
        _ntpPackage.mode = quint8(bArray[0] & 0x7);
        _ntpPackage.stratum = quint8(bArray[1]);
        _ntpPackage.poll = quint8(bArray[2]);
        _ntpPackage.precision = quint8(bArray[3]);
        _ntpPackage.rootDelay = _ntpPackage.ConvertByteToUInt32(bArray.mid(4, 4));
        _ntpPackage.rootDispersion = _ntpPackage.ConvertByteToUInt32(bArray.mid(8, 4));
        _ntpPackage.referenceID = _ntpPackage.ConvertByteToUInt32(bArray.mid(12, 4));
        _ntpPackage.refTimestamp = _ntpPackage.ConvertByteToUInt32(bArray.mid(16, 8));
        _ntpPackage.originTimestamp = _ntpPackage.ConvertByteToUInt32(bArray.mid(24, 8));
        _ntpPackage.receiveTimestamp = _ntpPackage.ConvertByteToUInt32(bArray.mid(32, 8));
        _ntpPackage.transmitTimestamp = _ntpPackage.ConvertByteToUInt32(bArray.mid(40, 8));

        _ntpPackage.offsetTimestamp = std::chrono::duration_cast<std::chrono::microseconds>(_time_receiv - _time_send).count() / 1000; //Задержка канала
        qint64 offsetReceive = _ntpPackage.offsetTimestamp / 2; //Задержка канала при получении
        //qint64 offsetTransmit = _ntpPackage.receiveTimestamp - _ntpPackage.originTimestamp; //Задержка канала при отправке
        //qint64 offsetReceive = _ntpPackage.transmitTimestamp - _ntpPackage.receiveClientTimestamp; //Задержка канала при получении
        //_ntpPackage.receiveClientTimeCounter = _timer->GetTime();
        //_timerThread->quit();
        //_timerThread->deleteLater();
        //emit getTimeDelayReceiv();
        //_timerThread->wait(5000);

        //if(_ntpPackage.receiveClientTimeCounter != 0)
        //    _ntpPackage.offsetTimestamp = _ntpPackage.transmitTimestamp - ((_ntpPackage.receiveClientTimeCounter / 2) / 1000);

        if (_ntpPackage.leapIndicator == 3)
            _ntpPackage.note = QString::fromLocal8Bit("Не следует доверять данным данного сервера!");

        if (_ntpPackage.mode != 4 && _ntpPackage.mode != 5)
            _ntpPackage.note += QString::fromLocal8Bit("Не корректный режим работы сервера!");

        if (_ntpPackage.stratum > 15)
            _ntpPackage.note += QString::fromLocal8Bit("Не следует доверять данным данного сервера! Страта > 15, слишком много посредников.");

        if (_ntpPackage.refTimestamp == 0)
            _ntpPackage.note += QString::fromLocal8Bit("Сервер не передал своё текущее время!");

        emit answerForServer(QString::fromLocal8Bit("\r\n\r\n Содержание полученного NTP пакета:\r\n"
            "Leap indicator = %1; <-- Число, предупреждающее о секунде координации;\r\n"
            "Version number = %2; <-- Номер версии протокола NTP;\r\n"
            "Mode = %3; <-- Режим работы сервера;\r\n"
            "Stratum = %4; <-- Кол-во посредников между клиентом и эталонными часами;\r\n"
            "Poll = %5; <-- Число, задающее интервал в секундах между последовательными обращениями;\r\n"
            "Precision = %6; <-- Точность локальных системных часов;\r\n"
            "Root delay = %7; <-- Задержка сервера;\r\n"
            "Root dispersion = %8; <-- Разброс показаний сервера;\r\n"
            "RefID = %9; <-- ID часов;\r\n"
            "Reference = %10; UTC(%11) <-- Время сервера;\r\n"
            "Originate = %12; UTC(%13) <-- Время, отправки запроса, по версии сервера;\r\n"
            "Receive = %14; UTC(%15) <-- Время получения запроса сервером;\r\n"
            "Transmit = %16; UTC(%17) <-- Время отправки ответа сервера клиенту.\r\n\r\n"
            "Примечание: %18;\r\n"
            "Задержка канала (мс) = %19").
            arg(_ntpPackage.leapIndicator).arg(_ntpPackage.versionNumber).arg(_ntpPackage.mode).arg(_ntpPackage.stratum).
            arg(_ntpPackage.poll).arg(_ntpPackage.precision).arg(_ntpPackage.rootDelay).arg(_ntpPackage.rootDispersion).
            arg(_ntpPackage.referenceID).arg(_ntpPackage.refTimestamp).arg(_dt->ConvertInt64ToTimeUTC(_ntpPackage.refTimestamp)).
            arg(_ntpPackage.originTimestamp).arg(_dt->ConvertInt64ToTimeUTC(_ntpPackage.originTimestamp)).
            arg(_ntpPackage.receiveTimestamp).arg(_dt->ConvertInt64ToTimeUTC(_ntpPackage.receiveTimestamp)).
            arg(_ntpPackage.transmitTimestamp).arg(_dt->ConvertInt64ToTimeUTC(_ntpPackage.transmitTimestamp)).
            arg(_ntpPackage.note).arg(_ntpPackage.offsetTimestamp));
    }

    Network::~Network()
    {

    }

    QByteArray ntp_package::PackageToByte()
    {
        QByteArray buff(48, 0);

        buff[0] = (leapIndicator << 6) | (versionNumber << 3) | mode;
        buff[1] = stratum & 0xff;
        buff[2] = poll & 0xff;
        buff[3] = precision & 0xff;

        if (_dt == nullptr)
            _dt = std::make_shared<DTime>(new DTime());
        
        QPair<qint64, qint64> timeStamp = _dt->GetCurrentTimeUTC();

        QByteArray currTimeUTC = ConvertTimeToByte(timeStamp.first, timeStamp.second); 

        std::memcpy(buff.data() + 40, currTimeUTC.data(), 8); //Можно не заполнять текущее время. Сервер свою очередь, тоже не будет запонять его. (Originate)

        return buff;
    }

    QByteArray ntp_package::ConvertTimeToByte(const qint64& sec, const qint64& msec)
    {
        double intpart;
        qint64 fracSec = modf((double)msec / 1000, &intpart) * 1000; //Выделение дробной части секунд

        QByteArray buff;
        buff[0] = sec >> 24 & 0xff;
        buff[1] = sec >> 16 & 0xff;
        buff[2] = sec >> 8 & 0xff;
        buff[3] = sec & 0xff;

        buff[4] = fracSec >> 24 & 0xff;
        buff[5] = fracSec >> 16 & 0xff;
        buff[6] = fracSec >> 8 & 0xff;
        buff[7] = fracSec & 0xff;

        return buff;
    }

    QString DTime::GetTimeZoneID()
    {
        return QTimeZone::systemTimeZone().id();
    }

    int DTime::GetTimeOffsetFromUTC()
    {
        QTimeZone tz;
        return tz.offsetFromUtc(QDateTime::currentDateTime());
    }

    QPair<qint64, qint64> DTime::GetCurrentTimeUTC()
    {
        QDateTime dt = QDateTime::currentDateTimeUtc();
        qint64 sec = FROM_JAN1900_TO_JAN1970 + dt.currentSecsSinceEpoch(); //Количество секунд (UTC)
        qint64 msec = sec * 1000; //Количество миллисекунд (UTC)

        return QPair<qint64, qint64>(sec, msec);
    }

    quint32 ntp_package::ConvertByteToUInt32(QByteArray bArray) {
        quint32 value = 0;

        for (int i = 0; i < 4; i++) {
            value = (value << 8) + (bArray[i] & 0xff);
        }

        return value;
    }

    QString DTime::ConvertInt64ToTimeUTC(const qint64 &tSource)
    {
        QString sec = QString::number(int(tSource % 60)).toLocal8Bit();
        QString min = QString::number(int((tSource % 3600) / 60)).toLocal8Bit();
        QString hour = QString::number(int((tSource % 86400L) / 3600)).toLocal8Bit();

        return QString::fromLocal8Bit("%1:%2:%3").arg(hour).arg(min).arg(sec);
    }

    std::string DTime::ConvertUInt64ToTimeUTC(const qint64 &timestamp)
    {
        auto tp = std::chrono::sys_time<std::chrono::seconds>(
            std::chrono::seconds(timestamp));
        return std::format("{:%d.%m.%Y %H:%M:%S}", tp); // UTC
    }

    //void TimerTimeDelayReseption::Init()
    //{
    //    /*if (_timer == nullptr)
    //        _timer = std::make_shared<QTimer>();

    //    connect(_timer.get(), &QTimer::timeout, this, &TimerTimeDelayReseption::onCounterTime);*/
    //}

    //void TimerTimeDelayReseption::timerEvent(QTimerEvent *event)
    //{
    //    _time += _delay;
    //}

    //void TimerTimeDelayReseption::onStart(const int &delay)
    //{
    //    _delay = delay;
    //    _idTimer = this->startTimer(_delay);
    //    /*if (_timer != nullptr) {
    //        _delay = delay;
    //        _timer->start(_delay);
    //    }*/
    //}

    //void TimerTimeDelayReseption::onStop()
    //{
    //    this->killTimer(_idTimer);
    //    /*if (_timer != nullptr)
    //        _timer->stop();*/
    //}

    //void TimerTimeDelayReseption::onReset()
    //{
    //    _time = 0;
    //}

    //void TimerTimeDelayReseption::onGetTime()
    //{
    //    emit sTimeDelayReseption(_time);
    //}

    //int TimerTimeDelayReseption::GetTime()
    //{
    //    return _time;
    //}

    //void TimerTimeDelayReseption::onCounterTime() 
    //{
    //    //_time += _delay;
    //}