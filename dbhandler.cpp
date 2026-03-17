#include "dbhandler.h"

DBHandler::DBHandler(QObject* parent = nullptr) {
    db = new QSqlDatabase();
    //*db = QSqlDatabase::database("QPSQL");
    *db = QSqlDatabase::addDatabase("QPSQL", "TST");
    db->setHostName("981757-ca08998.tmweb.ru");
    db->setPort(5432);
    db->setDatabaseName("demo");
    db->setUserName("netology_usr_cpp");
    db->setPassword("CppNeto3");

    dbConnIntervalMsec = 5000;
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
    //QSqlQuery* airportQuery = new QSqlQuery(db);
    //QSqlQuery airportQuery("SELECT airport_name FROM airports ORDER BY airport_name ASC", *db);
    QSqlQuery airportQuery("SELECT airport_name->>'ru' as airportName, airport_code FROM bookings.airports_data ORDER BY airportName ASC", *db);
    //airportQuery.exec();
    while (airportQuery.next()) {
        //qDebug() << "Airport: " << airportQuery.value(0).toString() << "\r\n";
        airportNames.push_back(airportQuery.value(0).toString());
    }

    return airportNames;
}

QVector<FlightData> DBHandler::FetchFlightsByAirportDirectionDate(QString flightScheduleType,
                                                                  QString flightDate,
                                                                  QString airportNameRu) {
    if (not db->isOpen()) {
        return QVector<FlightData>();
    }

    QVector<FlightData> flights{};

    auto airportQueryStr = QString("SELECT airport_code FROM bookings.airports_data WHERE airports_data.airport_name->>'ru' = '%1'")
        .arg(airportNameRu);
    QSqlQuery airportQuery(airportQueryStr, *db);
    airportQuery.next();
    auto airportCode = airportQuery.value(0).toString();

    QMap<QString, QString> flightScheduleTypeToAirportType{};
    flightScheduleTypeToAirportType.insert("scheduled_arrival", "arrival_airport");
    flightScheduleTypeToAirportType.insert("scheduled_departure", "departure_airport");
    auto airportType = flightScheduleTypeToAirportType.value(flightScheduleType);

    QMap<QString, QString> flightScheduleTypeToLinkAirportType{};
    flightScheduleTypeToLinkAirportType.insert("scheduled_arrival", "departure_airport");
    flightScheduleTypeToLinkAirportType.insert("scheduled_departure", "arrival_airport");
    auto linkAirportType = flightScheduleTypeToLinkAirportType.value(flightScheduleType);


    //auto flightsQueryStr = QString("SELECT flight_no, %1, ad.airport_name->>'ru' as airport_name from bookings.flights f ")
    //                       .arg(flightScheduleType);

    auto flightsQueryStr = QString("SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as airport_name from bookings.flights f ");
    flightsQueryStr += QString("JOIN bookings.airports_data ad on ad.airport_code = f.%1 ")
                           .arg(linkAirportType);
    flightsQueryStr += QString("where f.%1  = '%2' AND f.scheduled_departure BETWEEN DATE('%3') AND DATE('%3')+1 ORDER BY scheduled_departure")
                           .arg(airportType)
                           .arg(airportCode)
                           .arg(flightDate);
                           //.arg(flightScheduleType);
    QSqlQuery flightsQuery(flightsQueryStr, *db);
    while (flightsQuery.next()) {
        flights.push_back(FlightData{.flightNumber = flightsQuery.value(0).toString(),
                                     .dateTime = flightsQuery.value(1).toDateTime().toString("yyyy-MM-dd hh:mm:ss"),
                                     .airportName = flightsQuery.value(2).toString()});
        qDebug() << flightsQuery.value(0).toString() << " " << flightsQuery.value(1).toString() << " " << flightsQuery.value(2).toString() << "\r\n";
    }
    qDebug() << "--------------" << "\r\n";

    return flights;
}
