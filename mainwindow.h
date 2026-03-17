#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once
#include "dbhandler.h"
#include <QDebug>
#include <QMainWindow>
#include <QMap>

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
private slots:
    void DBConnectUIUpdate(bool connectionStatus);
    void on_directionSelector_currentIndexChanged(int index);
    void on_airportSelector_currentIndexChanged(int index);
    void on_dateSelector_selectionChanged();
    void on_getFlightsButton_clicked();

    void on_flightStatsButton_clicked();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


};
#endif // MAINWINDOW_H
