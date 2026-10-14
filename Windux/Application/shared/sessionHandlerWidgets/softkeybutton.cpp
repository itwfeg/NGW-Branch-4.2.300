//#define USE_QDEBUG
/**
 * @file softkeybutton.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <filesystem.h>
#include <logmanager.h>

#include "backgroundwidgettypes.h"
#include "softkeybutton.h"
#include "softkeyoverlaymanager.h"
#include <QObject>
#include <pixmap.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <Session.h>

#include <QApplication>
#include <QDir>
#include <QDrag>
#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QDebug>

/// Hobart namespace.
namespace hobart {

// font bold used for sofkeys
const bool softkeyButton::fontBold_ = false;

// font family used for sofkeys
const QString softkeyButton::fontFamily_ = QString( "Poppins" );        // FS UI - This worked

// font point size used for sofkeys
int softkeyButton::fontPointSize_ = 12;                                 // FS UI No effect - was it used?
int TWO_LINES   = 38;                                                   // FS V1 - not sure if this does anything
int THREE_LINES = 57;                                               // FS UI - not sure if this does anything

bool softkeyButton::selfServiceMode_ = false;

// timer to control blinking when selected
QTimer *softkeyButton::blinkTimer_ = NULL;

// mutex for saving thumbnail hints
QMutex softkeyButton::m_;

// thumbnail size hints
QMap<QString, QSize> softkeyButton::thumbHints_;

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton::softkeyButton( QWidget *parent ) :
    _Mybase( parent ),
    selected_( false ),
    toggled_( false ),
    overlayDisabled_( false ),
    canBlink_( true ),
    blinking_( false ),
    flashTimer_( NULL ),
    flashesRemaining_ (0),
    flashFlag_ ( false ),
    flashInfinitely_ (false),
    textAlignment_( Qt::AlignCenter ),
    cornerStyle_( NormalCornerStyle ),
    canDrag_( false ),
    dragging_( false ),
    dirty_( true ),
    isGen2UI_( false )
{
#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::softkeyButton():  parent = " << parent;
#endif
    // create timer
    if ( !blinkTimer_ )
    {
        blinkTimer_ = new QTimer();

        blinkTimer_->setInterval( 1000 );
        blinkTimer_->setSingleShot( false );
    }

    setFocusPolicy( Qt::NoFocus );
    setObjectName( "softkeyButton" );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // connect slots
    connect( this, SIGNAL( pressed() ), SLOT( update() ) );
    connect( this, SIGNAL( released() ), SLOT( update() ) );
    connect( blinkTimer_, SIGNAL( timeout() ), SLOT( onBlinkTimerTimeout() ) );
    //timer_ = new QTimer( this );

    //connect( timer_, &QTimer::timeout, this, &_Myt::onFlashTimeout );

    // create list of system folder names
    systemFolders_.append( "system/" );
    systemFolders_.append( "user/" );
    systemFolders_.append( "images/" );

    //cmdParser("flash 500 0");

    imageCache.setMaxCost(IMAGECACHE_MAX_ITEMS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton::~softkeyButton()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setBackgroundColor( const QColor& value )
{
    if ( value == backgroundColor_ )
        return;

    backgroundColor_ = value;
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setBackgroundImage( const QString& value )
{
    // search all A-Z
    if ( value == backgroundImage_ )
        return;

    // validate image (i.e. for remote access this will pull file)
    if ( value.length() )
        makeSoftkeyImage( value );

    backgroundImage_ = value;
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setFixedBackgroundImage( const QString& value )
{
    if ( value == backgroundImage_ )
        return;

    // validate image (i.e. for remote access this will pull file)
    if ( value.length() )
        makeSoftkeyImage( value );

    backgroundImage_ = value;
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setToggledBackgroundImage( const QString& value )
{
    if ( value == toggledBackgroundImage_ )
        return;

    // validate image (i.e. for remote access this will pull file)
    if ( value.length() )
        makeSoftkeyImage( value );

    toggledBackgroundImage_ = value;
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setGen2ActiveUI( bool value )
{
    isGen2UI_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setBorderColor( const QColor& value )
{
    if ( value == borderColor_ )
        return;

    borderColor_ = value;
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setCornerStyle( CornerStyle value )
{
    if ( value == cornerStyle_ )
        return;

    cornerStyle_ = value;
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setForegroundImage( const QString& value )
{
    if ( value == foregroundImage_ )
        return;

    // validate image (i.e. for remote access this will pull file)
    if ( value.length() )
        makeSoftkeyImage( value );

    foregroundImage_ = value;
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setSelected( bool value )
{
    if ( value == selected_ )
        return;

    if ( (selected_ = value) )
        blinkTimer_->start();
    else
        blinkTimer_->stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setTextAlignment( Qt::Alignment value )
{
    textAlignment_ = value;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setTextColor( const QColor& value )
{
    QPalette p( palette() );
    p.setColor( foregroundRole(), value );

    setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setToggled( bool value )
{
#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::setToggled(" << value << ")";
#endif
    LOG_CLASS_INFO( "softkeyButton", "setToggled" );

    LOG_TRACE( " ", NULL );

    if ( value == toggled_ )
        return;

    toggled_ = value;
    setDirtyFlag();

#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::setToggled(" << value << "):  Done";
#endif
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QColor softkeyButton::textColor() const
{
    QPalette p( palette() );

    return p.color( foregroundRole() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::create( int lang, const DomSoftkey* m, const DomSoftkeyStyle* style )
{
#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::create()";
#endif
    LOG_CLASS_INFO( "softkeyButton", "create" );

    LOG_TRACE( " ", NULL );

    if (( !m ) && ( !style ))
        return;

    setAction( m->action() );
    setActionFeature( m->actionFeature() );
    setGroupNum( m->groupNum() );
    setActionParam( m->actionParam() );

    if (( m->useDefBgColor() ) || ( m->bgColor().isEmpty() ))
    {
        QString c( m->defBgColor() );

        if ( !c.length() )
            c = style->backgroundColor();

        setBackgroundColor( c );
    }
    else
        setBackgroundColor( m->bgColor() );

    if (( m->useDefBgImage() ) || ( m->bgImage().isEmpty() ))
        setBackgroundImage( m->defBgImage() );
    else
        setBackgroundImage( m->bgImage() );

    // PJR-383
    setToggledBackgroundImage( m->bgToggledImage() );

    if (( m->useDefTextColor() ) || ( m->textColor().isEmpty() ))
    {
        QString c( m->defTextColor() );

        if ( !c.length() )
            c = style->textColor();

        setTextColor( c );
    }
    else
    {
        setTextColor( m->textColor() );
    }

    if ( 2 == lang )
        setText( m->useDefName2() ? m->defName2() : m->name2() );
    else
        setText( m->useDefName1() ? m->defName1() : m->name1() );

    setDirtyFlag();

#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::create():  Done";
#endif
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::deserialize( const QByteArray& a )
{
    QDataStream is( a );
    setAction( readFromDataStream<int>( is ) );
//    setGroupNum( readFromDataStream<int>( is ) );
    setActionFeature( readFromDataStream<int>( is ) );
    setActionParam( readFromDataStream<QString>( is ) );
    setBackgroundColor( readFromDataStream<QColor>( is ) );
    setBackgroundImage( readFromDataStream<QString>( is ) );
    setBorderColor( readFromDataStream<QColor>( is ) );
    setCornerStyle( (CornerStyle) readFromDataStream<int>( is ) );
    setForegroundImage( readFromDataStream<QString>( is ) );
    setGridPos( readFromDataStream<QPoint>( is ) );
    setGridSize( readFromDataStream<QSize>( is ) );
    setText( readFromDataStream<QString>( is ) );
    setTextAlignment( (Qt::Alignment) readFromDataStream<int>( is ) );
    setTextColor( readFromDataStream<QColor>( is ) );

    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray softkeyButton::serialize() const
{
    QByteArray a;
    QDataStream os( &a, QIODevice::WriteOnly );

    os << action();
    os << actionFeature();
    os << actionParam();
    os << backgroundColor();
    os << backgroundImage();
    os << borderColor();
    os << ((int) cornerStyle());
    os << foregroundImage();
    os << gridPos();
    os << gridSize();
    os << text();
    os << ((int) textAlignment());
    os << textColor();

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::resetBackgroundImage()
{
    backgroundImage_.clear();
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::resetToggledBackgroundImage()
{
    toggledBackgroundImage_.clear();
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::resetForegroundImage()
{
    foregroundImage_.clear();
    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::mouseMoveEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "softkeyButton", "mouseMoveEvent" );

    // sanity check
    if ( !e )
        return;

    // check for drag
    if (( isDraggable() ) && ( Qt::LeftButton & e->buttons() ))
    {
        int startDragDistance( qMax( 10, QApplication::startDragDistance() ) );
        int distance( (e->pos() - startPos_).manhattanLength() );

        if ( startDragDistance <= distance )
        {
            // force repaint of widget (we do not want to drag around a depressed pixmap)
            dragging_ = true;
            setDown( false );
            repaint();

            // create mime data
            QMimeData *mimeData = new QMimeData();
            mimeData->setData( softkeyButton::mimeDataType(), serialize() );
            mimeData->setText( text() );

            // create drag
            QDrag *drag = new QDrag( this );
            drag->setMimeData( mimeData );
            drag->setPixmap( grab() );

            LOG_INFO( "starting drag of button '%s' 0x%08x", qPrintable( text() ), this );

            // execute drag!
            Qt::DropAction a = drag->exec( Qt::MoveAction | Qt::CopyAction );

            LOG_INFO( "drag complete %d", a );
            dragging_ = false;

            // look for move action
            if ( Qt::MoveAction == a )
            {
                LOG_INFO( "object 0x%08x moved, deleting...", this );

                QTimer *deleteLaterTimer = new QTimer( this );
                deleteLaterTimer->setSingleShot( true );

                connect( deleteLaterTimer, SIGNAL( timeout() ), SLOT( deleteLater() ) );

                deleteLaterTimer->start( 0 );
            }

            LOG_DEBUG( "done", NULL );
            return;
        }
    }

    _Mybase::mouseMoveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::mousePressEvent( QMouseEvent *e )
{
    if ( Qt::LeftButton == e->button() )
        startPos_ = e->pos();

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::paintEvent( QPaintEvent *e )
{
    LOG_CLASS_INFO( "softkeyButton", "paintEvent" );
    Q_UNUSED( e );

    QPainter p( this );
    p.setRenderHint( QPainter::Antialiasing, true );

    // draw selected button when selected and blinking
    if (( isSelected() ) && ( blinkWhenSelected() ) && ( blinking_ ) && ( !dragging_ ))
    {
        p.fillRect( rect(), hobart::colorPalette::blue );
        return;
    }

    // need update
    if ( dirty_ )
    {
        updatePixmaps();
        dirty_ = false;
    }

    // draw background (but no overlay flash if we're in the editor)
    if (!(overlayDisabled_) && !(isDraggable()))
    {
        p.drawPixmap( rect(), (isDown() || flashFlag_ ? pressedImage_ : releasedImage_) );
    }
    else
    {
        p.drawPixmap( rect(), (isDown() ? pressedImage_ : releasedImage_) );
    }

    QPixmap nextOverlay;

    if (pixmapCache.count(std::pair<int, int>(this->width(), this->height()))  > 0)
    {
        nextOverlay = pixmapCache[std::pair<int,int>(this->width(), this->height())];
    }
    else
    {
        //std::string debug_message = "Redrawing overlay from (" + std::to_string(overlayPM_.width()) + "," + std::to_string(overlayPM_.height()) + ") to (";
        //debug_message += std::to_string(this->width()) + "," + std::to_string(this->height()) + ")";

        std::string debug_message = "string error 1";

        //LOG_INFO(debug_message.c_str(), NULL);

        updateOverlay();

        nextOverlay = overlayPM_;
    }

    // draws overlays if not on flashkey editor screen
    if( !(overlayDisabled_) && !(isDraggable()) )
    {
        if ( flashFlag_ )
        {
            invertPixmap inverter;
            nextOverlay = inverter( nextOverlay );
        }
        
        p.drawPixmap( rect(), nextOverlay );
    }

    writeText();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::applyOverlay( QPainter& p )
{
    LOG_CLASS_INFO( "softkeyButton", "applyOverlay" );

    QString currentFont = font().family();

    auto commands = (*softkeyOverlayManager::instance())[script_];

    p.setPen(Qt::NoPen); // don't draw borders on shapes

    for ( int i = 0; i < commands.size(); ++i )
    {
        softkeyOverlayCommand cmdDataObj = commands[i];

        switch(cmdDataObj.type)
        {
            case softkeyOverlayCommand::COMMAND_RECTANGLE:
                {
                    cmdDataObj.wSize = ( this->width() * cmdDataObj.wSize ) / 100;
                    cmdDataObj.hSize = ( this->height() * cmdDataObj.hSize ) / 100;
                    cmdDataObj.xPos = ( this->width() * cmdDataObj.xPos ) / 100;
                    cmdDataObj.xPos = cmdDataObj.xPos - ( cmdDataObj.wSize / 2 );
                    cmdDataObj.yPos = ( this->height() * cmdDataObj.yPos ) / 100;
                    cmdDataObj.yPos = cmdDataObj.yPos - (cmdDataObj.hSize / 2 );

                    QRect rect( QPoint(cmdDataObj.xPos, cmdDataObj.yPos), QSize(cmdDataObj.wSize, cmdDataObj.hSize) );

                    p.fillRect(rect, cmdDataObj.colorBrush);

                    LOG_INFO( "rectangle overlay command completed", NULL );
                }
                break;

            case softkeyOverlayCommand::COMMAND_TEXT:
                {
                    cmdDataObj.size = ( cmdDataObj.size * this->height() ) / 100;

                    cmdDataObj.xPos = ( this->width() * cmdDataObj.xPos ) / 100;
                    cmdDataObj.yPos = ( this->height() * cmdDataObj.yPos ) / 100;
                    cmdDataObj.yPos = cmdDataObj.yPos + (cmdDataObj.size / 2);

                    QFont font;
                    font.setPixelSize( cmdDataObj.size );
                    font.setFamily( currentFont );
                    QFontMetrics fm( font );
                    QRect fw = fm.boundingRect( fw , 0, cmdDataObj.qText );
                    cmdDataObj.xPos -= ( fw.width() / 2);
                    QPen pen(( QColor(cmdDataObj.qColor) ));

                    p.setPen( pen );
                    p.setFont( font );
                    p.drawText( QPoint(cmdDataObj.xPos, cmdDataObj.yPos), cmdDataObj.qText );
                    p.setPen(Qt::NoPen); // don't draw borders on shapes

                    LOG_INFO( "text overlay command completed", NULL );
                }
                break;

            case softkeyOverlayCommand::COMMAND_FONT:
                {
                    currentFont = cmdDataObj.qFontName;

                    LOG_INFO( "font overlay command completed", NULL );
                }
                break;

            case softkeyOverlayCommand::COMMAND_CIRCLE:
                {
                    if ( this->height() > this->width() )
                        cmdDataObj.radius = ( cmdDataObj.radius * this->width() ) / 100;
                    else
                        cmdDataObj.radius = ( cmdDataObj.radius * this->height() ) / 100;

                    cmdDataObj.xPos = ( this->width() * cmdDataObj.xPos ) / 100;
                    cmdDataObj.yPos = ( this->height() * cmdDataObj.yPos ) / 100;

                    p.setBrush( cmdDataObj.colorBrush );
                    p.drawEllipse( QPointF(cmdDataObj.xPos, cmdDataObj.yPos), cmdDataObj.radius, cmdDataObj.radius );

                    LOG_INFO( "circle overlay command completed", NULL );
                }
                break;
            
            case softkeyOverlayCommand::COMMAND_OVAL:
                {
                    cmdDataObj.wRadius = ( this->width() * cmdDataObj.wRadius ) / 100;
                    cmdDataObj.hRadius = ( this->height() * cmdDataObj.hRadius ) / 100;
                    cmdDataObj.xPos = ( this->width() * cmdDataObj.xPos ) / 100;
                    cmdDataObj.yPos = ( this->height() * cmdDataObj.yPos ) / 100;

                    p.setBrush( cmdDataObj.colorBrush );
                    p.drawEllipse( QPointF(cmdDataObj.xPos, cmdDataObj.yPos), cmdDataObj.wRadius, cmdDataObj.hRadius );

                    LOG_INFO( "oval overlay command completed", NULL );
                }
                break;

            case softkeyOverlayCommand::COMMAND_IMAGE:
            {
                cmdDataObj.width = ( this->width() * cmdDataObj.width ) / 100;
                cmdDataObj.height = ( this->height() * cmdDataObj.height ) / 100;
                cmdDataObj.xPos = ( this->width() * cmdDataObj.xPos ) / 100;
                cmdDataObj.xPos -= cmdDataObj.width / 2;
                cmdDataObj.yPos = ( this->height() * cmdDataObj.yPos ) / 100;
                cmdDataObj.yPos -= cmdDataObj.height / 2;

                QString fullPath = cmdDataObj.qFilePath + cmdDataObj.qFileType;

                if (imageCache.contains(fullPath) == 0)
                {
                    QImage* newImage = new QImage(fullPath); // n.b. the cache takes ownership and will delete this
                    imageCache.insert(fullPath, newImage);
                }

                QImage image(*imageCache[fullPath]);

                if ( image.isNull() )
                {
                    LOG_DEBUG( "Image could not be found and loaded", NULL );
                    break;
                }

                if ( cmdDataObj.qImgFit == "stretch" )
                {
                    QImage resizedImg = image.scaled( cmdDataObj.width, cmdDataObj.height, Qt::IgnoreAspectRatio );
                    p.drawImage( cmdDataObj.xPos, cmdDataObj.yPos, resizedImg );
                }
                else if ( cmdDataObj.qImgFit == "fit" )
                {
                    QImage resizedImg = image.scaled( cmdDataObj.width, cmdDataObj.height, Qt::KeepAspectRatio );
                    p.drawImage( cmdDataObj.xPos + cmdDataObj.width/2 - resizedImg.width()/2, cmdDataObj.yPos + cmdDataObj.height/2 - resizedImg.height()/2, resizedImg );              
                }
                else
                {
                    LOG_DEBUG( "The image display specified is invalid.", NULL );
                    break;
                }

                LOG_INFO( "image overlay command completed", NULL );
            }
            break;

            default:
                LOG_DEBUG( "A command specified does not match the allowed command types.", NULL );
                break;
        }
    }

    setDirtyFlag();
    //LOG_INFO( "done. dirty flag set", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::onFlashTimeout()
{
    LOG_CLASS_INFO( "softkeyButton", "onFlashTimeout" );

    if (flashInfinitely_)
    {
        flashFlag_ = !flashFlag_;
    }
    else
    {
        flashesRemaining_--;

        if (flashesRemaining_ <= 0)
        {
            // done, stop flashing
            if (flashTimer_) flashTimer_->stop();
            flashFlag_ = false;
        }
        else
        {
            flashFlag_ = !flashFlag_;
        }
    }

    setDirtyFlag();

    //LOG_INFO("flash timer hit", NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::updateOverlay()
{
    LOG_CLASS_INFO( "softkeyButton", "updateOverlay" );
    //LOG_WARN("updateOverlay for script %s", script_.toStdString().c_str());
    
    overlayPM_ = QPixmap( this->width(), this->height() );
    overlayPM_.fill( Qt::transparent );
    QPainter p( &overlayPM_ );
    applyOverlay( p );
    
    pixmapCache[std::pair<int, int>(this->width(), this->height())] = overlayPM_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setScript(QString &value)
{
    LOG_CLASS_INFO( "softkeyButton", "setScript" );

    if ( value.length() > 0 )
    {
        script_ = value;
        LOG_WARN("setScript: \"%s\"", value.toStdString().c_str());
        QObject::connect(softkeyOverlayManager::instance(), &softkeyOverlayManager::newData, this, &softkeyButton::newData);
    }
    
    newData(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::newData(const QString& scriptName)
{
    bool updateTimer = false;
    int flashSpeed = 0;
    if (scriptName == script_)
    {
        auto commands = (*softkeyOverlayManager::instance())[script_];

        // flash commands need to only be handled when new data is received

        for ( auto cmd : commands) 
        {
            if(softkeyOverlayCommand::COMMAND_FLASH == cmd.type)
            {
                if ( cmd.duration == 0)
                {
                    flashInfinitely_ = true;
                } 
                else
                {
                    flashesRemaining_ = (cmd.duration * 2) - 1;
                    flashInfinitely_ = false;
                }

                flashSpeed = cmd.speed;
                flashFlag_ = true;
                updateTimer = true;
            }
        }

        if (updateTimer)
        {
            if ( flashTimer_ != NULL)
            {
                delete flashTimer_;
                flashTimer_ = NULL;
            }

            flashTimer_ = new QTimer(this);
            connect(flashTimer_, &QTimer::timeout, this, &softkeyButton::onFlashTimeout);
            flashTimer_->start(flashSpeed);

            setDirtyFlag();
        }
        else if( updateTimer == false && (flashInfinitely_)  && flashTimer_ != NULL)
        {
            flashFlag_ = false;
            flashTimer_->stop();
            setDirtyFlag();
        }

        // if this script has new commands, dump all previously rendered pixmaps
        pixmapCache.clear();

        updateOverlay();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::writeText()
{
    LOG_CLASS_INFO( "softkeyButton", "mouseMoveEvent" );
    QPainter p( this );

    // text font point size
    QFont f( p.font() );

    if( isGen2UI_ )
    {
        f.setBold( fontBold_ );

        //if ( fontFamily_.length() )
        //    f.setFamily( fontFamily_ );
        f.setFamily( "Poppins SemiBold" );
        QString test_text = text().toUpper();

        f.setPointSize( fontPointSize_ );
        p.setFont( f );

        // TODO FS UI - mess with the text here on Search Flashkey results here
        QMargins margins( textMargins() );
        f.setFamily( "Poppins SemiBold" );
        f.setPointSize( 17 );
        QRect r( rect().adjusted( 15, margins.top(), -margins.right(), -margins.bottom() ) );

        QFontMetrics metrics( f );
        QRect boundingRect(  metrics.boundingRect( r, Qt::AlignLeft | Qt::AlignBottom | Qt::TextWordWrap, test_text ) );

        //QString elidedText( metrics.elidedText( test_text, Qt::ElideRight, text_width ) );

        //LOG_INFO( "Rectangle width: %d", r.width() );
        //LOG_INFO( "Rectangle height: %d", r.height() );
        //LOG_INFO( "boundingRect orig top: 80", NULL );
        //LOG_INFO( "boundingRect top: %d", ( r.height() / 2 ) );

        int bound_top = ( r.height() / 2 );
        boundingRect.setTop( bound_top + 5 );               // orig: 80
        boundingRect.setBottom( bound_top + 55 );           // orig: 135
        boundingRect.setWidth( r.width() - 15 );            // orig: 150

        p.setPen( QPen( hobart::colorPalette::pluDescDark, 4 ) );
        //p.drawText( boundingRect, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignBottom, elidedText );
        p.drawText( boundingRect, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignBottom, test_text );
    }
    else
    {
        // Original method of drawing text on flashkeys

        // text font point size
        f.setBold( fontBold_ );
        f.setPointSize( fontPointSize_ );

        if ( fontFamily_.length() )
            f.setFamily( fontFamily_ );

        p.setFont( f );

        QMargins margins( textMargins() );
        QRect r( rect().adjusted( margins.left(), margins.top(), -margins.right(), -margins.bottom() ) );

        // draw text
        p.setPen( palette().color( (isEnabled() ? QPalette::Active : QPalette::Disabled), QPalette::ButtonText ) );
        p.drawText( r, Qt::TextWordWrap | textAlignment(), text() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    setDirtyFlag();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::onBlinkTimerTimeout()
{
    QTime now( QTime::currentTime() );

    blinking_ = (0 == (now.second() % 2));

    if ( isSelected() )
        update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::setDirtyFlag()
{
#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::setDirtyFlag()";
#endif
    LOG_CLASS_INFO( "softkeyButton", "setDirtyFlag" );

    LOG_TRACE( " ", NULL );

    dirty_ = true;
    update();

#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::setDirtyFlag():  Done";
#endif
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::updatePixmaps()
{
#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::updatePixmaps()";
#endif
    LOG_CLASS_INFO( "softkeyButton", "updatePixmaps" );

    LOG_TRACE( " ", NULL );

    QRect rectHalfPen( rect().adjusted( 0, 0, -1, -1 ) );

    // FT-976
    //
    // Note:
    // When a background image is present and valid we will show it instead of the background
    // color. The image should override the color and display instead of.
    //

    // create background image
    QPixmap background;

    if ( backgroundImage_.length() )
    {
        QString thumb;
        QString img = makeSoftkeyImage( backgroundImage_, &thumb );

        background = thumb.length() ? thumb : img;

        updateThumbnailHint( backgroundImage_, size() );
    }

    // create pixmap for released state
    releasedImage_ = QPixmap( size() );
    releasedImage_.fill( Qt::transparent );

    QPainter p( &releasedImage_ );
    p.setRenderHint( QPainter::Antialiasing, true );

    // fill background color (when no image)
    if (( backgroundColor_.isValid() ) && ( background.isNull() ))
    {
        if ( NormalCornerStyle == cornerStyle() )
            p.fillRect( rectHalfPen, QBrush( backgroundColor_ ) );
        else if ( RoundedCornerStyle == cornerStyle() )
        {
            p.setBrush( backgroundColor_ );
            p.setPen( backgroundColor_ );
            p.drawRoundedRect( rectHalfPen, 5.0, 5.0 );
        }
    }

    // draw border
    if ( borderColor_.isValid() )
    {
        p.setBrush( Qt::transparent );
        p.setPen( borderColor_ );

        if ( NormalCornerStyle == cornerStyle() )
            p.drawRect( rectHalfPen );
        else if ( RoundedCornerStyle == cornerStyle() )
            p.drawRoundedRect( rectHalfPen, 5.0, 5.0 );
    }

    // fill background image
    if ( !background.isNull() )
    {
        // background image is stretched to fit button
        p.drawPixmap( rect(), background );
    }

    // fill foreground image
    if ( foregroundImage_.length() )
    {
        QString thumb;
        QString img = makeSoftkeyImage( foregroundImage_, &thumb );

        QPixmap temp = thumb.length() ? thumb : img;

        if ( !temp.isNull() )
        {
            QRect targetRect( rect() );

            // center x
            if ( temp.width() < width() )
            {
                targetRect.setLeft( (width() - temp.width()) / 2 );
                targetRect.setWidth( temp.width() );
            }

            // center y
            if ( temp.height() < height() )
            {
                targetRect.setTop( (height() - temp.height()) / 2 );
                targetRect.setHeight( temp.height() );
            }

            p.drawPixmap( targetRect, temp );
        }

        if ( temp.height() < temp.width() )
            updateThumbnailHint( foregroundImage_, QSize( width(), 0 ) );
        else
            updateThumbnailHint( foregroundImage_, QSize( 0, height() ) );
    }

    // create reverse of image for pressed state
    invertPixmap inverter;
    pressedImage_ = inverter( releasedImage_ );

#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::updatePixmaps():  Done";
#endif
   LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString softkeyButton::makeSoftkeyImage( const QString& filename, QString *thumb ) const
{
#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::makeSoftkeyImage()";
#endif
    LOG_CLASS_INFO( "softkeyButton", "makeSoftkeyImage" );

    LOG_TRACE( " ", NULL );

    if ( !filename.length() )
        return QString();

    QString image( filename );
    QString path;

    folderType::type t( folderType::Images );
    int from( 0 );
    int index;

    // change all backslash to forward slash
    image.replace( "\\", "/" );

    // strip out system pathing
    foreach ( const QString& f, systemFolders_ )
        if ( f == image.mid( from, f.length() ) )
            from += f.length();

    // just want the filename, not any system pathing before it
    if ( -1 != (index = image.indexOf( "/", from )) )
    {
        path = image.left( index );
        image = image.right( image.length() - index - 1 );
    }

    // look for special folders
    if ( path.contains( "categories" ) )
        t = folderType::CategoriesImages;
    else if ( path.contains( "keyboards" ) )
        t = folderType::KeyboardImages;
    else if ( path.contains( "products" ) )
        t = folderType::ProductsImages;
    else if ( path.contains( "selfServe" ) )
        t = folderType::SelfServeImages;
    else if ( path.contains( "softkeys" ) )
        t = folderType::FlashkeyImages;
    else if ( path.contains( "specials" ) )
    {
        // FT-668
        // As per Scott Davis and Debra Dyer, we are not overlaying specials images. If, for some
        // reason, this becomes a requirement again restore the commented code.
        //t = folderType::SpecialsImages;

        return QString();
    }

#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::makeSoftkeyImage():  Done";
#endif
    LOG_TRACE( "Done", NULL );

    // SOF-4258
    // check if thumbnail exists first before using actual image
    if (( thumb ) && ( !image.contains( ".thumb." ) ))
    {
        QStringList fileTypes;
        fileTypes.append( ".bmp" );
        fileTypes.append( ".gif" );
        fileTypes.append( ".jpg" );
        fileTypes.append( ".jpeg" );
        fileTypes.append( ".png" );

#ifdef Q_OS_LINUX
        fileTypes.append( ".BMP" );
        fileTypes.append( ".GIF" );
        fileTypes.append( ".JPG" );
        fileTypes.append( ".JPEG" );
        fileTypes.append( ".PNG" );
#endif

        // Add more files here that we dont want thumbnailed
        QStringList imageExceps;
        imageExceps.append( "less.png" );
        imageExceps.append( "more.png" );

        foreach ( const QString& no_thumb, imageExceps )
            if( image.contains( no_thumb ) )
                return filesystem::absoluteFilePath( image, t );

        foreach ( const QString& fileType, fileTypes )
            if ( image.endsWith( fileType ) )
            {
                QString thumbImage = image;
                thumbImage.replace( fileType, ".thumb" + fileType );

                QString f = filesystem::absoluteFilePath( thumbImage, t );

                if ( QFile::exists( f ) )
                    (*thumb) = f;

                break;
            }
    }

    return filesystem::absoluteFilePath( image, t );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QColor softkeyButton::makeColor( const QString& c )
{
    // hexadecimal string (i.e. 80FF80)
    if (( 6 <= c.length() ) && ( c.length() <= 7 ))
    {
        if ( "#" != c.left( 1 ) )
            return QColor( "#" + c );

        return QColor( c );
    }

    return QColor();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButton::updateThumbnailHint( const QString& thumb, const QSize& s )
{
    QMutexLocker guard( &m_ );

    QSize shint = s;

    if ( thumbHints_.contains( thumb ) )
    {
        const QSize temp = thumbHints_[thumb];

        // since images are usually scaled down, use the larger of the two sizes to prevent jaggies when scaling up
        if (( temp.width() < s.width() ) ||
            ( temp.height() < s.height() ))
            shint = s;

        // no changes... nothing to do
        if ( temp == shint )
            return;
    }

    thumbHints_[thumb] = shint;

    // write file
    QFile f( QDir::tempPath() + QDir::separator() + "thumbsizes" );

    if ( f.open( QFile::WriteOnly | QFile::Text ) )
    {
        for ( QMap<QString, QSize>::const_iterator i = thumbHints_.constBegin(); i != thumbHints_.constEnd(); ++i )
        {
            const QString line = QString( "%1,%2,%3\n" ).arg( i.key() ).arg( i.value().width() ).arg( i.value().height() );
            f.write( line.toLatin1() );
        }

        f.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *softkeyButtonFactory::create( QWidget *parent ) const
{
#ifdef USE_QDEBUG
    qDebug() << "softkeyButton::create()";
#endif
    LOG_CLASS_INFO( "softkeyButton", "sbf::create" );

    LOG_TRACE( " ", NULL );

    // create new button
    return new softkeyButton( parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void softkeyButtonFactory::destroy( softkeyButton *obj ) const
{
    // delete the object
    delete obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

