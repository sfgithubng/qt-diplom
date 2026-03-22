#include "flightstatswindow.h"
#include "ui_flightstatswindow.h"

FlightStatsWindow::FlightStatsWindow(DBHandler* constrDbHandler,
                                     QWidget* parent) :
    QWidget(parent),
    ui(new Ui::FlightStatsWindow)
{
    annualChartNeedsRefresh = false;
    monthlyChartNeedsRefresh = false;
    ui->setupUi(this);
    ui->closeWindowButton->setText("Закрыть");
    dbHandler = constrDbHandler;

    ui->statsTabs->setTabText(0, "Загруженность за год");
    ui->statsTabs->setTabText(1, "Загруженность за месяц");
    ui->annualStatsYearLabel->setText("Год");
    ui->monthlyStatsYearLabel->setText("Год");
    ui->monthlyStatsMonthLabel->setText("Месяц");
    daysPerMonth.insert("01", 31);
    daysPerMonth.insert("02", 0);
    daysPerMonth.insert("03", 31);
    daysPerMonth.insert("04", 30);
    daysPerMonth.insert("05", 31);
    daysPerMonth.insert("06", 30);
    daysPerMonth.insert("07", 31);
    daysPerMonth.insert("08", 31);
    daysPerMonth.insert("09", 30);
    daysPerMonth.insert("10", 31);
    daysPerMonth.insert("11", 30);
    daysPerMonth.insert("12", 31);
    years = QStringList{"2016",
                      "2017"};
    months = QStringList{"Январь",
                       "Февраль",
                       "Март",
                       "Апрель",
                       "Май",
                       "Июнь",
                       "Июль",
                       "Август",
                       "Сентябрь",
                       "Октябрь",
                       "Ноябрь",
                       "Декабрь"};
    ui->annualStatsYear->addItems(years);
    ui->monthlyStatsYear->addItems(years);
    ui->monthlyStatsMonth->addItems(months);
    defaultYear = years.at(0);
    defaultMonth = months.at(0);
}

FlightStatsWindow::~FlightStatsWindow()
{
    ui->annualStatsLayout->removeWidget(annualChartView);
    delete annualArrivalsSet;
    delete annualDeparturesSet;
    delete annualOverallSet;
    delete annualAxisX;
    delete annualAxisY;
    delete annualSeries;
    delete annualChart;
    delete annualChartView;
    delete monthlyAxisX;
    delete monthlyAxisY;
    delete monthlyArrivalsSeries;
    delete monthlyDeparturesSeries;
    delete monthlyOverallSeries;
    delete monthlyChart;
    delete monthlyChartView;
    delete ui;
}

void FlightStatsWindow::SetAirportName(QString airportName) {
    ui->airportNameDisplay->setText(airportName);
}

void FlightStatsWindow::closeEvent(QCloseEvent *event) {
    emit FormClosed(true);
    event->accept();
}

void FlightStatsWindow::on_closeWindowButton_clicked()
{
    this->close();
}

