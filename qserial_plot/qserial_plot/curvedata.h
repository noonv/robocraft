#include <qwt_series_data.h>
#include <QMutex>
#include <QReadWriteLock>

class CurveData: public QwtArraySeriesData<QPointF>
{
public:
    CurveData();

    virtual QPointF sample(size_t i) const;
    virtual size_t size() const;

    virtual QRectF boundingRect() const
    {
        if ( d_boundingRect.width() < 0.0 )
            d_boundingRect = qwtBoundingRect( *this );

        return d_boundingRect;
    }

    QPointF value(int index) const;

    void append(const QPointF &sample);

    inline void add_value(const QPointF &sample) {
        d_samples += sample;

        // adjust the bounding rectangle

        if ( d_boundingRect.width() < 0 || d_boundingRect.height() < 0 )
        {
            d_boundingRect.setRect(sample.x(), sample.y(), 0.0, 0.0);
        }
        else
        {
            d_boundingRect.setRight(sample.x());

            if ( sample.y() > d_boundingRect.bottom() )
                d_boundingRect.setBottom(sample.y());

            if ( sample.y() < d_boundingRect.top() )
                d_boundingRect.setTop(sample.y());
        }
    }

    void clear()
    {
        d_samples.clear();
        d_samples.squeeze();
        d_boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    }

    void clearStaleVal(double limit)
    {
        const QVector<QPointF> values = d_samples;
        d_samples.clear();

        int index;
        for ( index = values.size() - 1; index >= 0; index-- )
        {
            if ( values[index].x() < limit )
                break;
        }

        if ( index > 0 )
            d_samples += (values[index++]);

        while (index < values.size() - 1)
            d_samples += (values[index++]);

        d_boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    }

    void lock();
    void unlock();

protected:
    QReadWriteLock rwlock;

    QMutex mutex;
    QVector<QPointF> pendingValues;
};
