#include "plot.h"

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_series_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_text.h>
#include <qwt_math.h>

Plot::Plot(QWidget *parent):
    QwtPlot( parent )
    ,d_paintedPoints(0), d_interval(0.0, 10.0), d_timerId(-1)
{

#if 0

    // panning with the left mouse button
    (void) new QwtPlotPanner( canvas() );

    // zoom in/out with the wheel
    (void) new QwtPlotMagnifier( canvas() );

    setAutoFillBackground( true );
    setPalette( QPalette( QColor( 238, 238, 238 ) ) );
    updateGradient();

    //   setTitle("Test");
    insertLegend(new QwtLegend(), QwtPlot::RightLegend);

    // axes
    setAxisTitle(xBottom, "x" );
    setAxisScale(xBottom, 0.0, 300.0);

    setAxisTitle(yLeft, "y");
    setAxisScale(yLeft, -10.0, 80.0);

    // canvas
    canvas()->setLineWidth( 1 );
    canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
    canvas()->setBorderRadius( 10 );

    QPalette canvasPalette( Qt::white );
    canvasPalette.setColor( QPalette::Foreground, QColor( 238, 238, 238 ) );
    canvas()->setPalette( canvasPalette );

    populate();

#else

    d_directPainter = new QwtPlotDirectPainter();

    setAutoReplot(false);

    // The backing store is important, when working with widget
    // overlays ( f.e rubberbands for zooming ).
    // Here we don't have them and the internal
    // backing store of QWidget is good enough.

    canvas()->setPaintAttribute(QwtPlotCanvas::BackingStore, false);

#if defined(Q_WS_X11)
    // Even if not recommended by TrollTech, Qt::WA_PaintOutsidePaintEvent
    // works on X11. This has a nice effect on the performance.

    canvas()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true);

    // Disabling the backing store of Qt improves the performance
    // for the direct painter even more, but the canvas becomes
    // a native window of the window system, receiving paint events
    // for resize and expose operations. Those might be expensive
    // when there are many points and the backing store of
    // the canvas is disabled. So in this application
    // we better don't both backing stores.

    if ( canvas()->testPaintAttribute( QwtPlotCanvas::BackingStore ) )
    {
        canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
        canvas()->setAttribute(Qt::WA_NoSystemBackground, true);
    }

#endif

    initGradient();

    plotLayout()->setAlignCanvasToScales(true);

    setAxisTitle(QwtPlot::xBottom, "Time [s]");
    setAxisScale(QwtPlot::xBottom, d_interval.minValue(), d_interval.maxValue());
    setAxisScale(QwtPlot::yLeft, 0.0, 100.0);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(QPen(Qt::gray, 0.0, Qt::DotLine));
    grid->enableX(true);
    grid->enableXMin(true);
    grid->enableY(true);
    grid->enableYMin(false);
    grid->attach(this);

    d_origin = new QwtPlotMarker();
    d_origin->setLineStyle(QwtPlotMarker::Cross);
    d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);
    d_origin->setLinePen(QPen(Qt::gray, 0.0, Qt::DashLine));
    d_origin->attach(this);

    d_curve = new QwtPlotCurve();
    d_curve->setStyle(QwtPlotCurve::Lines);
    d_curve->setPen(QPen(Qt::green, 2));
#if 1
    d_curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
#endif
#if 1
    d_curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, false);
#endif

    d_curve->setData(&curvedata);
    d_curve->attach(this);

    //create_curve();
#endif

}

Plot::~Plot()
{
    if(d_directPainter)
        delete d_directPainter;
}

void Plot::start()
{
    d_clock.start();
    d_timerId = startTimer(10);
}

void Plot::replot()
{
    //CurveData *data = static_cast<CurveData *>( d_curve->data());
    QwtPlot::replot();
    d_paintedPoints = curvedata.size();
}

void Plot::appendPoint(double u)
{
    double elapsed = d_clock.elapsed() / 1000.0;
    qDebug() << "d_clock0: " << elapsed << "val: " << u << " size="<< curvedata.size() ;
    QPointF point(elapsed, u);
    //CurveData *data = static_cast<CurveData *>( d_curve0->data() );
    curvedata.append(point);

    //$TODO add record to file here

    //qDebug() << "y="<<curvedata.boundingRect().y()<<"h= "<<curvedata.boundingRect().height();
    //setAxisScale(QwtPlot::yLeft, curvedata.boundingRect().y()-30, curvedata.boundingRect().y()+30);

    updateCurve();
}

void Plot::populate()
{
    // Insert markers

    //  ...a horizontal line at y = 0...
    QwtPlotMarker *mY = new QwtPlotMarker();
    //mY->setLabel(QString::fromLatin1("y = 0"));
    //mY->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
    mY->setLineStyle(QwtPlotMarker::HLine);
    mY->setLinePen(QPen(Qt::black, 0, Qt::DotLine));
    mY->setYValue(0.0);
    mY->attach(this);

    //  ...a vertical line at x = 2 * pi
    QwtPlotMarker *mX = new QwtPlotMarker();
    //mX->setLabel(QString::fromLatin1("x = 2 pi"));
    //mX->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
    //mX->setLabelOrientation(Qt::Vertical);
    mX->setLineStyle(QwtPlotMarker::VLine);
    mX->setLinePen(QPen(Qt::black, 0, Qt::DotLine));
    mX->setXValue(0.0);
    mX->attach(this);
}

