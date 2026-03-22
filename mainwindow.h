#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include "dbhandler.h"
#include "flightstatswindow.h"
#include <QAction>
#include <QDebug>
#include <QMainWindow>
#include <QMap>
#include <QMenuBar>
#include <QStatusBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    DBHandler* dbHandler;
    void PopulateAirports();
    bool flightSelectionInitialized;
    FlightStatsWindow* flightStatsWindow;
    QMenuBar* mainWindowMenuBar;
    QAction* flightStatsAction;
    QStatusBar* mainWindowStatusBar;
private slots:
    void DBConnectUIUpdate(bool connectionStatus);
    void SetMainWindowEnabled(bool enabledStatus);
    void on_directionSelector_currentIndexChanged(int index);
    void on_airportSelector_currentIndexChanged(int index);
    void on_dateSelector_selectionChanged();
    void on_getFlightsButton_clicked();
    void FlightStatsWindowShow();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void SetFlightStatsAirportName(QString airportName);
    void NotifyToResetFlightsForm();
    void NotifyToRefreshFlightStats(FlightRefreshType flightRefreshType);

};
#endif // MAINWINDOW_H
