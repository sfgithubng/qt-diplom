#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Инспектор аэропортов");
    ui->airportSelectorLabel->setText("Выбор аэропорта");
    ui->directionSelectorLabel->setText("Выбор направления");
    ui->directionSelector->addItem("Вылет");
    ui->directionSelector->addItem("Прилет");
    ui->directionSelector->setCurrentIndex(-1);    
    ui->directionSelector->setEnabled(false);
    ui->dateSelector->setEnabled(false);
    ui->dateSelectorLabel->setText("Выбор даты");
    ui->dateSelector->setMinimumDate(QDate(2016, 8, 15));
    ui->dateSelector->setMaximumDate(QDate(2017, 9, 14));
    ui->getFlightsButton->setEnabled(false);
    ui->getFlightsButton->setText("Получить данные");
    ui->flightsTable->setRowCount(0);
    ui->flightsTable->setColumnCount(3);
    ui->flightsTable->setEnabled(false);
    ui->flightsTable->setVisible(false);

    flightSelectionInitialized = false;

    DBConnectUIUpdate(false);
    dbHandler = new DBHandler(this);
    QObject::connect(dbHandler, &DBHandler::ConnectedToDB, this, &MainWindow::DBConnectUIUpdate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DBConnectUIUpdate(bool connectionStatus) {
    QMap<bool, QString> dbConnStatusToLabel{};
    dbConnStatusToLabel.insert(true, "Подключено к БД");
    dbConnStatusToLabel.insert(false, "Отключено от БД");

    QMap<bool, QString> dbConnStatusToLabelStyle{};
    dbConnStatusToLabelStyle.insert(true, "color:green");
    dbConnStatusToLabelStyle.insert(false, "color:red");


    ui->dbConnStatusLabel->setText(dbConnStatusToLabel.value(connectionStatus));
    ui->dbConnStatusLabel->setStyleSheet(dbConnStatusToLabelStyle.value(connectionStatus));
    ui->airportSelector->setEnabled(connectionStatus);
    ui->airportSelectorLabel->setEnabled(connectionStatus);
    ui->directionSelectorLabel->setEnabled(connectionStatus);
    ui->dateSelectorLabel->setEnabled(connectionStatus);

    if (connectionStatus == true) {
        PopulateAirports();
    }

}

void MainWindow::PopulateAirports() {
    QVector<QString> airportNames = dbHandler->FetchAirportNames();
    for (const auto& airportName : airportNames) {
        ui->airportSelector->addItem(airportName);
    }
    ui->airportSelector->setCurrentIndex(-1);
}

void MainWindow::on_directionSelector_currentIndexChanged(int index)
{
    if (index == -1) {
        ui->dateSelector->setEnabled(false);
        return;
    }
    ui->dateSelector->setEnabled(true);

    if (flightSelectionInitialized) {
        ui->getFlightsButton->setEnabled(true);
    }
}


void MainWindow::on_airportSelector_currentIndexChanged(int index)
{
    if (index == -1) {
        ui->directionSelector->setEnabled(false);
        return;
    }
    ui->directionSelector->setEnabled(true);

    if (flightSelectionInitialized) {
        ui->getFlightsButton->setEnabled(true);
    }
}


void MainWindow::on_dateSelector_selectionChanged()
{
    ui->getFlightsButton->setEnabled(true);
    flightSelectionInitialized = true;
}


void MainWindow::on_getFlightsButton_clicked()
{
    QString flightScheduleType;
    QStringList flightTableHeaders;
    if (ui->directionSelector->currentText() == "Вылет") {
        flightScheduleType = "scheduled_departure";
        flightTableHeaders = QStringList{"Номер рейса", "Время вылета", "Аэропорт назначения"};
    }
    if (ui->directionSelector->currentText() == "Прилет") {
        flightScheduleType = "scheduled_arrival";
        flightTableHeaders = QStringList{"Номер рейса", "Время вылета", "Аэропорт отправления"};
    }
    ui->flightsTable->setHorizontalHeaderLabels(flightTableHeaders);
    ui->flightsTable->setVisible(true);

    QString flightDate{ui->dateSelector->selectedDate().toString()};
    QString airportNameRu{ui->airportSelector->currentText()};
    auto flights = dbHandler->FetchFlightsByAirportDirectionDate(
        flightScheduleType,
        flightDate,
        airportNameRu);

    ui->flightsTable->setRowCount(0);
    ui->flightsTable->setEnabled(true);
    uint32_t rowCount{0};
    for (const auto& flight : flights) {
        ui->flightsTable->insertRow(rowCount++);
        QTableWidgetItem* flightNumberItem = new QTableWidgetItem(flight.flightNumber);
        ui->flightsTable->setItem(rowCount - 1, 0, flightNumberItem);
        QTableWidgetItem* departureTimeItem = new QTableWidgetItem(flight.dateTime);
        ui->flightsTable->setItem(rowCount - 1, 1, departureTimeItem);
        QTableWidgetItem* airportNameItem = new QTableWidgetItem(flight.airportName);
        ui->flightsTable->setItem(rowCount - 1, 2, airportNameItem);
    }
    ui->getFlightsButton->setEnabled(false);
}


void MainWindow::on_flightStatsButton_clicked()
{

}

