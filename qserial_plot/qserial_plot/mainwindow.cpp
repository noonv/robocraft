#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qlayout.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#if 1
    reader = 0;

    reader = new SerialReader(this);
    if(!reader)
        return;

    connect(reader,SIGNAL(OurSignal(double)),this,SLOT(OnGetSample(double)));

    if(!reader->init())
        reader->start();
#endif

#if 1
    d_plot = new Plot(this);
    if(!d_plot)
        return;

    //const double intervalLength = 10.0; // seconds
    //d_plot->setIntervalLength(intervalLength);

    QHBoxLayout *layout = new QHBoxLayout(ui->centralWidget);
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->addWidget(d_plot);

    this->resize(800, 400);

    d_plot->start();
#endif
}

MainWindow::~MainWindow()
{
#if 1
    if(reader) {
        reader->stop();
        reader->wait(500);
        delete reader;
    }
#endif
    delete ui;
}

void MainWindow::OnGetSample(double val)
{
    //qDebug("%f", val);
    if(d_plot)
        d_plot->appendPoint(val);
}
