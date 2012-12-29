#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    reader = 0;

    reader = new SerialReader(this);
    if(!reader)
        return;

    connect(reader,SIGNAL(OurSignal(double)),this,SLOT(OnGetSample(double)));

    if(!reader->init())
        reader->start();

    d_plot = new Plot(this);
    if(!d_plot)
        return;

    const double intervalLength = 10.0; // seconds
    d_plot->setIntervalLength(intervalLength);

    QHBoxLayout *layout = new QHBoxLayout(ui->centralWidget);
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget(d_plot);

    QWidget *page = new QWidget(this);
    if(!page)
        return;

    page->setMaximumWidth(200);

    d_port_name = new QLineEdit(page);
    d_port_name->setText(QString(SERIAL_PORT_NAME));
    d_port_rate = new QLineEdit(page);
    d_port_rate->setText(QString::number(SERIAL_RATE));
    d_interval_len = new QLineEdit(page);
    d_interval_len->setText(QString::number(intervalLength));
    d_chbx_autointerval = new QCheckBox("auto Y", page);
    btn_serial_connect = new QPushButton("Connect", page);
    btn_change_settings = new QPushButton("Change settings", page);
    d_lbl_current_value = new QLabel("", page);

    int row = 0;

    QGridLayout *vlayout = new QGridLayout(page);
    if(!vlayout)
        return;

    vlayout->addWidget(new QLabel("port name: ", page), row, 0 );
    vlayout->addWidget(d_port_name, row++, 1);

    vlayout->addWidget(new QLabel("port rate: ", page), row, 0 );
    vlayout->addWidget(d_port_rate, row++, 1);

    vlayout->addLayout(new QHBoxLayout(), row++, 0);
    vlayout->addWidget(btn_serial_connect, row++, 1);

    vlayout->addWidget(new QLabel("interval", page), row, 0 );
    vlayout->addWidget(d_interval_len, row++, 1);

    vlayout->addWidget(new QLabel("", page), row, 0 );
    vlayout->addWidget(d_chbx_autointerval, row++, 1);

    vlayout->addLayout(new QHBoxLayout(), row++, 0);

    vlayout->addWidget(btn_change_settings, row++, 1);

    vlayout->addWidget(new QLabel("value: ", page), row, 0 );
    vlayout->addWidget(d_lbl_current_value, row++, 1);

    vlayout->addLayout(new QHBoxLayout(), row++, 0);

    vlayout->setColumnStretch(1, 10);
    vlayout->setRowStretch(row, 10);

    layout->addWidget(page);

    connect( btn_serial_connect, SIGNAL(clicked()),this, SLOT(OnClickedSerial()) );
    connect( btn_change_settings, SIGNAL(clicked()),this, SLOT(OnClickedSettings()) );

    this->resize(800, 400);

    if(reader->is_serial_opened())
        btn_serial_connect->setText(QString("Disconnect"));

    d_plot->start();
}

MainWindow::~MainWindow()
{
    if(reader) {
        reader->stop();
        reader->wait(500);
        delete reader;
    }
    delete ui;
}

void MainWindow::OnGetSample(double val)
{
    //qDebug("%f", val);
    d_lbl_current_value->setText(QString().number(val));
    if(d_plot)
        d_plot->appendPoint(val);
}

void MainWindow::OnClickedSerial()
{
    if(!reader)
        return;

    if(!reader->is_serial_opened()) {
        if(!reader->open(d_port_name->text(), d_port_rate->text().toInt())) {
            reader->start();
            btn_serial_connect->setText(QString("Disconnect"));
        }
    }
    else {
        reader->stop();
        reader->terminate();
        reader->wait(500);
        reader->close();
        btn_serial_connect->setText(QString("Connect"));
    }
}

void MainWindow::OnClickedSettings()
{
    if(!d_interval_len || !d_chbx_autointerval || !d_plot)
        return;

    bool val = (d_chbx_autointerval->checkState() == Qt::Checked)?true:false;
    double intervalLength = d_interval_len->text().toDouble();
    if(intervalLength < 1)
        intervalLength = 1.0;
    if(intervalLength > PLOT_LEN_Y)
        intervalLength = PLOT_LEN_Y;
    d_interval_len->setText(QString().number(intervalLength));

    d_plot->setAutointerval(val);
    d_plot->setIntervalLength(intervalLength);
}
