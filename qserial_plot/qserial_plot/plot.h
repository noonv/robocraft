#ifndef PLOT_H
#define PLOT_H

#include <qwt_interval.h>
#include <qwt_system_clock.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <qwt_curve_fitter.h>
#include <qwt_painter.h>
#include <qevent.h>

#include "definitions.h"
#include "curvedata.h"

class Plot : public QwtPlot
{
public:
    Plot( QWidget *parent = NULL);
    virtual ~Plot();

    void start();
    virtual void replot();

    void setIntervalLength(double interval);
    void setAutointerval(bool val);

public Q_SLOTS:
    void appendPoint(double u);

protected:
    virtual void resizeEvent( QResizeEvent * );
    virtual void showEvent(QShowEvent *);
    virtual void timerEvent(QTimerEvent *);

private:
    void updateGradient();
    void create_curve();

    QVector< double > data_x;
    QVector< double > data;

    void initGradient();
    void updateCurve();
    void incrementInterval();

    QwtPlotMarker *d_origin;
    QwtPlotCurve *d_curve;
    int d_paintedPoints;

    QwtPlotDirectPainter *d_directPainter;

    QwtInterval d_interval;
    int d_timerId;

    bool is_autointerval;

    QwtSystemClock d_clock;

    CurveData curvedata;
};

#endif //#ifndef PLOT_H
