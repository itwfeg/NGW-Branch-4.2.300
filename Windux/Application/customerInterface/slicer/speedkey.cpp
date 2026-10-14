#include "speedkey.h"

#include <QFile>
#include <QMouseEvent>
#include <QPixmap>
#include <QDebug>

///////////////////////////////////////////////////////////////////////////////////////////////////
speedKey::speedKey(QWidget *parent) :
    _Mybase(parent)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
speedKey::~speedKey()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void speedKey::create(const QSize &menuSize, const QStringList &value)
{
    x_ = ( 0 < value.size() ) ? value.at(0).toInt() : 0;
    y_ = ( 1 < value.size() ) ? value.at(1).toInt() : 0;
    w_ = ( 2 < value.size() ) ? value.at(2).toInt() : 1;
    h_ = ( 3 < value.size() ) ? value.at(3).toInt() : 1;

    type_ = ( 4 < value.size() ) ? value.at(4).toInt() : 0;
    val1_ = ( 5 < value.size() ) ? value.at(5) : QString();
    val2_ = ( 6 < value.size() ) ? value.at(6) : QString();
    val3_ = ( 16 < value.size() ) ? value.at(16) : QString();

    text_ = ( 7 < value.size() ) ? value.at(7) : QString();
    textHAlign_ = ( 8 < value.size() ) ? value.at(8).toInt() : 0;
    textVAlign_ = ( 9 < value.size() ) ? value.at(9).toInt() : 0;
    textColor_ = QColor( ( 10 < value.size() ) ? value.at(10) : "#FFFFFF" );
    textFontFamily_ = ( 11 < value.size() ) ? value.at(11) : QString();
    textFontPointSize_ = ( 12 < value.size() ) ? value.at(12).toInt() : 12;

    backgroundColor_ = QColor( ( 13 < value.size() ) ? value.at(13) : "#000000" );
    backgroundImage_ = ( 14 < value.size() ) ? value.at(14) : QString();

    foregroundImage_ = ( 15 < value.size() ) ? value.at(15) : QString();

    totalWeight_ = ( 17 < value.size() ) ? value.at(17).toFloat() : 0.0;
    //qDebug() << "TOTALWEIGHT" << value;

    // ---- //

    static const int spacing( 6 );
    static const int spacing_half( spacing / 2 );

    const int xstep( parentWidget()->width() / menuSize.width() );
    const int ystep( parentWidget()->height() / menuSize.height() );

    QRect r( x_ * xstep, y_ * ystep, w_ * xstep, h_ * ystep );

    if ( 0 < x_ )
        r.adjust( spacing_half, 0, 0, 0 );
    if ( x_ <= (menuSize.width() - 1) )
        r.adjust( 0, 0, -spacing_half, 0 );

    if ( 0 < y_ )
        r.adjust( 0, spacing_half, 0, 0 );
    if ( y_ <= (menuSize.height() - 1) )
        r.adjust( 0, 0, 0, -spacing_half );

    int a( 0 );

    if ( 1 == textHAlign_ )
        a |= Qt::AlignRight;
    else if ( 2 == textHAlign_ )
        a |= Qt::AlignLeft;
    else
        a |= Qt::AlignHCenter;

    if ( 1 == textVAlign_ )
        a |= Qt::AlignTop;
    else if ( 2 == textVAlign_ )
        a |= Qt::AlignBottom;
    else
        a |= Qt::AlignVCenter;

    QPixmap bg;

    if ( backgroundImage_.length() )
        if ( QFile::exists( backgroundImage_ ) )
            bg.load( backgroundImage_ );

    QPalette p( palette() );

    if ( textColor_.isValid() )
        p.setBrush( foregroundRole(), textColor_ );

    if ( !bg.isNull() ){
        p.setBrush( backgroundRole(), bg );
    }
    else if ( backgroundColor_.isValid() ){
        p.setBrush( backgroundRole(), backgroundColor_ );
    }

    QFont f( font() );

    if ( textFontFamily_.length() )
        f.setFamily( textFontFamily_ );

    if  ( 8 <= textFontPointSize_ )
        f.setPointSize( textFontPointSize_ );

    // ---- //

    setStyleSheet( QString() );

    setAlignment( (Qt::Alignment) a );
    setAutoFillBackground( true );
    setFont( f );
    setGeometry( r );
    setPalette( p );
    setText( text_ );

    setWordWrap(true);
    repaint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void speedKey::mousePressEvent(QMouseEvent *e)
{
    QPixmap bg;
    if ( backgroundImage_.length() )
        if ( QFile::exists( backgroundImage_ ) )
            bg.load( backgroundImage_ );

    QPalette p( palette() );
    /* invert the text color */
    if ( textColor_.isValid() )
        p.setBrush(foregroundRole(), QColor(255 - textColor_.red(), 255 - textColor_.green(),
                                                              255 - textColor_.blue(), textColor_.alpha()));
    /* invert background */
    if ( !bg.isNull() ){
        p.setBrush(backgroundRole(), invertPixmap(&bg) );
    }
    else if ( backgroundColor_.isValid() ){
        //special inversion case if Gray because you can't tell a difference between Grey and Gray
        if(backgroundColor_ == QColor("Gray")){
            p.setBrush(backgroundRole(),QColor(Qt::magenta));
        }
        else if(backgroundColor_ == QColor(Qt::magenta))
        {
            p.setBrush(backgroundRole(),QColor("Gray"));
        }
        else{
            p.setBrush(backgroundRole(), QColor(255 - backgroundColor_.red(), 255 - backgroundColor_.green(),
                                                              255 - backgroundColor_.blue(), backgroundColor_.alpha()) );
        }
    }
    setPalette( p );
    this->repaint();
    e->accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void speedKey::mouseReleaseEvent(QMouseEvent *e)
{
    QPixmap bg;

    if ( backgroundImage_.length() )
        if ( QFile::exists( backgroundImage_ ) )
            bg.load( backgroundImage_ );

    QPalette p( palette() );
    /* restore the text color */
    if ( textColor_.isValid() )
        p.setBrush(foregroundRole(), textColor_ );

    /* restore background */

    if ( !bg.isNull() ){
        p.setBrush(backgroundRole(), bg );
    }
    else if ( backgroundColor_.isValid() ){
        p.setBrush(backgroundRole(), backgroundColor_ );
    }
    setPalette( p );
    this->repaint();

    emit clicked(); //Emit signal to mainwindow
    this->setEnabled(true);
    e->accept();
}

QPixmap speedKey::invertPixmap(const QPixmap * src){
    QImage temp(src->toImage());
    temp.invertPixels();
    QPixmap inverted;
    inverted.convertFromImage(temp);
    return inverted;
}
