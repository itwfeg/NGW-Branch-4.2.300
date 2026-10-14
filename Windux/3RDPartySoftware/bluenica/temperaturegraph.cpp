/**
 * @file temperaturegraph.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "temperaturegraph.h"

#include <QDateTime>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPen>

///////////////////////////////////////////////////////////////////////////////////////////////////
temperatureGraphWidget::temperatureGraphWidget( QWidget *parent ) :
    _Mybase( parent ),
    filterBottom_( 0 ),
    filterTop_( 0 ),
    renderGraphLines_( false ),
    useCelsius_( false )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
temperatureGraphWidget::~temperatureGraphWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureGraphWidget::refresh( unsigned int filterBottom, unsigned int filterTop, const QList<tempInfo> &temps )
{
    filterBottom_ = filterBottom;
    filterTop_ = filterTop;
    temps_ = temps;

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureGraphWidget::translate()
{
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureGraphWidget::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    static const QPen blackPen( Qt::black, 0 );
    static const QPen bluePen( BLUE, 3 );
    static const QPen lightGrayPen( Qt::lightGray, 0 );
    static const QPen greenPen( GREEN, 2 );

    static const QBrush redBrush( QColor( RED.red(), RED.green(), RED.blue(), 63 ) );
    static const QBrush transparentBrush( Qt::transparent );

    QPainter p;
    p.begin( this );
    p.setBrush( transparentBrush );
    p.setFont( font() );
    p.setPen( blackPen );

    // background
    p.fillRect( rect(), Qt::white );

    // render axis
    if ( filterBottom_ < filterTop_ )
    {
        const int timezone( utcOffset() );

        const QFontMetrics fm( font() );
        const QSize textSize( fm.width( "XXX.X" ), fm.lineSpacing() );
        const QSize timeSize( fm.width( "XX:XX pm" ), fm.lineSpacing() );
        const QSize daySize( fm.width( "XXX" ), fm.lineSpacing() );

        const QRect graphRect( textSize.width(), textSize.height(), width() - (textSize.width() * 15) / 10, height() - textSize.height() * 2 );

        // compute temperature bounds
        int tempBottom;
        int tempTop;

        computeTempBounding( tempBottom, tempTop );

        // compute axis deltas
        const int deltaX( filterTop_ - filterBottom_ );
        const int deltaY( tempTop - tempBottom );

        // draw profile
        if ( isTempProfileValid() )
        {
            int coord;
            int top;
            int bottom;

            // draw standard temp
            coord = graphRect.bottom() - (((toInt( convertTemp( tempProfile_.tempStandard ) ) - tempBottom) * graphRect.height()) / deltaY);

            p.setPen( greenPen );
            p.drawLine( graphRect.left(), coord, graphRect.right(), coord );

            // draw upper warning
            top = graphRect.bottom() - (((toInt( convertTemp( tempProfile_.tempWarningTo ) ) - tempBottom) * graphRect.height()) / deltaY);
            bottom = graphRect.bottom() - (((toInt( convertTemp( tempProfile_.tempNormalTo ) ) - tempBottom) * graphRect.height()) / deltaY);

            if ( graphRect.top() < bottom )
            {
                top = qMax( graphRect.top(), top );

                if ( top < bottom )
                    p.fillRect( graphRect.left(), top, graphRect.width(), bottom - top, redBrush );
            }

            // draw lower warning
            top = graphRect.bottom() - (((toInt( convertTemp( tempProfile_.tempNormalFrom ) ) - tempBottom) * graphRect.height()) / deltaY);
            bottom = graphRect.bottom() - (((toInt( convertTemp( tempProfile_.tempWarningFrom ) ) - tempBottom) * graphRect.height()) / deltaY);

            if ( top < graphRect.bottom() )
            {
                bottom = qMin( graphRect.bottom(), bottom );

                if ( top < bottom )
                    p.fillRect( graphRect.left(), top, graphRect.width(), bottom - top, redBrush );
            }
        }

        // draw axis
        p.setPen( blackPen );
        p.drawRect( graphRect );

        // compute y axis steppings
        const int numLabelsY( graphRect.height() / textSize.height() );

        int stepY( 10 );

        if ( (deltaY / 10) < numLabelsY ) // 0.1
            ;
        else if ( (deltaY / 20) < numLabelsY ) // 0.2
            stepY = 20;
        else if ( (deltaY / 50) < numLabelsY ) // 0.5
            stepY = 50;
        else if ( (deltaY / 100) < numLabelsY ) // 1.0
            stepY = 100;
        else if ( (deltaY / 200) < numLabelsY ) // 2.0
            stepY = 200;
        else if ( (deltaY / 500) < numLabelsY ) // 5.0
            stepY = 500;
        else if ( (deltaY / 1000) < numLabelsY ) // 10.0
            stepY = 1000;
        else if ( (deltaY / 2500) < numLabelsY ) // 25.0
            stepY = 2500;
        else if ( (deltaY / 5000) < numLabelsY ) // 50.0
            stepY = 5000;
        else if ( (deltaY / 10000) < numLabelsY ) // 100.0
            stepY = 10000;

        // label the y axis
        p.drawText( 0, 0, textSize.width(), textSize.height(), 0, tempUnit() );

        int y( tempBottom );

        y += stepY - (y % stepY);

        while ( tempBottom < y )
            y -= stepY;

        while ( y <= tempBottom )
            y += stepY;

        do
        {
            int coord( graphRect.bottom() - (((y - tempBottom) * graphRect.height()) / deltaY) );

            // render graph line
            if ( renderGraphLines_ )
            {
                p.setPen( lightGrayPen );
                p.drawLine( graphRect.left(), coord, graphRect.right(), coord );
            }

            // render tick
            p.setPen( blackPen );
            p.drawLine( graphRect.left(), coord, graphRect.left() + 4, coord );

            // render temp
            p.drawText(
                0,
                coord - (textSize.height() / 2),
                textSize.width(),
                textSize.height(),
                Qt::AlignRight,
                QString( "%0.%1" ).arg( y / 100 ).arg( (y % 100) / 10 ) );

        } while ( (y += stepY) < tempTop );


        // compute x axis steppings
        const int numLabelsX( graphRect.width() / timeSize.width() );

        int stepX( 3600 );

        if ( (deltaX / 3600) < numLabelsX ) // 1h
            ;
        else if ( (deltaX / 7200) < numLabelsX ) // 2h
            stepX = 7200;
        else if ( (deltaX / 21600) < numLabelsX ) // 6h
            stepX = 21600;
        else if ( (deltaX / 43200) < numLabelsX ) // 12h
            stepX = 43200;
        else if ( (deltaX / 86400) < numLabelsX ) // 1d
            stepX = 86400;
        else if ( (deltaX / 172800) < numLabelsX ) // 2d
            stepX = 172800;
        else if ( (deltaX / 604800) < numLabelsX ) // 1w
            stepX = 604800;

        // label the x axis
        int x( filterBottom_ );

        x += stepX - (x % stepX);
        x -= timezone;

        while ( (int) filterBottom_ < x )
            x -= stepX;

        while ( x <= (int) filterBottom_ )
            x += stepX;

        do
        {
            QDateTime stamp( QDateTime::fromMSecsSinceEpoch( (qint64) x * 1000 ) );

            int coord( graphRect.left() + (((x - filterBottom_) * graphRect.width()) / deltaX) );

            // render graph line
            if ( renderGraphLines_ )
            {
                p.setPen( lightGrayPen );
                p.drawLine( coord, graphRect.top(), coord, graphRect.bottom() );
            }

            // render tick
            p.setPen( blackPen );
            p.drawLine( coord, graphRect.bottom() - 4, coord, graphRect.bottom() );

            // render time
            p.drawText(
                coord - (timeSize.width() / 2),
                graphRect.bottom(),
                timeSize.width(),
                timeSize.height(),
                Qt::AlignCenter,
                stamp.toString( "hh:mm a") );

        } while ( (x += stepX) < (int) filterTop_ );

        // label day of week
        // only do this when more than one day of data
        if ( 86400 <= deltaX )
        {
            stepX = 43200; // 12h

            x = filterBottom_;
            x += stepX - (x % stepX);
            x -= timezone;

            while ( (int) filterBottom_ < x )
                x -= stepX;

            while ( x <= (int) filterBottom_ )
                x += stepX;

            do
            {
                QDateTime stamp( QDateTime::fromMSecsSinceEpoch( (qint64) x * 1000 ) );

                int coord( graphRect.left() + (((x - filterBottom_) * graphRect.width()) / deltaX) );

                // render tick (midnight)
                if ( 0 == ((x + timezone) % 86400) )
                {
                    p.setPen( blackPen );
                    p.drawLine( coord, 4, coord, daySize.height() - 4 );
                }

                // render day of week (noon)
                else
                {
                    p.drawText(
                        coord - (daySize.width() / 2),
                        0,
                        daySize.width(),
                        daySize.height(),
                        Qt::AlignCenter,
                        stamp.toString( "ddd") );
                }

            } while ( (x += stepX) < (int) filterTop_ );
        }

        // render data
        if ( temps_.size() )
        {
            bool first( true );
            QPoint prevPt;

            p.setPen( bluePen );
            p.setRenderHint( QPainter::Antialiasing );

            foreach ( const tempInfo& ti, temps_ )
            {
                QPoint pt(
                    graphRect.left() + (((ti.stamp - filterBottom_) * graphRect.width()) / deltaX),
                    graphRect.bottom() - (((toInt( convertTemp( ti.temperature ) ) - tempBottom) * graphRect.height()) / deltaY) );

                if ( !first )
                    p.drawLine( prevPt, pt );
                else
                {
                    first = false;
                }

                prevPt = pt;
            }
        }
    }

    p.end();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
qreal temperatureGraphWidget::convertTemp( qreal temp ) const
{
    if ( useCelsius_ )
        return temp;

    temp *= 9.0;
    temp /= 5.0;
    temp += 32.0;

    return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString temperatureGraphWidget::tempUnit() const
{
    static const QChar degree( 0260 ); // degree character in octal

    return QString( "%0%1").arg( degree ).arg( useCelsius_ ? "C" : "F" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void temperatureGraphWidget::computeTempBounding( int &bottom, int &top ) const
{
    static const qreal minDiff( 0.2 );

    // compute temperature bounds
    qreal tempBottom( convertTemp( 0.0 ) );
    qreal tempTop( convertTemp( 40.0 ) );

    if ( temps_.size() )
    {
        bool first( true );

        foreach ( const tempInfo& ti, temps_ )
        {
            const qreal converted( convertTemp( ti.temperature ) );

            if ( first )
            {
                tempBottom = tempTop = converted;
                first = false;
            }
            else if ( converted < tempBottom )
                tempBottom = converted;
            else if ( tempTop < converted )
                tempTop = converted;
        }

        // adjust to profile bounds
        if ( isTempProfileValid() )
        {
            const qreal tempWarningTo( convertTemp( tempProfile_.tempWarningTo ) );
            const qreal tempStandard( convertTemp( tempProfile_.tempStandard ) );
            const qreal tempWarningFrom( convertTemp( tempProfile_.tempWarningFrom ) );

            // above standard
            if ( tempStandard <= tempBottom )
            {
                tempTop = qMax( tempWarningTo, tempTop );
                tempBottom = tempStandard;
            }

            // below standard
            else if ( tempTop <= tempStandard )
            {
                tempTop = tempStandard;
                tempBottom = qMin( tempWarningFrom, tempBottom );
            }

            // above and below standard
            else
            {
                tempTop = qMax( tempWarningTo, tempTop );
                tempBottom = qMin( tempWarningFrom, tempBottom );
            }
        }

        // modify bounds to allow some white space at top and bottom
        qreal diff( tempTop - tempBottom );
        diff *= 0.1;

        diff = qMax( diff, minDiff );

        tempBottom -= diff;
        tempTop += diff;
    }

    bottom = toInt( tempBottom );
    top = toInt( tempTop );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool temperatureGraphWidget::isTempProfileValid() const
{
    return (( tempProfile_.tempWarningFrom < tempProfile_.tempNormalFrom ) &&
        ( tempProfile_.tempNormalFrom < tempProfile_.tempStandard ) &&
        ( tempProfile_.tempStandard < tempProfile_.tempNormalTo ) &&
        ( tempProfile_.tempNormalTo < tempProfile_.tempWarningTo ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int temperatureGraphWidget::toInt( qreal temp )
{
    return (int)((temp * 100.0) + 0.5);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int temperatureGraphWidget::utcOffset()
{
    const QDateTime dateTime1( QDateTime::currentDateTime() );
    const QDateTime dateTime2( QDateTime( dateTime1.date(), dateTime1.time(), Qt::UTC ) );

    return dateTime1.secsTo( dateTime2 );
}
