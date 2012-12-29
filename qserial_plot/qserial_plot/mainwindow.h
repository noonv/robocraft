#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "definitions.h"
#include "plot.h"
#include "serialreader.h"

namespace Ui {
class MainWindow;
}

class QLabel;
class QLineEdit;
class QCheckBox;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void OnGetSample(double val);
    void OnClickedSerial();
    void OnClickedSettings();

signals:


private:
    Ui::MainWindow *ui;
    QLineEdit *d_port_name;
    QLineEdit *d_port_rate;
    QLineEdit *d_interval_len;
    QCheckBox *d_chbx_autointerval;
    QPushButton *btn_serial_connect;
    QPushButton *btn_change_settings;
    QLabel *d_lbl_current_value;

    Plot *d_plot;
    SerialReader *reader;
};

#endif // MAINWINDOW_H