void Plot::initGradient()
{
    QPalette pal = canvas()->palette();

#if QT_VERSION >= 0x040400
    QLinearGradient gradient( 0.0, 0.0, 1.0, 0.0 );
    gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
    gradient.setColorAt(0.0, QColor( 0, 49, 110 ) );
    gradient.setColorAt(1.0, QColor( 0, 87, 174 ) );

    pal.setBrush(QPalette::Window, QBrush(gradient));
#else
    pal.setBrush(QPalette::Window, QBrush( color ));
#endif

    canvas()->setPalette(pal);
}

void Plot::updateCurve()
{
    //qDebug()<<"[u] "<<d_clock.elapsed() / 1000.0;

    //CurveData *data = (CurveData *)d_curve->data();
    const int numPoints = curvedata.size();

    if (numPoints > d_paintedPoints) {
        const bool doClip = !canvas()->testAttribute( Qt::WA_PaintOnScreen );
        if ( doClip ) {
            /*
                   Depending on the platform setting a clip might be an important
                   performance issue. F.e. for Qt Embedded this reduces the
                   part of the backing store that has to be copied out - maybe
                   to an unaccelerated frame buffer device.
               */

            const QwtScaleMap xMap = canvasMap( d_curve->xAxis() );
            const QwtScaleMap yMap = canvasMap( d_curve->yAxis() );

            QRectF br = qwtBoundingRect( curvedata, d_paintedPoints - 1, numPoints - 1 );

            const QRect clipRect = QwtScaleMap::transform( xMap, yMap, br ).toRect();
            d_directPainter->setClipRegion(clipRect);
        }

        d_directPainter->drawSeries(d_curve, d_paintedPoints - 1, numPoints - 1);
        d_paintedPoints = numPoints;
    }
}

void Plot::incrementInterval()
{
    d_interval = QwtInterval(d_interval.maxValue(),
            d_interval.maxValue() + d_interval.width());

        curvedata.clearStaleVal(d_interval.minValue());
        //curvedata.clearStaleVal(d_interval.minValue());

        QwtScaleDiv scaleDiv = *axisScaleDiv(QwtPlot::xBottom);
        scaleDiv.setInterval(d_interval);

        for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
        {
            QList<double> ticks = scaleDiv.ticks(i);
            for ( int j = 0; j < ticks.size(); j++ )
                ticks[j] += d_interval.width();
            scaleDiv.setTicks(i, ticks);
        }
        setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);

        double y_delta = curvedata.boundingRect().width()<1?2:5*curvedata.boundingRect().width();
        setAxisScale(QwtPlot::yLeft, curvedata.boundingRect().y()-y_delta, curvedata.boundingRect().y()+y_delta);

        d_origin->setValue(d_interval.minValue() + d_interval.width() / 2.0, 0.0);

        d_paintedPoints = 0;
        replot();
}

void Plot::updateGradient()
{
    QPalette pal = palette();

    const QColor buttonColor = pal.color( QPalette::Button );

#ifdef Q_WS_X11
    // Qt 4.7.1: QGradient::StretchToDeviceMode is buggy on X11

    QLinearGradient gradient( rect().topLeft(), rect().bottomLeft() );
    gradient.setColorAt( 0.0, Qt::white );
    gradient.setColorAt( 0.7, buttonColor );
    gradient.setColorAt( 1.0, buttonColor );
#else
    QLinearGradient gradient( 0, 0, 0, 1 );
    gradient.setCoordinateMode( QGradient::StretchToDeviceMode );
    gradient.setColorAt( 0.0, Qt::white );
    gradient.setColorAt( 0.7, buttonColor );
    gradient.setColorAt( 1.0, buttonColor );
#endif

    pal.setBrush( QPalette::Window, gradient );
    setPalette( pal );
}

void Plot::create_curve()
{
    QwtPlotCurve *cMy = new QwtPlotCurve( "*C" );
    if(!cMy)
        return;

    cMy->setRenderHint(QwtPlotItem::RenderAntialiased);
    cMy->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
    //cMy->setPen(QPen(QColor(175,224,58)));
    cMy->setPen(QPen(Qt::green));
    cMy->attach(this);

    std::vector<double> xs;
    std::vector<double> ys;
    for (double x = 0; x < 300; x++)
    {
        xs.push_back(x);
        ys.push_back(20);
    }
    QwtPointArrayData * const mydata = new QwtPointArrayData(&xs[0],&ys[0],xs.size());
    cMy->setData(mydata);

    //cMy->setRawSamples(datas[i].first, datas[i].second, datas_size);
}

void Plot::resizeEvent( QResizeEvent *event )
{
    QwtPlot::resizeEvent( event );
#ifdef Q_WS_X11
    updateGradient();
#endif
}

void Plot::showEvent(QShowEvent *event)
{
    replot();
}

void Plot::timerEvent(QTimerEvent *event)
{
    if ( event->timerId() == d_timerId )
    {
        updateCurve();

        const double elapsed = d_clock.elapsed() / 1000.0;
        if ( elapsed > d_interval.maxValue() )
            incrementInterval();

        return;
    }

    QwtPlot::timerEvent(event);

}
