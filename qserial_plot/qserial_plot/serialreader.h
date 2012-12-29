#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QThread>

#include "definitions.h"
#include "serial.h"

class SerialReader : public QThread
{
    Q_OBJECT
public:
    explicit SerialReader(QObject *parent = 0);
    ~SerialReader();
    int init();
    void run();
    void stop();

    int open(const QString &port_name, int port_rate);
    int close();
    int is_serial_opened();

    int parse_buf(char* buf, int &buf_size);
    void move_data_in_buffer(char* buf, int beg, int& end);
    void append_data(double elapsed);
    
signals:
    void OurSignal(double i);

private:
    volatile bool m_stop;

    double value;

    Serial d_serial;

    QString serial_name;
    int serial_rate;
};

#endif // SERIALREADER_H
