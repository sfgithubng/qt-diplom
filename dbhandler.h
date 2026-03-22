#ifndef DBHANDLER_H
#define DBHANDLER_H

#pragma once
#include <QDateTime>
#include <QDebug>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QVector>

struct FlightData {
    QString flightNumber;
    QString dateTime;
    QString airportName;
};


using AnnualFlightStats = QMap<QString, int>;
using MonthlyFlightStats = QMap<QString, int>;

enum class FlightDirection {
    arrival,
    departure
};

class DBHandler : public QObject {
    Q_OBJECT
private:
    QSqlDatabase* db;
    uint32_t dbConnIntervalMsec;
    QTimer* timer;
    void ConnectToDB();
signals:
    void ConnectedToDB(bool connectionStatus);

public:
    explicit DBHandler(QObject* parent);
    ~DBHandler();
    QVector<QString> FetchAirportNames();
    QString FetchAirportCodeByNameRu(QString airportNameRu);
    QVector<FlightData> FetchFlightsByAirportDirectionDate(QString flightScheduleType,
                                                           QString flightDate,
                                                           QString airportNameRu);
    AnnualFlightStats FetchAnnualFlightCountByAirportDirectionYear(FlightDirection flightDirection,
                                                           QString flightYear,
                                                           QString airportNameRu);
    MonthlyFlightStats FetchMonthlyFlightCountByAirportDirectionYear(FlightDirection flightDirection,
                                                                     QString flightYear,
                                                                     QString flightMonth,
                                                                     QString daysInMonth,
                                                                     QString airportNameRu);
};

#endif // DBHANDLER_H
