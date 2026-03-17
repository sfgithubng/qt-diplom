#ifndef DBHANDLER_H
#define DBHANDLER_H

#pragma once
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
//#include <QTableWidget>
#include <QTimer>
#include <QVector>

struct FlightData {
    QString flightNumber;
    QString dateTime;
    QString airportName;
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
    QVector<FlightData> FetchFlightsByAirportDirectionDate(QString flightScheduleType,
                                                           QString flightDate,
                                                           QString airportNameRu);
};

#endif // DBHANDLER_H
