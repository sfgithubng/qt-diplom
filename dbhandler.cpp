#include "dbhandler.h"

DBHandler::DBHandler(QObject* parent = nullptr) {
    db = new QSqlDatabase();
    *db = QSqlDatabase::addDatabase("QPSQL", "Flights");
    db->setHostName("981757-ca08998.tmweb.ru");
    db->setPort(5432);
    db->setDatabaseName("demo");
    db->setUserName("netology_usr_cpp");
    db->setPassword("CppNeto3");

    dbConnIntervalMsec = 100;
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &DBHandler::ConnectToDB);    
    timer->start(dbConnIntervalMsec);
}

DBHandler::~DBHandler() {
    db->close();
    delete db;
    delete timer;
}

void DBHandler::ConnectToDB() {
    timer->stop();
    if (not db->open()) {
        QString connErrStr("Ошибка установки соединения с хостом БД. ");
        connErrStr += QString("Причина: %1").arg(db->lastError().text());
        QMessageBox connErrMsg;
        connErrMsg.setText(connErrStr);
        connErrMsg.exec();
        dbConnIntervalMsec = 5000;
        timer->start(dbConnIntervalMsec);
    } else {
        emit ConnectedToDB(true);
    }
}

QVector<QString> DBHandler::FetchAirportNames() {
    if (not db->isOpen()) {
        return QVector<QString>();
    }

    QVector<QString> airportNames{};
    QSqlQuery airportQuery("SELECT airport_name->>'ru' as airportName, airport_code FROM bookings.airports_data ORDER BY airportName ASC", *db);
    while (airportQuery.next()) {
        airportNames.push_back(airportQuery.value(0).toString());
    }

    return airportNames;
}

QString DBHandler::FetchAirportCodeByNameRu(QString airportNameRu) {
    auto airportQueryStr = QString("SELECT airport_code FROM bookings.airports_data WHERE airports_data.airport_name->>'ru' = '%1'")
                               .arg(airportNameRu);
    QSqlQuery airportQuery(airportQueryStr, *db);
    airportQuery.next();
    auto airportCode = airportQuery.value(0).toString();
    return airportCode;
}

QVector<FlightData> DBHandler::FetchFlightsByAirportDirectionDate(QString flightScheduleType,
                                                                  QString flightDate,
                                                                  QString airportNameRu) {
    if (not db->isOpen()) {
        return QVector<FlightData>();
    }

    QVector<FlightData> flights{};

    auto airportCode = FetchAirportCodeByNameRu(airportNameRu);

    QMap<QString, QString> flightScheduleTypeToAirportType{};
    flightScheduleTypeToAirportType.insert("scheduled_arrival", "arrival_airport");
    flightScheduleTypeToAirportType.insert("scheduled_departure", "departure_airport");
    auto airportType = flightScheduleTypeToAirportType.value(flightScheduleType);

    QMap<QString, QString> flightScheduleTypeToLinkAirportType{};
    flightScheduleTypeToLinkAirportType.insert("scheduled_arrival", "departure_airport");
    flightScheduleTypeToLinkAirportType.insert("scheduled_departure", "arrival_airport");
    auto linkAirportType = flightScheduleTypeToLinkAirportType.value(flightScheduleType);

    auto flightsQueryStr = QString("SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as airport_name from bookings.flights f ");
    flightsQueryStr += QString("JOIN bookings.airports_data ad on ad.airport_code = f.%1 ")
                           .arg(linkAirportType);
    flightsQueryStr += QString("where f.%1  = '%2' AND f.scheduled_departure BETWEEN DATE('%3') AND DATE('%3')+1 ORDER BY scheduled_departure")
                           .arg(airportType)
                           .arg(airportCode)
                           .arg(flightDate);
    QSqlQuery flightsQuery(flightsQueryStr, *db);
    while (flightsQuery.next()) {
        flights.push_back(FlightData{.flightNumber = flightsQuery.value(0).toString(),
                                     .dateTime = flightsQuery.value(1).toDateTime().toString("yyyy-MM-dd hh:mm:ss"),
                                     .airportName = flightsQuery.value(2).toString()});
    }

    return flights;
}


AnnualFlightStats DBHandler::FetchAnnualFlightCountByAirportDirectionYear(FlightDirection flightDirection,
                                                                        QString flightYear,
                                                                        QString airportNameRu) {
    if (not db->isOpen()) {
        return AnnualFlightStats();
    }

    AnnualFlightStats annualFlightStats{};

    auto airportCode = FetchAirportCodeByNameRu(airportNameRu);

    QMap<FlightDirection, QString> flightDirectionToStr{};
    flightDirectionToStr.insert(FlightDirection::arrival, "arrival");
    flightDirectionToStr.insert(FlightDirection::departure, "departure");
    auto flightDirectionStr = flightDirectionToStr.value(flightDirection);

    auto flightStatsQueryStr = QString("SELECT to_char(scheduled_departure, 'YYYY-MM') as month, COUNT(flight_no) from bookings.flights f ");
    flightStatsQueryStr += QString("WHERE f.%1_airport = '%2' AND f.scheduled_%1 BETWEEN DATE('%3-01-01') AND DATE('%3-12-31')+1 GROUP BY month ORDER BY month")
                            .arg(flightDirectionStr)
                            .arg(airportCode)
                            .arg(flightYear);
    QSqlQuery flightStatsQuery(flightStatsQueryStr, *db);
    while (flightStatsQuery.next()) {
        annualFlightStats.insert(flightStatsQuery.value(0).toString(),
                                 flightStatsQuery.value(1).toInt());
    }

    return annualFlightStats;
}

MonthlyFlightStats DBHandler::FetchMonthlyFlightCountByAirportDirectionYear(FlightDirection flightDirection,
                                                                 QString flightYear,
                                                                 QString flightMonth,
                                                                 QString daysInMonth,
                                                                 QString airportNameRu) {
    if (not db->isOpen()) {
        return MonthlyFlightStats();
    }

    MonthlyFlightStats monthlyFlightStats{};

    auto airportCode = FetchAirportCodeByNameRu(airportNameRu);

    QMap<FlightDirection, QString> flightDirectionToStr{};
    flightDirectionToStr.insert(FlightDirection::arrival, "arrival");
    flightDirectionToStr.insert(FlightDirection::departure, "departure");
    auto flightDirectionStr = flightDirectionToStr.value(flightDirection);

    auto flightStatsQueryStr = QString("SELECT to_char(scheduled_departure, 'YYYY-MM-DD') as day, COUNT(flight_no) from bookings.flights f ");
    flightStatsQueryStr += QString("WHERE f.%1_airport = '%2' AND f.scheduled_%1 BETWEEN DATE('%3-%4-01') AND DATE('%3-%4-%5')+1 GROUP BY day ORDER BY day")
                               .arg(flightDirectionStr)
                               .arg(airportCode)
                               .arg(flightYear)
                               .arg(flightMonth)
                               .arg(daysInMonth);
    QSqlQuery flightStatsQuery(flightStatsQueryStr, *db);
    while (flightStatsQuery.next()) {
        monthlyFlightStats.insert(flightStatsQuery.value(0).toString(),
                                 flightStatsQuery.value(1).toInt());
    }

    return monthlyFlightStats;
}
