#include "serialreader.h"
#include <stdio.h>

SerialReader::SerialReader(QObject *parent) :
    QThread(parent)
{
    m_stop = false;
}

SerialReader::~SerialReader()
{
    d_serial.close();
}

int SerialReader::init()
{
    serial_name = QString(SERIAL_PORT_NAME);
    serial_rate = SERIAL_RATE;

    return open(serial_name, serial_rate);
}

void SerialReader::run()
{
    int res;
    char buf[2048];
    int buf_size = 0;

    m_stop = false;

    while (!m_stop) {
        //qDebug(".");
        if(d_serial.waitInput(200)) {
            if( (res = d_serial.available()) > 0 ) {
                if(res+buf_size < sizeof(buf)) {
                    res = d_serial.read(buf+buf_size, res);
                    buf_size += res;
                    do {
                        res = parse_buf(buf, buf_size);
                    }
                    while( !res );
                    //qDebug("buf= %s buf_size=%d", buf, buf_size);
                }
                else {
                    printf("[!] Too mutch data: size=%d res=%d\n", buf_size, res);
                    buf_size = 0;
                }
            }
        }
    }
    d_serial.close();
}

void SerialReader::stop()
{
    m_stop = true;
}

int SerialReader::open(const QString &port_name, int port_rate)
{
    if(port_name.length() <= 0 || port_rate <= 0)
        return -1;

    char s_name[256] = {0};
    strncpy(s_name, port_name.toLocal8Bit().data(), sizeof(s_name));

    int res = d_serial.open(s_name, port_rate);
    qDebug("[i] open serial %s %d res=%d", s_name, port_rate, res);
    if(res)
        fprintf(stderr, "[!] Cant open port: %s\n", s_name);
    else {
        serial_name = port_name;
        serial_rate = port_rate;
    }

    return res;
}

int SerialReader::close()
{
    return d_serial.close();
}

int SerialReader::is_serial_opened()
{
    return d_serial.connected();
}

int SerialReader::parse_buf(char *buf, int &buf_size)
{
    if(!buf || buf_size <= 0)
        return -1;

    //qDebug("%s", buf);

    char str[1024];
    memset(str, 0, sizeof(str));
    bool is_str = false;

    // find string
    int i=0;
    int beg = 0;
    for(i=0; i<buf_size; i++) {
        if(i<3 && buf[i]=='\r' || buf[i]=='\n') {
            beg++;
            continue;
        }
        if(buf[i]=='\r' || buf[i]=='\n') {
            memcpy(str, buf+beg, i);
            str[i]=0;
            is_str = true;
            break;
        }
    }

    if(is_str) {
        move_data_in_buffer(buf, i, buf_size);
        double val;
        char *tmp_ptr = 0;
        val = strtod(str, &tmp_ptr);
        //qDebug("str=%s (%d)", str, buf_size);
        append_data( val/1000. );
        return 0;
    }

    return 1;
}

void SerialReader::move_data_in_buffer(char* buf, int beg, int& end)
{
    if(!buf || beg <=0 || end <= 0)
        return;

    int i, j;
    for(j=0, i=beg; i<end; i++, j++) {
        buf[j] = buf[i];
    }
    end = j;
}

void SerialReader::append_data(double val)
{
    //qDebug("%f", val);
    emit OurSignal(val);
}
