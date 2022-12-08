#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <QChart>
#include <QChartView>
#include <QtCharts>
#include <QDateTimeAxis>

QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initUi();
    void initSignals();
    void initChart();

private slots:
    void on_action_open_triggered();
    void onMouseMove(QPointF point);
    void onLegendMarkerClicked();

private:
    Ui::MainWindow *ui;
    QChart *m_pChart;
    QLineSeries *m_pCapacitySeries;
    QLineSeries *m_pCapacitySeriesDummy;
    QLineSeries *m_pVoltageSeries;
    QLineSeries *m_pCurrentSeries;
    QLineSeries *m_pBatteryTempratureSeries;
    QDateTimeAxis *m_pTimeAxis;
    QValueAxis *m_pCapacityAxis;
    QValueAxis *m_pVoltageAxis;
    QValueAxis *m_pCurrentAxis;
    QValueAxis *m_pBatteryTempratureAxis;
    qreal m_dataSamples;
    int m_scrollCount;
};
#endif // MAINWINDOW_H
