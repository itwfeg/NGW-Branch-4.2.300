/**
 * @file licensingdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "licensingdialog.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <filesystem.h>
#include <font.h>
#include <logmanager.h>
#include <palette.h>
#include <python.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QDir>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QXmlStreamReader>

///////////////////////////////////////////////////////////////////////////////////////////////////
LicensingDialog::LicensingDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "LicensingDialog" );

    // initialize interface
    initialize();

    // FT-415
    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );

    // read licenses
    readHobartLicenses();
    readThirdPartyLicenses();

    if ( hobartLicenses_.isEmpty() )
        hobartLicenses_.append( tr( "No optional licensing installed." ) + "\n" );

    // update dialog
    text_->appendPlainText( hobartLicenses_ );
    text_->appendPlainText( thirdPartyLicenses_ );

    // scroll to top
    text_->moveCursor( QTextCursor::Start );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
LicensingDialog::~LicensingDialog()
{
    // FT-415
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LicensingDialog::initialize()
{
    // create widgets
    text_ = new QPlainTextEdit( this );
    text_->setObjectName( "text" );
    text_->setReadOnly( true );
    text_->setFocusPolicy( Qt::NoFocus );
    text_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // create layouts
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->addWidget( text_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LicensingDialog::readHobartLicenses()
{
    LOG_CLASS_INFO( "LicensingDialog", "readHobartLicenses" );

#if defined( Q_OS_WIN )

    QDir tempFolder( QDir::temp() );
    QString license( tempFolder.absoluteFilePath( "license.txt" ) );

    // retrieve keys
    python p;

    if ( !p.execCommand( "scaleKeys" ) )
        LOG_WARN( "failed to retrieve licensing", NULL );
    else if ( 0 != p.getExitCode() )
        LOG_WARN( "error retrieve licensing", NULL );
    else
    {
        QByteArray a( p.readStdOut().toLatin1() );

        if ( a.size() <= 0 )
            LOG_WARN( "no licensing data present", NULL );
        else
        {
            QFile out( license );

            if ( !out.open( QIODevice::ReadWrite | QIODevice::Text ) )
                LOG_WARN( "error opening license file '%s'", qPrintable( license ) );
            else
            {

                // write data
                out.write( a );
                out.close();
            }
        }
    }

#elif defined( Q_OS_LINUX )

    QString license( "/dev/mtd6" );

#endif

    // check for file
    if ( !QFile::exists( license ) )
    {
        LOG_WARN( "license file '%s' does not exist", qPrintable( license ) );
        return;
    }

    QFile f( license );

    // open file
    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        LOG_WARN( "error opening license file '%s'", qPrintable( license ) );
        return;
    }

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

    // parse data
    QXmlStreamReader xml( a );

    // <hobart>
    //   <license>
    //     <responsible_individual_name>Scott Davis</responsible_individual_name>
    //     <company>
    //       <name>ITW</name>
    //       <address>401 Market St</address>
    //     </company>
    //     <product name="SCALE_OFFICE_VIEWER" />
    //     <product name="SCALE_PDF_VIEWER" />
    //     <type>USB_SERIAL_NUMBER</type>
    //     <allotment>5</allotment>
    //     <key>9302323003303030023</key>
    //   </license>
    // </hobart>

    // <hobart>
    //   <license>
    //     <responsible_individual_name>Aaron Swift</responsible_individual_name>
    //     <company>
    //       <name>Walmart</name>
    //       <address>Troy OH</address>
    //     </company>
    //     <product name="SCALE_OFFICE_VIEWER" />
    //     <type>SERVER_IP_ADDRESS</type>
    //     <allotment>16</allotment>
    //     <key>8302323003303030023</key>
    //   </license>
    // </hobart>

    while (( !xml.atEnd() ) && ( !xml.hasError() ))
    {
        // read next element
        QXmlStreamReader::TokenType token( xml.readNext() );

        // if token is just StartDocument, we'll go to next...
        if ( QXmlStreamReader::StartDocument == token )
            continue;

        // if token is StartElement, we'll see if we can read it
        else if ( QXmlStreamReader::StartElement == token )
        {
            if ( "license" == xml.name() )
            {
                hobartLicenses_.append( tr( "This product is licensed to:" ) + "\n" );
                hobartLicenses_.append( "\n" );
                parseLicense( xml );
                hobartLicenses_.append( "\n" );
            }
        }
    }

    if ( xml.hasError() )
        LOG_WARN( "error parsing license file '%s'", qPrintable( license ) );

    xml.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LicensingDialog::readThirdPartyLicenses()
{
    LOG_CLASS_INFO( "LicensingDialog", "readThirdPartyLicenses" );

    QString license( filesystem::absoluteFilePath( "3rdparty.txt", folderType::Licenses ) ); // TODO

    // check for file
    if ( !QFile::exists( license ) )
    {
        LOG_WARN( "license file '%s' does not exist", qPrintable( license ) );
        return;
    }

    QFile f( license );

    // open file
    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        LOG_WARN( "error opening license file '%s'", qPrintable( license ) );
        return;
    }

    // read file
    QByteArray a( f.readAll() );
    f.close();

    // append to string
    thirdPartyLicenses_.append( a );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LicensingDialog::parseLicense( QXmlStreamReader& xml )
{
    static const QString element( "license" );

    bool products( false );

    // sanity check
    if (( QXmlStreamReader::StartElement != xml.tokenType() ) && ( element != xml.name() ))
        return;

    // next element
    xml.readNext();

    // read elements loop
    while (( !xml.atEnd() ) && ( !xml.hasError() ) &&
           ( !(( QXmlStreamReader::EndElement == xml.tokenType() ) && ( element == xml.name() )) ))
    {
        if ( QXmlStreamReader::StartElement == xml.tokenType() )
        {
            if ( "responsible_individual_name" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    hobartLicenses_.append( xml.text().toString() + "\n" );
            }
            else if ( "company" == xml.name() )
                parseCompany( xml );
            else if ( "product" == xml.name() )
            {
                QXmlStreamAttributes attributes( xml.attributes() );

                if ( attributes.hasAttribute( "name" ) )
                {
                    if ( !products )
                    {
                        hobartLicenses_.append( tr( "Products:" ) + "\n" );
                        products = true;
                    }

                    hobartLicenses_.append( attributes.value( "name" ).toString() + "\n" );
                }
            }
            else if ( "type" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    hobartLicenses_.append( tr( "Type:" ) + " " + xml.text().toString() + "\n" );
            }
            else if ( "allotment" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    hobartLicenses_.append( tr( "Allotment:" ) + " " + xml.text().toString() + "\n" );
            }
            else if ( "key" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    hobartLicenses_.append( tr( "Key:" ) + " " + xml.text().toString() + "\n" );
            }
        }

        // next element
        xml.readNext();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void LicensingDialog::parseCompany( QXmlStreamReader& xml )
{
    static const QString element( "company" );

    // sanity check
    if (( QXmlStreamReader::StartElement != xml.tokenType() ) && ( element != xml.name() ))
        return;

    // next element
    xml.readNext();

    // read elements loop
    while (( !xml.atEnd() ) && ( !xml.hasError() ) &&
           ( !(( QXmlStreamReader::EndElement == xml.tokenType() ) && ( element == xml.name() )) ))
    {
        if ( QXmlStreamReader::StartElement == xml.tokenType() )
        {
            if ( "name" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    hobartLicenses_.append( xml.text().toString() + "\n" );
            }
            else if ( "address" == xml.name() )
            {
                xml.readNext();

                if ( QXmlStreamReader::Characters == xml.tokenType() )
                    hobartLicenses_.append( xml.text().toString() + "\n" );
            }
        }

        // next element
        xml.readNext();
    }
}
