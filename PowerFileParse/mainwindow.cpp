#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QDebug>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QFileDialog>

#define READ_PACKET_SIZE 109

typedef union
{
    unsigned char uc[4];
    long lda;
    unsigned long ul;
    float fda;
} un_4b;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_dataSamples(0)
    , m_scrollCount(0)
{
    ui->setupUi(this);
    initSignals();
    initChart();
    initUi();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUi()
{

}


void MainWindow::initSignals()
{
    // connect(m_pReadSerialTimer, SIGNAL(timeout()), this, SLOT(readSerial()));
    connect(ui->chartView, SIGNAL(mouseMoveSignal(QPointF)), this, SLOT(onMouseMove(QPointF)));
}

void MainWindow::initChart()
{
    m_pChart = new QChart();
    m_pChart->setTheme(QChart::ChartThemeBlueCerulean);
    ui->chartView->setChart(m_pChart);
    ui->chartView->setMinimumSize(200,200);

    m_pCapacitySeries = new QLineSeries;
    m_pCapacitySeries->setName("实测电量");
    m_pChart->addSeries(m_pCapacitySeries);

    m_pCapacitySeriesDummy = new QLineSeries;
    m_pCapacitySeriesDummy->setName("理论电量");
    m_pChart->addSeries(m_pCapacitySeriesDummy);

    m_pVoltageSeries = new QLineSeries;
    m_pVoltageSeries->setName("电压");
    m_pChart->addSeries(m_pVoltageSeries);

    m_pCurrentSeries = new QLineSeries;
    m_pCurrentSeries->setName("电流");
    m_pChart->addSeries(m_pCurrentSeries);

    m_pBatteryTempratureSeries = new QLineSeries;
    m_pBatteryTempratureSeries->setName("电池温度");
    m_pChart->addSeries(m_pBatteryTempratureSeries);


    m_pCapacityAxis = new QValueAxis;
    m_pCapacityAxis->setRange(0,100);
    m_pChart->setAxisY(m_pCapacityAxis, m_pCapacitySeriesDummy);

    m_pVoltageAxis = new QValueAxis;
    m_pVoltageAxis->setRange(3000,5000);
    m_pChart->addAxis(m_pVoltageAxis, Qt::AlignRight);
    m_pVoltageSeries->attachAxis(m_pVoltageAxis);


    m_pCurrentAxis = new QValueAxis;
    m_pCurrentAxis->setRange(0,8000);
    m_pChart->addAxis(m_pCurrentAxis, Qt::AlignRight);
    m_pCurrentSeries->attachAxis(m_pCurrentAxis);

    m_pBatteryTempratureAxis = new QValueAxis;
    m_pBatteryTempratureAxis->setRange(20,50);
    m_pChart->setAxisY(m_pBatteryTempratureAxis, m_pBatteryTempratureSeries);


    m_pTimeAxis = new QDateTimeAxis;
    m_pTimeAxis->setRange(QDateTime::fromString("2021年10月19日 00:00:00", "yyyy年MM月dd日 hh:mm:ss"), QDateTime::fromString("2021年10月22日 00:00:00", "yyyy年MM月dd日 hh:mm:ss"));
    m_pTimeAxis->setFormat("yyyy年MM月dd日 hh:mm:ss");
    m_pChart->setAxisX(m_pTimeAxis, m_pCapacitySeriesDummy);
    m_pChart->setAxisY(m_pCapacityAxis, m_pCapacitySeries);
    m_pChart->setAxisX(m_pTimeAxis, m_pCapacitySeries);

    m_pChart->setAxisX(m_pTimeAxis, m_pVoltageSeries);

    m_pChart->setAxisX(m_pTimeAxis, m_pCurrentSeries);

    m_pChart->setAxisX(m_pTimeAxis, m_pBatteryTempratureSeries);

    // 图例被点击后触发
    foreach (QLegendMarker* marker, m_pChart->legend()->markers())
    {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(onLegendMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()), this, SLOT(onLegendMarkerClicked()));
    }



    // QChartView
   // ui->chartView-> setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // ui->chartView->setRubberBand( QChartView::HorizontalRubberBand );


}


void MainWindow::onLegendMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    marker->series()->setVisible(!marker->series()->isVisible());
    marker->setVisible(true);
}


void MainWindow::on_action_open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open powerrecord.txt"), "powerrecord.txt", tr("powerrecord.txt (*.txt)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug()<<"Can't open the file!"<<endl;
        }

        m_pCapacitySeries->clear();
        m_pCapacitySeriesDummy->clear();
        m_pVoltageSeries->clear();
        m_pCurrentSeries->clear();
        m_pBatteryTempratureSeries->clear();


        QByteArray firstLine;
        QByteArray lastLine;
        while(!file.atEnd()) {
            QByteArray line = file.readLine();
            if (firstLine.isEmpty())
                firstLine = line;

            lastLine = line;
            QString str(line);
            // qDebug()<< str.mid(27,2);
            QDateTime dt = QDateTime::fromString(str.mid(0,21), "yyyy年MM月dd日  hh:mm:ss");
            // qDebug()<< str.mid(27,2).toInt();
           m_pCapacitySeries->append(dt.toMSecsSinceEpoch(), str.mid(27,4).split("%")[0].toInt());

           m_pVoltageSeries->append(dt.toMSecsSinceEpoch(), str.mid(str.indexOf("电压:")+3).split(",")[0].toInt());
           m_pCurrentSeries->append(dt.toMSecsSinceEpoch(), qAbs(str.mid(str.indexOf("电流:")+3).split(",")[0].toLong()/1000));
           m_pBatteryTempratureSeries->append(dt.toMSecsSinceEpoch(), str.mid(str.indexOf("温度:")+3).toInt()/10.0);

        }

        QString str_start(firstLine);
        QDateTime dt_start = QDateTime::fromString(str_start.mid(0,21), "yyyy年MM月dd日  hh:mm:ss");
        m_pCapacitySeriesDummy->append(dt_start.toMSecsSinceEpoch(), str_start.mid(27,4).split("%")[0].toInt());


        QString str_end(lastLine);
        QDateTime dt_end = QDateTime::fromString(str_end.mid(0,21), "yyyy年MM月dd日  hh:mm:ss");
        m_pCapacitySeriesDummy->append(dt_end.toMSecsSinceEpoch(), str_end.mid(27,4).split("%")[0].toInt());

        m_pTimeAxis->setRange(dt_start, dt_end);

    }
}

void MainWindow::onMouseMove(QPointF point)
{
    ui->label->setText(QString::asprintf("y:%0.1f",point.y()));
    qDebug() << point;
}
