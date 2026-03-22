| Идентификатор требования | Результат проверки |Наименование функций, реализующих это требование|
|-----|--------|---------------------------|
| Функциональные требования |
| ФТ1 | Требование выполнено | DBHandler::ConnectToDB() |
| ФТ2 | Требование выполнено | DBHandler::DBHandler(QObject*) |
| ФТ3 | Требование выполнено | MainWindow::DBConnectUIUpdate(bool) |
| ФТ4 | Требование выполнено | MainWindow::DBConnectUIUpdate(bool) |
| ФТ5 | Требование выполнено | DBHandler::ConnectToDB() |
| ФТ6 | Требование выполнено | QTimer::timeout(), DBHandler::ConnectToDB() |
| ФТ7 | Требование выполнено | MainWindow::PopulateAirports() |
| ФТ8 | Требование выполнено | MainWindow::PopulateAirports() |
| ФТ9 | Требование выполнено | DBHandler::FetchAirportNames() |
| ФТ10 | Требование выполнено | MainWindow::on_getFlightsButton_clicked() |
| ФТ11 | Требование выполнено | MainWindow::on_getFlightsButton_clicked() |
| ФТ12 | Требование выполнено | MainWindow::on_getFlightsButton_clicked() |
| ФТ13 | Требование выполнено | MainWindow::FlightStatsWindowShow() |
| ФТ14 | Требование выполнено | MainWindow::FlightStatsWindowShow() |
| ФТ15 | Требование выполнено | MainWindow::SetMainWindowEnabled(bool) |
| ФТ16 | Требование выполнено | FlightStatsWindow::closeEvent(QCloseEvent*), FlightStatsWindow::on_closeWindowButton_clicked() |
| ФТ17 | Требование выполнено | FlightStatsWindow::RefreshAnnualFlightStats(), FlightStatsWindow::RefreshMonthlyFlightStats() |
| ФТ18 | Требование выполнено | FlightStatsWindow::RefreshAnnualFlightStats() |
| ФТ19 | Требование выполнено | FlightStatsWindow::FlightStatsWindow(DBHandler*, QWidget*) |
| ФТ20 | Требование выполнено | FlightStatsWindow::RefreshMonthlyFlightStats() |
| ФТ21 | Требование выполнено | FlightStatsWindow::ResetFlightsForm(), FlightStatsWindow::RefreshMonthlyFlightStats() |
| ФТ22 | Требование выполнено | FlightStatsWindow::on_monthlyStatsYear_currentTextChanged(const QString) |
| Пользовательские требования |
| ПТ1 | Требование выполнено | MainWindow::on_directionSelector_currentIndexChanged(int) |
| ПТ2 | Требование выполнено | MainWindow::on_dateSelector_selectionChanged() |
| ПТ3 | Требование выполнено |  |
| Ограничения |
| О1 | Требование выполнено | MainWindow::on_directionSelector_currentIndexChanged(int) |
| О2 | Требование выполнено | MainWindow::MainWindow(QWidget*) |
| Требования к внешним интерфейсам |
| ВИ1 | Требование выполнено | DBHandler::DBHandler(QObject*) |
| Требования к пользовательскому интерфейсу |
| ПИ1 | Требование выполнено | MainWindow::MainWindow(QWidget*) |
| ПИ2 | Требование выполнено | FlightStatsWindow::FlightStatsWindow(DBHandler*, QWidget*) |
| ПИ3 | Требование выполнено | MainWindow::MainWindow(QWidget*), FlightStatsWindow::FlightStatsWindow(DBHandler*, QWidget*) |
| ПИ4 | Требование выполнено | MainWindow::MainWindow(QWidget*), FlightStatsWindow::FlightStatsWindow(DBHandler*, QWidget*) |
| Требования к производительности |
| ТП1 | Требование выполнено | MainWindow::MainWindow(QWidget*), FlightStatsWindow::FlightStatsWindow(DBHandler*, QWidget*), DBHandler::DBHandler(QObject*) |
| Требования к безопасности |
| ТБ2 | Требование выполнено | DBHandler::DBHandler(QObject*) |
| Требования к атрибутам качества |
| АК1 | Требование выполнено | QWidget::QWidget(QWidget*, Qt::WindowFlags) |