void FlightStatsWindow::RefreshAnnualFlightStats() {
    auto arrivalAnnualFlightStats = dbHandler->FetchAnnualFlightCountByAirportDirectionYear(FlightDirection::arrival,
                                                                                            ui->annualStatsYear->currentText(),
                                                                                            ui->airportNameDisplay->text());
    auto departureAnnualFlightStats = dbHandler->FetchAnnualFlightCountByAirportDirectionYear(FlightDirection::departure,
                                                                                            ui->annualStatsYear->currentText(),
                                                                                            ui->airportNameDisplay->text());
    if (annualChartNeedsRefresh) {
        ui->annualStatsLayout->removeWidget(annualChartView);
        delete annualArrivalsSet;
        delete annualDeparturesSet;
        delete annualOverallSet;
        delete annualAxisX;
        delete annualAxisY;
        delete annualSeries;
        delete annualChart;
        delete annualChartView;
    }
    annualArrivalsSet = new QBarSet("Прилеты");
    annualDeparturesSet = new QBarSet("Вылеты");
    annualOverallSet = new QBarSet("Общее");

    int defaultFlightCount{0};
    int monthlyArrivedFlightCount{0};
    int monthlyDepartedFlightCount{0};
    int monthlyOverallFlightCount{0};
    int maxMonthlyFlights{0};
    QString yearAndMonth{};
    for (uint32_t month = 1; month <= 12; ++month) {
        yearAndMonth = QString("%1-%2")
                           .arg(ui->annualStatsYear->currentText())
                           .arg(month, 2, 10, QChar('0'));
        monthlyArrivedFlightCount = arrivalAnnualFlightStats.value(yearAndMonth, defaultFlightCount);
        monthlyDepartedFlightCount = departureAnnualFlightStats.value(yearAndMonth, defaultFlightCount);
        monthlyOverallFlightCount = monthlyArrivedFlightCount + monthlyDepartedFlightCount;
        if (monthlyOverallFlightCount > maxMonthlyFlights) {
            maxMonthlyFlights = monthlyOverallFlightCount;
        }
        *annualArrivalsSet << monthlyArrivedFlightCount;
        *annualDeparturesSet << monthlyDepartedFlightCount;
        *annualOverallSet << monthlyOverallFlightCount;
    }
    annualSeries = new QBarSeries;
    annualSeries->append(annualArrivalsSet);
    annualSeries->append(annualDeparturesSet);
    annualSeries->append(annualOverallSet);
    annualChart = new QChart;
    annualChart->addSeries(annualSeries);

    annualChart->setTitle(QString("Рейсы за %1 год").arg(ui->annualStatsYear->currentText()));
    annualAxisX = new QBarCategoryAxis;
    annualAxisX->append(months);
    annualChart->addAxis(annualAxisX, Qt::AlignBottom);
    annualSeries->attachAxis(annualAxisX);
    annualAxisY = new QValueAxis;
    auto axisYRange = qRound(maxMonthlyFlights * 1.1);
    if (axisYRange / 10 > 1) {
        annualAxisY->setTickCount(10);
    } else {
        annualAxisY->setTickCount(axisYRange);
    }
    annualAxisY->setLabelFormat("%d");
    annualChart->addAxis(annualAxisY, Qt::AlignLeft);
    annualSeries->attachAxis(annualAxisY);
    annualChartView = new QChartView(annualChart);
    annualChartView->setParent(ui->annualStatsTab);
    annualChartView->show();
    ui->annualStatsLayout->addWidget(annualChartView);
    annualChartNeedsRefresh = true;
}

