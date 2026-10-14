/**
 * @file uartedgify.cpp
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "uartedgify.h"

#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QTimer>

static const QString CMD_PING( "PING" );
static const QString CMD_PONG( "PONG" );

static const QString CMD_PREDICT( "WHAT" );
static const QString CMD_SET_GROUND_TRUTH( "TRUE" );

static const QString CMD_STATS( "STAT" );
static const QString CMD_STATS_INFO( "SINF" );

static const QString CMD_PREDICT_RESULT( "PRED" );

static const QString CMD_EXECUTE( "CMD " );
static const QString CMD_EXECUTE_INFO( "CINF" );

static const QString CMD_SET_DATE( "DATE" );

static const QString RESET_SEQUENCE( "^CON^RESET^CON^RESET^" );

#if defined( Q_OS_WIN )
static const QString INPUT_FILE( "cmdin.txt" );
static const QString OUTPUT_FILE( "cmdout.txt" );
#elif defined( Q_OS_LINUX )
static const QString INPUT_FILE( "/usr/share/edgify-apphook/cmdin.txt" );
static const QString OUTPUT_FILE( "/usr/share/edgify-apphook/cmdout.txt" );
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyuart::edgifyuart( const QString& name, int baud, QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    processing_( false ),
    errors_( 0 ),
    resets_( 0 ),
    stats_( 0 )
{
    connect( this, &_Myt::sendCommand, this, &_Myt::onSendCommand, Qt::QueuedConnection );

    // open uart
    uart_ = new QSerialPort( name, this );
    uart_->setBaudRate( baud );

    LOG_INFO << "uart name " << qPrintable( name );
    LOG_INFO << "uart baud rate " << uart_->baudRate();

#if QT_VERSION < QT_VERSION_CHECK( 5, 8, 0 )
    connect( uart_, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>( &QSerialPort::error ), this, &_Myt::onErrorOccurred );
#else
    connect( uart_, &QSerialPort::errorOccurred, this, &_Myt::onErrorOccurred );
#endif

    connect( uart_, &QSerialPort::readyRead, this, &_Myt::onReadyRead );

    if ( !uart_->open( QSerialPort::ReadWrite ) )
        LOG_ERROR << "failed to open uart " << qPrintable( name ) << " baud " << baud;
    else
    {
        LOG_INFO << "uart open " << qPrintable( name ) << " " << baud;
    }

    // setup ping timer
    pingTimer_ = new QTimer( this );
    pingTimer_->setInterval( PING_TIMEOUT );
    pingTimer_->setSingleShot( false );

    connect( pingTimer_, &QTimer::timeout, this, &_Myt::onTimeout );

    pingTimer_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyuart::~edgifyuart()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool edgifyuart::valid() const
{
    QMutexLocker guard( &m_);

    return (( uart_->isOpen() ) && ( QSerialPort::NoError == uart_->error() ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::forceReset()
{
    QMutexLocker guard( &m_);
    resetSerialPort();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::executeCommand( const QString& cmd )
{
    LOG_TRACE << "execute command '" << qPrintable( cmd ) << "'...";
    sendCommand( CMD_EXECUTE, cmd.toLatin1() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::predict()
{
    LOG_TRACE << "predict...";
    sendCommand( CMD_PREDICT );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::setDate()
{
    const QDateTime now( QDateTime::currentDateTime() );
    const QString s( now.toString( "yyyy-MM-dd HH:mm" ) );

    LOG_TRACE << "setting date to '" << qPrintable( s ) << "'...";
    sendCommand( CMD_SET_DATE, s.toLatin1() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::setGroundTruth( const QJsonDocument& doc )
{
    LOG_TRACE << "set ground truth...";
    sendCommand( CMD_SET_GROUND_TRUTH, doc.toJson( QJsonDocument::Compact ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::onErrorOccurred( QSerialPort::SerialPortError error )
{
    LOG_WARN << "have serial port error " << error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::onReadyRead()
{
    QMutexLocker guard( &m_);

    // read all pending bytes
    buffer_.append( uart_->readAll() );

    // parse buffer
    if ( !processing_ )
    {
        processing_ = true;
        parseBuffer();

        processing_ = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::onSendCommand( const QString& cmd, const QByteArray& payload )
{
    QMutexLocker guard( &m_);

    if ( !valid() )
    {
        LOG_WARN << "serial port not valid";
        ++errors_;

        return;
    }
    else if ( 4 != cmd.length() )
        return;

    // determine message length
    QString length( QString::number( 4 + payload.size() ) );

    while ( length.length() < 6 )
        length = '0' + length;

    // generate message
    QByteArray a;
    a.append( length.toLatin1() );
    a.append( cmd.toLatin1() );
    a.append( payload );

    // send!
    LOG_DEBUG << "send command " << qPrintable( cmd ) << " (" << a.size() << ")";

    uart_->write( a );

    // wait for write to complete
    if ( !uart_->waitForBytesWritten( WRITE_TIMEOUT ) )
    {
        LOG_WARN << "failed to write!";
        ++errors_;
    }
    else
    {
        LOG_TRACE << "send success";

        // for ping event increment errors until we get a response
        if ( CMD_PING == cmd )
            ++errors_;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::onTimeout()
{
    QMutexLocker guard( &m_);

    // attempt to reset comms
    if ( 4 <= errors_ )
        resetSerialPort();

    // execute command
    // - or -
    // request status (every so often)
    // - or -
    // send ping
    if (( QFile::exists( INPUT_FILE ) ) && ( valid() ))
    {
        QFile f( INPUT_FILE );

        if ( !f.open( QFile::ReadOnly | QFile::Text ) )
            LOG_WARN << "failed to open input file for read";
        else
        {
            QTextStream is( &f );

            LOG_TRACE << "execute command...";
            executeCommand( is.readAll() );

            f.close();

            LOG_TRACE << "remove file...";
            QFile::remove( INPUT_FILE );
        }
    }
    else if ( 0 == (++stats_ % STATS_REQUEST) )
    {
        setDate();

        LOG_TRACE << "stats request...";
        sendCommand( CMD_STATS );
    }
    else
    {
        LOG_TRACE << "ping...";
        sendCommand( CMD_PING );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::processCommand( const QString& cmd, const QByteArray& payload )
{
    LOG_DEBUG << "process command " << qPrintable( cmd ) << " (" << payload.length() << ")...";

    bool unknown( false );

    // response to ping command
    if ( CMD_PING == cmd )
        sendCommand( CMD_PONG );

    // their response to our ping command
    else if ( CMD_PONG == cmd )
    {
        ; // nothing to do
    }

    // prediction response (JSON data)
    else if ( CMD_PREDICT_RESULT == cmd )
    {
        QJsonParseError err;

        // parse!
        const QJsonDocument doc( QJsonDocument::fromJson( payload, &err ) );

        if ( QJsonParseError::NoError != err.error )
            LOG_WARN << "error " << err.error << " parsing network request JSON document at offset " << err.offset << " " << qPrintable( err.errorString() );
        else
        {
            LOG_INFO << "have prediction";

            emit processDocument( QJsonDocument(), 200, doc );
        }
    }

    // stats information
    else if ( CMD_STATS_INFO == cmd )
    {
        const QStringList fields( QString( payload ).split( '|' ) );

        if ( 0 < fields.size() )
            LOG_INFO << "date:                  " << qPrintable( fields[0] );
        if ( 1 < fields.size() )
            LOG_INFO << "version:               " << qPrintable( fields[1] );
        if ( 2 < fields.size() )
            LOG_INFO << "camera attached:       " << qPrintable( fields[2] );
        if ( 3 < fields.size() )
            LOG_INFO << "edgify active:         " << qPrintable( fields[3] );
        if ( 4 < fields.size() )
            LOG_INFO << "edgify 3000 listening: " << qPrintable( fields[4] );
        if ( 5 < fields.size() )
            LOG_INFO << "edgify 8080 listening: " << qPrintable( fields[5] );
        if ( 6 < fields.size() )
            LOG_INFO << "edgify 8090 listening: " << qPrintable( fields[6] );
        if ( 7 < fields.size() )
            LOG_INFO << "edgify num samples:    " << qPrintable( fields[7] );
    }

    // command execution results
    else if ( CMD_EXECUTE_INFO == cmd )
    {
        if ( QFile::exists( OUTPUT_FILE ) )
            QFile::remove( OUTPUT_FILE );

        LOG_INFO << "have execution results";

        QFile f( OUTPUT_FILE );

        if ( !f.open( QFile::WriteOnly | QFile::Text ) )
            LOG_WARN << "failed to open output file for write";
        else
        {
            QTextStream os( &f );
            os << payload;

            f.close();
        }
    }

    // unknown
    else
    {
        LOG_WARN << "unknown command " << qPrintable( cmd );
        unknown = true;
    }

    // zero errors and resets if command was valid
    if ( unknown )
        ++errors_;
    else
    {
        errors_ = resets_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::parseBuffer()
{
    for ( ;; )
    {
        LOG_TRACE << "parse buffer " << buffer_.size();

        if ( buffer_.size() < 6 )
            break;

        for ( int i( 6 ); i--; )
            if (( buffer_[i] < '0' ) || ( '9' < buffer_[i] ))
            {
                LOG_WARN << "protocol error: length should only contain digits";
                buffer_.clear();
                break;
            }

        const int length( buffer_.left( 6 ).toInt() );

        if ( length < 4 )
        {
            LOG_WARN << "protocol error: length should be 4 or higher";
            buffer_.clear();
            break;
        }

        // partial message
        if ( buffer_.size() < (6+length) )
            break;

        const QString cmd( buffer_.mid( 6, 4 ) );
        const QByteArray cmdData( buffer_.mid( 10, length-4 ) );

        // process
        processCommand( cmd, cmdData );

        buffer_ = buffer_.mid( 6 + length );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyuart::resetSerialPort()
{
    LOG_WARN << "reset serial port!";

    // too many resets
    // close the port and try to open again
    if ( 3 <= ++resets_ )
    {
        LOG_WARN << "too many resets!";

        uart_->close();

        if ( !uart_->open( QSerialPort::ReadWrite ) )
            LOG_ERROR << "failed to open uart";

        resets_ = 0;
    }

    // attempt to reset connection and data
    uart_->flush();
    uart_->clearError();

    buffer_.clear();

    errors_ = 0;

    uart_->write( RESET_SEQUENCE.toLatin1() );
}

