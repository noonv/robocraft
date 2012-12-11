#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plot.h"
#include "serialreader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void OnGetSample(double val);

signals:


private:
    Ui::MainWindow *ui;

    Plot *d_plot;
    SerialReader *reader;
};

#endif // MAINWINDOW_H
