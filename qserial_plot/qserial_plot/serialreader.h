#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QThread>

#include "serial.h"

#define SERIAL_PORT_NAME "COM10"
#define SERIAL_RATE 115200

class SerialReader : public QThread
{
    Q_OBJECT
public:
    explicit SerialReader(QObject *parent = 0);
    ~SerialReader();
    int init();
    void run();
    void stop();

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