void FlightStatsWindow::RefreshMonthlyFlightStats() {
    if ((ui->monthlyStatsYear->currentText().toInt() % 4) == 0) {
        daysPerMonth["02"] = 29;
    } else {
        daysPerMonth["02"] = 28;
    }
    auto monthString = QString("%1").arg(months.indexOf(ui->monthlyStatsMonth->currentText()) + 1, 2, 10, QChar('0'));
    auto daysInMonthString = QString("%1").arg(daysPerMonth.value(monthString));
    auto arrivalMonthlyFlightStats = dbHandler->FetchMonthlyFlightCountByAirportDirectionYear(FlightDirection::arrival,
                                                                                              ui->monthlyStatsYear->currentText(),
                                                                                              monthString,
                                                                                              daysInMonthString,
                                                                                              ui->airportNameDisplay->text());
    auto departureMonthlyFlightStats = dbHandler->FetchMonthlyFlightCountByAirportDirectionYear(FlightDirection::departure,
                                                                                              ui->monthlyStatsYear->currentText(),
                                                                                              monthString,
                                                                                              daysInMonthString,
                                                                                              ui->airportNameDisplay->text());
    if (monthlyChartNeedsRefresh) {
        ui->monthlyStatsLayout->removeWidget(monthlyChartView);
        delete monthlyAxisX;
        delete monthlyAxisY;
        delete monthlyArrivalsSeries;
        delete monthlyDeparturesSeries;
        delete monthlyOverallSeries;
        delete monthlyChart;
        delete monthlyChartView;
    }
    monthlyArrivalsSeries = new QLineSeries();
    monthlyDeparturesSeries = new QLineSeries();
    monthlyOverallSeries = new QLineSeries();
    int defaultFlightCount{0};
    int dailyArrivedFlightCount{0};
    int dailyDepartedFlightCount{0};
    int dailyOverallFlightCount{0};
    int maxDailyFlights{0};
    QString yearMonthDay{};
    for (uint32_t day = 1; day <= daysPerMonth.value(monthString); ++day) {
        yearMonthDay = QString("%1-%2-%3")
                           .arg(ui->monthlyStatsYear->currentText())
                           .arg(monthString)
                           .arg(day, 2, 10, QChar('0'));
        dailyArrivedFlightCount = arrivalMonthlyFlightStats.value(yearMonthDay, defaultFlightCount);
        dailyDepartedFlightCount = departureMonthlyFlightStats.value(yearMonthDay, defaultFlightCount);
        dailyOverallFlightCount = dailyArrivedFlightCount + dailyDepartedFlightCount;
        if (dailyOverallFlightCount > maxDailyFlights) {
            maxDailyFlights = dailyOverallFlightCount;
        }
        *monthlyArrivalsSeries << QPointF(day, dailyArrivedFlightCount);
        *monthlyDeparturesSeries << QPointF(day, dailyDepartedFlightCount);
        *monthlyOverallSeries << QPointF(day, dailyOverallFlightCount);
    }

    monthlyChart = new QChart;
    monthlyChart->addSeries(monthlyArrivalsSeries);
    monthlyChart->addSeries(monthlyDeparturesSeries);
    monthlyChart->addSeries(monthlyOverallSeries);
    auto legendMarkers = monthlyChart->legend()->markers();
    legendMarkers.at(0)->setLabel("Прилеты");
    legendMarkers.at(1)->setLabel("Вылеты");
    legendMarkers.at(2)->setLabel("Общее");

    monthlyChart->setTitle(QString("Рейсы за %1 %2 года")
                              .arg(ui->monthlyStatsMonth->currentText())
                              .arg(ui->monthlyStatsYear->currentText()));

    monthlyAxisX = new QValueAxis;
    monthlyAxisX->setTickInterval(1);
    monthlyAxisX->setTickCount(daysPerMonth.value(monthString));
    monthlyAxisX->setLabelFormat("%d");
    monthlyChart->addAxis(monthlyAxisX, Qt::AlignBottom);
    monthlyArrivalsSeries->attachAxis(monthlyAxisX);
    monthlyAxisY = new QValueAxis;
    monthlyAxisY->setMin(0);
    auto axisYRange = qRound(maxDailyFlights * 1.1);
    monthlyAxisY->setMax(axisYRange);
    if (axisYRange / 10 > 1) {
        monthlyAxisY->setTickCount(10);
    } else {
        monthlyAxisY->setTickCount(axisYRange);
    }
    monthlyAxisY->setLabelFormat("%d");
    monthlyChart->addAxis(monthlyAxisY, Qt::AlignLeft);
    monthlyArrivalsSeries->attachAxis(monthlyAxisY);
    monthlyDeparturesSeries->attachAxis(monthlyAxisY);
    monthlyOverallSeries->attachAxis(monthlyAxisY);
    monthlyChartView = new QChartView(monthlyChart);
    monthlyChartView->setParent(ui->annualStatsTab);
    monthlyChartView->show();
    ui->monthlyStatsLayout->addWidget(monthlyChartView);
    monthlyChartNeedsRefresh = true;
}

void FlightStatsWindow::ResetFlightsForm() {
    ui->annualStatsYear->setCurrentText(defaultYear);
    ui->monthlyStatsYear->setCurrentText(defaultYear);
    ui->monthlyStatsMonth->setCurrentText(defaultMonth);
    ui->statsTabs->setCurrentWidget(ui->annualStatsTab);
}

void FlightStatsWindow::RefreshFlightStats(FlightRefreshType flightRefreshType) {
    switch (flightRefreshType) {
    case FlightRefreshType::all:
        RefreshAnnualFlightStats();
        RefreshMonthlyFlightStats();
        break;
    case FlightRefreshType::annual:
        RefreshAnnualFlightStats();
        break;
    case FlightRefreshType::monthly:
        RefreshMonthlyFlightStats();
        break;
    }

}

void FlightStatsWindow::on_annualStatsYear_currentTextChanged(const QString &arg1)
{
    ui->monthlyStatsYear->setCurrentText(ui->annualStatsYear->currentText());
    RefreshAnnualFlightStats();
}


void FlightStatsWindow::on_monthlyStatsYear_currentTextChanged(const QString &arg1)
{
    ui->annualStatsYear->setCurrentText(ui->monthlyStatsYear->currentText());
    RefreshMonthlyFlightStats();
}

void FlightStatsWindow::on_monthlyStatsMonth_currentTextChanged(const QString &arg1)
{
    RefreshMonthlyFlightStats();
}




