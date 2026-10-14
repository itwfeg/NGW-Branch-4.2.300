#include "labelplacementwidget.h"

#include <QPainter>
#include <QVector>

#include <logmanager.h>
#include <backgroundwidgettypes.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelPlacementWidget::LabelPlacementWidget( QWidget *parent )
    : _Mybase( parent )
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
LabelPlacementWidget::~LabelPlacementWidget()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementWidget::generatePreview(
    int printerCount,
    int currentPrinter,
    int primaryPos,
    int secondPos,
    QSize& primarySize,
    QSize& secondSize,
    const QVector<printerProp>& printers )
{
    LOG_CLASS_INFO("LabelPlacementWidget", "generatePreview");

    QPalette p( this->palette() );

    QPixmap pixmap( this->size() );
    pixmap.fill( Qt::transparent );

    QSize labelSize( 80, 125 );
    int offset( 20 );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    QFont font( painter.font() );
    QPen pen( painter.pen() );

    //
    // render the package
    //

    pen.setColor( hobart::colorPalette::black );
    pen.setWidth( 4 );

    painter.setBrush( hobart::colorPalette::white );
    painter.setPen( pen );

    painter.drawRoundedRect( this->rect().adjusted( 4, 4, -4, -4 ), 25, 25 );

    //
    // render labels
    //

    for ( int i( 0 ); i < printerCount; ++i )
    {
        if ( printers[i].installed && ( ( i == first && primaryPos != none ) || ( i == second && secondPos != none ) ) )
        {
            if(printerCount > 1)
            {
                for(int j = 0; j < rightOfPrimary; ++j)
                {
                    if( j == 2 ) continue; // 2nd enum position is none selected, dont want to handle it here
                    if( ( currentPrinter == 0 ) && ( j > upperLeft ) ) continue;     // if primaryPrinter then only do top corners

                    if(j != primaryPos && j != secondPos)
                    {
                        drawAvailableCorners(painter, j);       // j is corner location
                    }
                }
            }

            if(i == second)
            {
                labelSize = QSize(80, 80);
            }

            QRect boundingRect( QPoint( offset, offset ), labelSize );

            // compute bounding rect
            if ( 0 == (printers[i].rotation % 180) )
            {
                if( printerCount > 1 )
                {
                    if ( i == first )
                    {
                        primaryRotation_ = 0;
                    }

                    // need to reset the width and height
                    boundingRect.setWidth( labelSize.width() );
                    boundingRect.setHeight( labelSize.height() );

                    if ( i == first )
                    {
                        primarySize.setWidth( labelSize.width() );
                        primarySize.setHeight( labelSize.height() );
                    }
                    else if ( i == second )
                    {
                        secondSize.setWidth( labelSize.width() );
                        secondSize.setHeight( labelSize.height() );
                    }
                }
            }
            else if ( 0 == (printers[i].rotation % 90) )
            {
                if( printerCount > 1 )
                {
                    if ( i == first )
                    {
                        primaryRotation_ = 90;
                    }
                    boundingRect.setWidth( labelSize.height() );
                    boundingRect.setHeight( labelSize.width() );

                    if ( i == first )
                    {
                        primarySize.setWidth( labelSize.height() );
                        primarySize.setHeight( labelSize.width() );
                    }
                    else if ( i == second )
                    {
                        secondSize.setWidth( labelSize.height() );
                        secondSize.setHeight( labelSize.width() );
                    }
                }
                else
                {
                    boundingRect.setWidth( labelSize.height() );
                    boundingRect.setHeight( labelSize.width() );
                }
            }
            else
            {
                if( printerCount > 1 )
                {
                    if ( i == first )
                    {
                        primaryRotation_ = 45;
                    }
                    qreal length( 0.7071 ); // sqrt( 0.5 )
                    length *= labelSize.height() + labelSize.width();

                    boundingRect.setWidth( length );
                    boundingRect.setHeight( length );

                    if ( i == first )
                    {
                        primarySize.setWidth( length );
                        primarySize.setHeight( length );
                    }
                    else if ( i == second )
                    {
                        secondSize.setWidth( length );
                        secondSize.setHeight( length );
                    }
                }
                else
                {
                    qreal length( 0.7071 ); // sqrt( 0.5 )
                    length *= labelSize.height() + labelSize.width();

                    boundingRect.setWidth( length );
                    boundingRect.setHeight( length );
                }
            }

            if( printerCount > 1 )
            {
                if(secondPos == rightOfPrimary || secondPos == leftOfPrimary)
                {
                    drawAvailableRelativePositions(painter, 0, primaryPos, secondPos, primarySize, secondSize, currentPrinter);
                }
                else
                {
                    for(int j = 0; j < 2; ++j)
                    {
                        drawAvailableRelativePositions(painter, j, primaryPos, secondPos, primarySize, secondSize, currentPrinter);
                    }
                }
            }

            // move label for position
            corner loc = none;
            if( i == first )
            {
                loc = ( corner )primaryPos;
            }
            else if( i == second )
            {
                loc = ( corner )secondPos;
            }

            if (( lowerRight == loc ) || ( upperRight == loc ))
                boundingRect.moveRight( this->width() - offset );

            if (( lowerLeft == loc ) || ( lowerRight == loc ))
                boundingRect.moveBottom( this->height() - offset );

            // if were not drawing for one of the four corners
            if ( loc > lowerLeft )
            {
                handleRelativePositions( boundingRect, primaryPos, secondPos, primarySize, secondSize );
            }

            // compute bounding rect center
            QPoint boundingRectCenter( boundingRect.center() );

            // prep painter
            painter.save();
            painter.translate( boundingRectCenter );

            LOG_INFO("labelStockUpsideDown[%d] = %d", i, printers[i].labelStockUpsideDown);
            LOG_INFO("printers[%d].rotation = %d", i, printers[i].rotation);
            painter.rotate( printers[i].rotation + (printers[i].labelStockUpsideDown ? 180 : 0) );

            // blue outline for currently selected printer
            if ( i == currentPrinter )
            {
                pen.setColor( hobart::colorPalette::blue );
                pen.setWidth( 2 );
            }
            else
            {
                pen.setColor( hobart::colorPalette::lightGray );
                pen.setWidth( 4 );
            }

            painter.setPen( pen );
            painter.drawRoundedRect( QRect( QPoint( -labelSize.width()/2, -labelSize.height()/2 ), labelSize ), 5, 5 );

            // printer number
            if ( i == currentPrinter )
            {
                pen.setColor( hobart::colorPalette::black );
                pen.setWidth( 2 );
            }
            else
            {
                pen.setColor( hobart::colorPalette::lightGray );
                pen.setWidth( 4 );
            }

            if(i == first)
            {
                font.setPointSize( 96 );
            }
            else if(i == second)
            {
                font.setPointSize(36);
            }

            painter.setFont( font );
            painter.setPen( pen );

            painter.drawText(
                -labelSize.width() / 2,
                -labelSize.height() / 2,
                labelSize.width(),
                labelSize.height(),
                Qt::AlignCenter,
                QString( "%1" ).arg( i+1 )
            );

            // restore painter
            painter.restore();
        }
    }


    // show pixmap
    p.setBrush( this->backgroundRole(), pixmap );
    this->setPalette( p );

    this->update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementWidget::drawAvailableCorners( QPainter& painter, int currentLoc ) const
{
    QPen pen;

    int offset = 20;
    QSize labelSize( 40, 40 );

    QRect boundingRect( QPoint( offset, offset ), labelSize );

    // move label for position
    if (( currentLoc == lowerRight ) || ( currentLoc == upperRight ))
        boundingRect.moveRight( this->width() - offset );

    if (( currentLoc == lowerLeft ) || ( currentLoc == lowerRight ))
        boundingRect.moveBottom( this->height() - offset );

    // compute bounding rect center
    QPoint boundingRectCenter( boundingRect.center() );

    // prep painter
    painter.save();
    painter.translate( boundingRectCenter );

    pen.setColor( hobart::colorPalette::green );
    pen.setWidth(4);

    painter.setBrush( hobart::colorPalette::white );
    painter.setPen( pen );

    painter.drawRoundedRect( QRect( QPoint( -labelSize.width()/2, -labelSize.height()/2 ), labelSize ), 5, 5 );

    painter.restore();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementWidget::handleRelativePositions( QRect& boundingRect, int primaryPos, int secondPos, const QSize& primarySize, const QSize& secondSize ) const
{
    int offset = 20;
    switch(primaryPos)
    {
        case upperLeft:
        {
            // should only be below or to the right of label
            if(secondPos == belowPrimary)
            {
                boundingRect.moveTop( this->y() + (primarySize.height() + offset) );

                if(primaryRotation_ == 90)
                {
                    boundingRect.moveLeft( this->x() + (primarySize.width() / 8) );
                }
                else
                {
                    boundingRect.moveLeft( this->x() );
                }
            }
            else if(secondPos == rightOfPrimary)
            {
                if(primaryRotation_ == 90)
                {
                    boundingRect.moveTop( this->y() );
                }
                else
                {
                    boundingRect.moveTop( this->y() + (primarySize.height() / 4) );
                }
                boundingRect.moveLeft( this->x() + primarySize.width() + offset );
            }

            break;
        }
        case upperRight:
        {
            if(secondPos == belowPrimary)
            {
                boundingRect.moveTop(this->y() + (primarySize.height() + offset));

                if(primaryRotation_ == 90)
                {
                    boundingRect.moveLeft(this->x() + this->width() - primarySize.width() );
                }
                else
                {
                    boundingRect.moveLeft(this->x() + this->width() - primarySize.width() - (2 * offset) );
                }
            }
            else if(secondPos == leftOfPrimary)
            {
                if(primaryRotation_ == 90)
                {
                    boundingRect.moveTop( this->y() );
                }
                else
                {
                    boundingRect.moveTop( this->y() + (primarySize.height() / 4) );
                }
                boundingRect.moveLeft( this->width() - primarySize.width() - secondSize.width() - (2 * offset) );
            }

            break;
        }
        case lowerLeft:
        {
            if(secondPos == abovePrimary)
            {
                boundingRect.moveTop( this->height() - primarySize.height() - secondSize.height() - (2 * offset) );

                if(primaryRotation_ == 90)
                {
                    boundingRect.moveLeft( this->x() + primarySize.width() / 8 );
                }
                else
                {
                    boundingRect.moveLeft( this->x() );
                }
            }
            else if(secondPos == rightOfPrimary)
            {
                if(primaryRotation_ == 90)
                {
                    boundingRect.moveTop( this->height() - (primarySize.height()) - offset );
                }
                else
                {
                    boundingRect.moveTop( this->height() - (primarySize.height() * 0.75) - offset);
                }

                boundingRect.moveLeft( this->x() + primarySize.width() + offset );
            }

            break;
        }
        case lowerRight:
        {
            if(secondPos == abovePrimary)
            {
                boundingRect.moveTop( this->height() - primarySize.height() - secondSize.height() - (2 * offset) );

                if(primaryRotation_ == 90)
                {
                    boundingRect.moveLeft(this->x() + this->width() - primarySize.width() );
                }
                else
                {
                    boundingRect.moveLeft( this->width() - primarySize.width() - (offset) );
                }
            }
            else if(secondPos == leftOfPrimary)
            {
                if(primaryRotation_ == 90)
                {
                    boundingRect.moveTop( this->height() - (primarySize.height()) - offset );
                }
                else
                {
                    boundingRect.moveTop( this->height() - (primarySize.height() * 0.75) - offset);
                }

                boundingRect.moveLeft( this->width() - primarySize.width() - secondSize.width() - (2 * offset) );
            }
            break;
        }
        default: break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LabelPlacementWidget::drawAvailableRelativePositions(
        QPainter &painter,
        int index,
        int primaryPos,
        int secondPos,
        const QSize& primarySize,
        const QSize& secondSize,
        int currentPrinter ) const
{
    QPen pen;

    QVector<qreal> dashes;
    dashes << 1 << 2;

    int offset = 20;
    //int width = 40, height = 40;
    QSize labelSize(40, 40);

    QRect boundingRect( QPoint( offset, offset ), labelSize );

    switch(primaryPos)
    {
        case upperLeft:
        {
            // first time around draw the below and above positions
            if(secondPos != belowPrimary && index == 0)
            {
                boundingRect.moveTop(this->y() + (primarySize.height() + offset));
                boundingRect.moveLeft(this->x());
            }
            else if(secondPos != rightOfPrimary)
            {
                boundingRect.moveTop( this->y() + (primarySize.height() / 3) );
                boundingRect.moveLeft( this->x() + primarySize.width() + offset );
            }
            else
            {
                return;
            }

            break;
        }
        case upperRight:
        {
            if(secondPos != belowPrimary && index == 0)
            {
                boundingRect.moveTop(this->y() + (primarySize.height() + offset));
                boundingRect.moveLeft(this->x() + this->width() - primarySize.width() /*- (edgeOffset)*/ );
            }
            else if(secondPos != leftOfPrimary)
            {
                boundingRect.moveTop( this->y() + (primarySize.height() / 3) );
                boundingRect.moveLeft( this->width() - primarySize.width() - primarySize.width() /*- (edgeOffset)*/ );
            }

            break;
        }
        case lowerLeft:
        {
            if(secondPos != abovePrimary && index == 0)
            {
                boundingRect.moveTop( this->height() - primarySize.height() - secondSize.height() /*- (edgeOffset)*/ );
                boundingRect.moveLeft( this->x() );
            }
            else if(secondPos != rightOfPrimary)
            {
                boundingRect.moveTop( this->height() - (primarySize.height() * 0.75) - offset);
                boundingRect.moveLeft( this->x() + primarySize.width() + offset );
            }

            break;
        }
        case lowerRight:
        {
            if(secondPos != abovePrimary && index == 0)
            {
                boundingRect.moveTop( this->height() - primarySize.height() - secondSize.height() /*- (edgeOffset)*/ );
                boundingRect.moveRight( this->width() /*- primaryLabelSize_.width()*/ - (offset) );
            }
            else if(secondPos != leftOfPrimary)
            {
                boundingRect.moveTop( this->height() - (primarySize.height() * 0.75) - offset);
                boundingRect.moveLeft( this->width() - primarySize.width() - secondSize.width() /*- (edgeOffset)*/ );
            }

            break;
        }
        default:
        {
            break;
        }
    }

    // compute bounding rect center
    QPoint boundingRectCenter( boundingRect.center() );

    // prep painter
    painter.save();
    painter.translate( boundingRectCenter );

    //pen.setDashPattern(dashes);
    //pen.setDashOffset(2.0);
    //pen.setColor(hobart::colorPalette::lightGray);

    // painter.drawRoundedRect( QRect( QPoint( -labelSize.width()/2, -labelSize.height()/2 ), labelSize ), 5, 5 );

    if(currentPrinter == first)
    {
        QFont font( "Arial", 30 );
        painter.setFont( font );

        pen.setColor( hobart::colorPalette::red );
        pen.setWidth( 4 );

        painter.setBrush( hobart::colorPalette::white );
        painter.setPen( pen );

        // draw a red X indicating the currently selected printer cannot move here
        //painter.drawText(
        //    QRect( QPoint( -labelSize.width()/2, -labelSize.height()/2 ), labelSize ),
        //    Qt::AlignCenter,
        //    "X"
        //);

        // confused about the red X
        painter.drawText(
            QRect( QPoint( -labelSize.width()/2, -labelSize.height()/2 ), labelSize ),
            Qt::AlignCenter,
            " "
        );
    }
    else
    {
        pen.setColor( hobart::colorPalette::green );
        pen.setWidth( 4 );

        painter.setBrush( hobart::colorPalette::white );
        painter.setPen( pen );

        painter.drawRoundedRect( QRect( QPoint( -labelSize.width()/2, -labelSize.height()/2 ), labelSize ), 5, 5 );

        /*
        painter.drawText(
            QRect( QPoint( -labelSize.width()/2, -labelSize.height()/2 ), labelSize ),
            Qt::AlignCenter,
            "O"
        );
        */
    }

    painter.restore();
}
