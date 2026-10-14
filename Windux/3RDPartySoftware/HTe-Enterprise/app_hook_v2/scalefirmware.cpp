/**
 * @file scalefirmware.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "scaleconfig.h"
#include "scalefirmware.h"
#include "stringsjson.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

QString scaleFirmware::buildNumber_;

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleFirmware::scaleFirmware( QObject *parent ) :
    _Mybase( parent ),
    features_( installedFeatures() ),
    licenses_( installedLicenses() )
{
    // read version info
    readVersionInfo();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleFirmware::~scaleFirmware()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleFirmware::isValid() const
{
    return versions_.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject scaleFirmware::toJsonObject() const
{
    // get features
    QJsonArray features;

    foreach ( const QString& feature, features_ )
        features.append( feature );

    // get licensing
    QJsonArray licenses;

    foreach ( const QString& license, licenses_ )
        licenses.append( license );

    // ---- //

    QJsonObject obj;

    if ( buildNumber_.length() )
        obj[JSON_BUILD_NUMBER] = buildNumber_;

    obj[JSON_APPLICATION] = versions_["Release ID"];

#if defined(Q_OS_LINUX)
    obj[JSON_OPERATING_SYSTEM] = versions_["OS Version"];
    obj[JSON_SYSTEM_CONTROLLER] = versions_["HW Rev"];
    obj[JSON_LOADER] = versions_["Bootloader Version"];
    obj[JSON_SM_BACKEND] = versions_["SM Backend Version"];
#endif

    obj[JSON_FEATURES] = features;
    obj[JSON_LICENSES] = licenses;

    return obj;
}


QString scaleFirmware::versions(QString id) const
{
    if ( isValid() )
        return versions_[id];
    return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleFirmware::readVersionInfo()
{
#if defined(Q_OS_LINUX)
    static const QString file( "/SOFTWARE_VERSION_INFO" );
#elif defined(Q_OS_WIN32)
    static const QString file( "SOFTWARE_VERSION_INFO" );
#endif

    if ( !scaleConfig::instance()->parseConfigFile( file, ": ", versions_ ) )
        return false;

    versions_["Bootloader Version"] = ubootVer();

#if defined(Q_OS_LINUX)
    versions_["OS Version"] = opersysVer();
    versions_["SM Backend Version"] = smbackendVer();
    versions_["HW Rev"] = hardwareRev();
#endif

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleFirmware::opersysVer()
{
    QString result;

#if defined(Q_OS_LINUX)
    static const QString program( "uname" );

    QStringList args;
    args.append( "-r" );

    QProcess p;
    p.start( program, args );

    if ( p.waitForFinished() )
    {
        const QByteArray a( p.readAllStandardOutput() );
        result = QString::fromLatin1( a ).trimmed();
    }

#endif

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleFirmware::smbackendVer()
{
    QString result;

#if defined(Q_OS_LINUX)
    static const QString program( "python" );

    QStringList args;
    args.append( "/usr/local/hobart/python/getSmBackendVersion.py" );
    args.append( "/usr/local/hobart/backend" );

    QProcess p;
    p.start( program, args );

    if ( p.waitForFinished() )
    {
        const QByteArray a( p.readAllStandardOutput() );
        result = QString::fromLatin1( a ).trimmed();
    }

#endif

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleFirmware::ubootVer()
{
    QString result;

#if defined(Q_OS_LINUX)
    static const QString file( "/uBootVersion" );
#elif defined(Q_OS_WIN32)
    static const QString file( "uBootVersion" );
#endif

    if ( !QFile::exists( file ) )
        LOG_WARN << "uboot version file " << qPrintable( file ) << " does not exist";
    else
    {
        QFile f( file );

        if ( !f.open( QFile::ReadOnly | QFile::Text ) )
            LOG_WARN << "could not open uboot version file " << qPrintable( file );
        else
        {
            QTextStream s( &f );

            // parse first line
            while ( !s.atEnd() )
            {
                const QString line( s.readLine() );

                LOG_TRACE << qPrintable( line );

                if ( line.length() )
                {
                    result = line.trimmed();
                    break;
                }
            }

            f.close();
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleFirmware::hardwareRev()
{
    QString result;

#if defined(Q_OS_LINUX)
    static const QString program( "python" );

    QStringList args;
    args.append( "/usr/local/hobart/python/hwRev.py" );

    QProcess p;
    p.start( program, args );
    p.waitForFinished();

#endif

#if defined(Q_OS_LINUX)
    static const QString file( "/etc/hobart/hwrev.dat" );
#elif defined(Q_OS_WIN32)
    static const QString file( "hwrev.dat" );
#endif

    if ( !QFile::exists( file ) )
        LOG_WARN << "hwrev file " << qPrintable( file ) << " does not exist";
    else
    {
        QFile f( file );

        if ( !f.open( QFile::ReadOnly | QFile::Text ) )
            LOG_WARN << "could not open hwrev file " << qPrintable( file );
        else
        {
            QTextStream s( &f );

            // parse first line
            while ( !s.atEnd() )
            {
                const QString line( s.readLine() );

                LOG_TRACE << qPrintable( line );

                if ( line.length() )
                {
                    result = line.trimmed();
                    break;
                }
            }

            f.close();
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList scaleFirmware::installedFeatures()
{
    QStringList result;

#if defined(Q_OS_LINUX)
    static const QString file( "/usr/local/hobart/features/FEATURES_INSTALLED" );
#elif defined(Q_OS_WIN32)
    static const QString file( "FEATURES_INSTALLED" );
#endif

    if ( !QFile::exists( file ) )
        LOG_WARN << "features file " << qPrintable( file ) << " does not exist";
    else
    {
        QFile f( file );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "could not open features file " << qPrintable( file );
        else
        {
            QXmlStreamReader xml( &f );

            // parse xml
            while ( !xml.atEnd() )
            {
                if ( xml.hasError() )
                {
                    LOG_WARN << "could not parse features file " << qPrintable( file );
                    break;
                }

                // found installed feature
                if (( QXmlStreamReader::StartElement == xml.readNext() ) && ( "feature" == xml.name() ))
                {
                    const QXmlStreamAttributes attr( xml.attributes() );

                    const QString activationDate( attr.value( "activation_date" ).toString() );
                    const QString name( attr.value( "name" ).toString() );
                    const QString version( attr.value( "version" ).toString() );

                    LOG_INFO << "feature " << qPrintable( name );

                    if ( name.length() )
                        result.append( name );
                }
            }

            f.close();
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList scaleFirmware::installedLicenses()
{
    QStringList result;

#if defined(Q_OS_LINUX)
    static const QString file( "/dev/mtd6" );
#elif defined(Q_OS_WIN32)
    static const QString file( HOBART_LOCAL_DIR + "mtd6" );
#endif

    if ( !QFile::exists( file ) )
        LOG_WARN << "license file " << qPrintable( file ) << " does not exist";
    else
    {
        QFile f( file );

        if ( !f.open( QFile::ReadOnly ) )
            LOG_WARN << "could not open license file " << qPrintable( file );
        else
        {
            // read file
            QByteArray a( f.readAll() );
            f.close();

            // terminate file at EOF character
            for ( int i = 0; i < a.size(); ++i )
                if (( 0x0 == a[i] ) || ( ((char)0xFF) == a[i] ))
                {
                    a.truncate( i );
                    break;
                }

            QXmlStreamReader xml( a );

            // parse xml
            while ( !xml.atEnd() )
            {
                if ( xml.hasError() )
                {
                    LOG_WARN << "could not parse license file " << qPrintable( file );
                    break;
                }

                // found license
                if (( QXmlStreamReader::StartElement == xml.readNext() ) && ( "license" == xml.name() ))
                    result.append( parseLicense( xml ) );
            }
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleFirmware::parseLicense( QXmlStreamReader& xml )
{
    QString result;

    bool products( false );

    /*
    <license>
     <responsible_individual_name>AUTHORIZED USER</responsible_individual_name>
     <company>
      <name>HOBART</name>
      <address>TROY, OHIO</address>
     </company>
     <product name="SCALE_DIGIMARC" />
     <product name="SCALE_OFFICE_VIEWER" />
     <product name="SCALE_PDF_VIEWER" />
     <type>USB_SERIAL_NUMBER</type>
     <activation_date>2020-05-01</activation_date>
     <allotment>8</allotment>
     <timeout>0</timeout>
     <key>D445FAC0ED0BC7B5D9D1EB9FA99E5A16607713E1</key>
    </license>
    */

    do
    {
        xml.readNext();

        if (( QXmlStreamReader::EndElement == xml.tokenType() ) && ( "license" == xml.name() ))
            break;
        else if ( QXmlStreamReader::StartElement == xml.tokenType() )
        {
            if ( "responsible_individual_name" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    result.append( xml.text().toString() + "\n" );
            }
            else if ( "company" == xml.name() )
                result.append( parseLicenseCompany( xml ) );
            else if ( "product" == xml.name() )
            {
                QXmlStreamAttributes attributes( xml.attributes() );

                if ( attributes.hasAttribute( "name" ) )
                {
                    if ( !products )
                    {
                        result.append( tr( "Products:" ) + "\n" );
                        products = true;
                    }

                    result.append( attributes.value( "name" ).toString() + "\n" );
                }
            }
            else if ( "type" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    result.append( tr( "Type:" ) + " " + xml.text().toString() + "\n" );
            }
            else if ( "allotment" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    result.append( tr( "Allotment:" ) + " " + xml.text().toString() + "\n" );
            }
            else if ( "key" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    result.append( tr( "Key:" ) + " " + xml.text().toString() + "\n" );
            }
        }

    } while (( !xml.atEnd() ) && ( !xml.hasError() ));

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleFirmware::parseLicenseCompany( QXmlStreamReader& xml )
{
    QString result;

    do
    {
        xml.readNext();

        if (( QXmlStreamReader::EndElement == xml.tokenType() ) && ( "company" == xml.name() ))
            break;
        else if ( QXmlStreamReader::StartElement == xml.tokenType() )
        {
            if ( "name" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    result.append( xml.text().toString() + "\n" );
            }
            else if ( "address" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    result.append( xml.text().toString() + "\n" );
            }
        }

    } while (( !xml.atEnd() ) && ( !xml.hasError() ));

    return result;
}
