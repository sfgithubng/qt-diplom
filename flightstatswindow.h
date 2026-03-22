#ifndef FLIGHTSTATSWINDOW_H
#define FLIGHTSTATSWINDOW_H

#pragma once
#include "dbhandler.h"
#include <QtCharts/QtCharts>
#include <QCloseEvent>
#include <QWidget>

enum class FlightRefreshType {
    all,
    annual,
    monthly
};

namespace Ui {
class FlightStatsWindow;
}

class FlightStatsWindow : public QWidget
{
    Q_OBJECT
private:
    Ui::FlightStatsWindow *ui;
    DBHandler* dbHandler;
    void closeEvent(QCloseEvent *event);
    QChart* annualChart;
    QChartView* annualChartView;
    QBarSeries* annualSeries;
    QBarCategoryAxis* annualAxisX;
    QValueAxis* annualAxisY;
    QBarSet* annualArrivalsSet;
    QBarSet* annualDeparturesSet;
    QBarSet* annualOverallSet;
    QChart* monthlyChart;
    QChartView* monthlyChartView;
    QLineSeries* monthlyArrivalsSeries;
    QLineSeries* monthlyDeparturesSeries;
    QLineSeries* monthlyOverallSeries;
    QValueAxis* monthlyAxisX;
    QValueAxis* monthlyAxisY;
    bool annualChartNeedsRefresh;
    bool monthlyChartNeedsRefresh;
    QMap<QString, uint32_t> daysPerMonth;
    QStringList months;
    QStringList years;
    QString defaultYear;
    QString defaultMonth;
public:
    explicit FlightStatsWindow(DBHandler* constrDbHandler,
                               QWidget *parent = nullptr);
    ~FlightStatsWindow();

public slots:
    void SetAirportName(QString airportName);
    void RefreshAnnualFlightStats();
    void RefreshMonthlyFlightStats();
    void ResetFlightsForm();
    void RefreshFlightStats(FlightRefreshType flightRefreshType);
signals:
    void FormClosed(bool closedStatus);
private slots:
    void on_annualStatsYear_currentTextChanged(const QString &arg1);
    void on_monthlyStatsYear_currentTextChanged(const QString &arg1);
    void on_monthlyStatsMonth_currentTextChanged(const QString &arg1);
    void on_closeWindowButton_clicked();
};

#endif // FLIGHTSTATSWINDOW_H
