#include "curvedata.h"

CurveData::CurveData()
{
    d_samples.reserve(1000);
}

QPointF CurveData::sample(size_t i) const
{
    return d_samples.at(i);
}

size_t CurveData::size() const
{
    return d_samples.size();
}

QPointF CurveData::value(int i) const
{
    return d_samples.at(i);
}

void CurveData::append(const QPointF &sample)
{
    mutex.lock();

    pendingValues += sample;

    const bool isLocked = rwlock.tryLockForWrite();
    if ( isLocked )
    {
        const int numValues = pendingValues.size();
        const QPointF *pValues = pendingValues.data();

        for ( int i = 0; i < numValues; i++ ) {
            add_value(pValues[i]);
        }

        pendingValues.clear();

        rwlock.unlock();
    }

    mutex.unlock();
}

void CurveData::lock()
{
    rwlock.lockForRead();
}

void CurveData::unlock()
{
    rwlock.unlock();
}
