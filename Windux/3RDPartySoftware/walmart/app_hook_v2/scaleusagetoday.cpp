/**
 * @file scaleusagetoday.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "scaleusagetoday.h"
#include "stringsjson.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QJsonArray>

#if defined(Q_OS_LINUX)
static const QString USAGE_FILE( "/usr/share/walmart-apphook/productusage" );
#elif defined(Q_OS_WIN32)
static const QString USAGE_FILE( "productusage" );
#endif

QMutex scaleUsageToday::instanceMutex_;
scaleUsageToday *scaleUsageToday::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleUsageToday::scaleUsageToday( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive )
{
    // move to application thread
    moveToThread( QCoreApplication::instance()->thread() );

    // import last usage
    importLastUsage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleUsageToday::~scaleUsageToday()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleUsageToday::integerList scaleUsageToday::cakeLabels() const
{
    QMutexLocker guard( &m_ );
    return cakeLabels_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleUsageToday::setCakeLabels( const integerList& value )
{
    QMutexLocker guard( &m_ );
    cakeLabels_ = value;

    exportLastUsage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleUsageToday::updateUsage( int plu, int numLabels, PrintType type, bool isCakeLabel )
{
    const QDateTime now( QDateTime::currentDateTime() );

    LOG_TRACE << "update usage " << plu << " " << numLabels << " " << type << " " << isCakeLabel;

    QMutexLocker guard( &m_ );

    // force cake label when product is cake label
    if ( cakeLabels_.contains( plu ) )
    {
        LOG_DEBUG << "found cake label for plu " << plu;
        isCakeLabel = true;
    }

    // reset usage today when different day
    if ( now.date() != stamp_.date() )
    {
        LOG_INFO << "usage info reset!";
        usageToday_ = emptyUsage();
    }

    // update cake prints (not a product!)
    if ( isCakeLabel )
    {
        int cakePrints( usageToday_[JSON_CAKE_PRINTS].toInt() );
        cakePrints += numLabels;

        if ( cakePrints < 0 )
            cakePrints = 0;

        LOG_DEBUG << "cake prints " << cakePrints;
        usageToday_[JSON_CAKE_PRINTS] = cakePrints;
    }

    // locate this product
    else
    {
        const QJsonArray oldPrints( usageToday_[JSON_PLU_PRINTS].toArray() );

        QJsonArray newPrints;
        bool found( false );

        QJsonObject obj;

        foreach ( const QJsonValue& v, oldPrints )
            if ( !v.isObject() )
                LOG_WARN << "bad plu value!";
            else
            {
                const QJsonObject pluPrint( v.toObject() );

                const int pluNum( pluPrint[JSON_PLU_NUM].toInt() );

                if ( pluNum != plu )
                    newPrints.append( pluPrint );
                else
                {
                    obj = pluPrint;
                    found = true;
                }
            }

        if ( !found )
        {
            obj[JSON_PLU_NUM] = plu;
            obj[JSON_MANUAL_PRINTS] = 0;
            obj[JSON_REMOTE_PRINTS] = 0;
        }

        // update product usage
        int manual( obj[JSON_MANUAL_PRINTS].toInt() );
        int remote( obj[JSON_REMOTE_PRINTS].toInt() );

        if ( ManualPrint == type )
        {
            manual += numLabels;

            if ( manual < 0 )
                manual = 0;

            LOG_DEBUG << "manual prints " << manual;
            obj[JSON_MANUAL_PRINTS] = manual;
        }
        else if ( RemotePrint == type )
        {
            remote += numLabels;

            if ( remote < 0 )
                remote = 0;

            LOG_DEBUG << "remote prints " << remote;
            obj[JSON_REMOTE_PRINTS] = remote;
        }

        newPrints.append( obj );

        usageToday_[JSON_PLU_PRINTS] = newPrints;
    }

    // save!
    stamp_ = now;
    
    exportLastUsage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject scaleUsageToday::toJsonObject() const
{
    const QDateTime now( QDateTime::currentDateTime() );

    QMutexLocker guard( &m_ );

    // give usage today when its actually today
    QJsonObject result( emptyUsage() );

    if ( now.date() == stamp_.date() )
        result = usageToday_;

    // check for empty array
    const QJsonArray a( result[JSON_PLU_PRINTS].toArray() );

    if ( a.isEmpty() )
    {
        static const QJsonValue nullVal( QJsonValue::Null );

        // dont give empty array to caller, give null instead
        result[JSON_PLU_PRINTS] = nullVal;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleUsageToday *scaleUsageToday::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleUsageToday::importLastUsage()
{
    bool result( false );

    // import file
    if ( !QFile::exists( USAGE_FILE ) )
        LOG_WARN << "usage file does not exist";
    else
    {
        QFile f( USAGE_FILE );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "could not open usage file " << qPrintable( USAGE_FILE );
        else
        {
            QDataStream s( &f );
            s >> stamp_;
            s >> usageToday_;
            s >> cakeLabels_;

            f.close();

            // check validity
            if (( !stamp_.isNull() ) && ( stamp_.isValid() ) && ( !usageToday_.isEmpty() ))
                result = true;
        }
    }

    // use defaults when error
    if ( !result )
    {
        LOG_WARN << "using defaults";

        stamp_ = QDateTime::currentDateTime();
        usageToday_ = emptyUsage();
    }

    LOG_TRACE << "done " << result;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleUsageToday::exportLastUsage()
{
    QFile f( USAGE_FILE );

    if ( !f.open( QIODevice::WriteOnly ) )
    {
        LOG_WARN << "could not open usage file " << qPrintable( USAGE_FILE );
        return false;
    }

    QDataStream s( &f );
    s << stamp_;
    s << usageToday_;
    s << cakeLabels_;

    f.close();

    LOG_TRACE << "success";
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject scaleUsageToday::emptyUsage()
{
    static const QJsonArray a;

    QJsonObject obj;
    obj[JSON_CAKE_PRINTS] = 0;
    obj[JSON_PLU_PRINTS] = a;

    return obj;
}
