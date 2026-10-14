/**
 * @file bluezviewer.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "inputwidgethook.h"
#include "mainwindow.h"
#include "touchscreenstyle.h"
#include "webviewnetwork.h"

#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include <QMessageBox>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QtSql>

#ifdef Q_OS_WIN
#include <direct.h>
#else
#include <unistd.h>
#endif

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#endif

class NetworkAccessManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkAccessManager( QObject *parent = 0 );

    virtual ~NetworkAccessManager();

public Q_SLOTS:
    void replyFinished( QNetworkReply *resp );
};

#include "main.moc"

///////////////////////////////////////////////////////////////////////////////////////////////////
NetworkAccessManager::NetworkAccessManager( QObject *parent )
{
    Q_UNUSED( parent );

    // nothing to do for now
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NetworkAccessManager::~NetworkAccessManager()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkAccessManager::replyFinished( QNetworkReply *resp )
{

    QVariant status_code(resp->attribute(QNetworkRequest::HttpStatusCodeAttribute));
    
    if ( !(status_code.isValid()) )
    {
        qDebug() << "replyFinished Slot - Unable to make http request from given URL: " << status_code;
        QMessageBox msgBox;
        msgBox.setText( "Unable to make http request from given URL" );
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }
    else 
    {
        QString status = status_code.toString();
        if ( status == "200")
            qDebug() << "replyFinished Slot - http request successful: " << status;
        else if ( status == "301" )
            qDebug() << "replyFinished Slot - http request response: " << status << "- redirected";
        else
        {
            qDebug() << "replyFinished Slot - http request returned with error code: " << status;
            QMessageBox msgBox;
            QString text = QString("http request returned with error code: %1").arg(status);
            msgBox.setText( text );
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void loadParameters( const QString &configFile, QMap<QString, QString> &params )
{
    QFile f( configFile );

    // load settings
    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
        qWarning() << "error loading config: " << configFile;
    else
    {
        QTextStream is( &f );

        while ( !is.atEnd() )
        {
            QString line( is.readLine() );

            if (( 2 <= line.length() ) && ( ' ' != line[0] ) && ( '#' != line[0] ))
            {
                int index( line.indexOf( "=" ) );

                if ( 0 < index )
                    params[line.left(index)] = line.mid( index + 1 );
            }
        }

        f.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString getDBPath(void)
{
    QString db_path = "";

#ifdef Q_OS_LINUX
    std::ifstream infile("/etc/hobart/defaults_linux.ini");
#else
    std::ifstream infile("/Program Files/hobart/db/scale/dbfiles/defaults.ini");
#endif

    for (std::string line; getline(infile, line);)
    {
        std::size_t found = line.find("db.SQLite.path=");
        
        if (found != std::string::npos)
        {
            line.erase(found, 15);
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            db_path = line.c_str();
            
            qDebug() << "Found DB path:" << db_path;
            
            break;
        }
    }
    
    return db_path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    a.setStyle( new touchScreenStyle() );

    // set current working dir to application working dir
    std::string pwd( argv[0] );

    size_t len( 0 );
    size_t idx_bs( pwd.find_last_of( '\\' ) );
    size_t idx_fs( pwd.find_last_of( '/' ) );

    if ( std::string::npos != idx_bs )
        if ( len < idx_bs )
            len = idx_bs;

    if ( std::string::npos != idx_fs )
        if ( len < idx_fs )
            len = idx_fs;

    if ( len )
    {
        std::string new_pwd( pwd.substr( 0, len ) );
        chdir( new_pwd.c_str() );
    }

    // set default params
    QMap<QString, QString> params;
    params["url"] = "";
    params["configfilename"] = argv[0];
    params["allowrefresh"] = "0";
    params["allowcookies"] = "0";
    params["allowkeyboard"] = "0";
    params["allowclose"] = "1";
    params["allowminimize"] = "0";
    params["allowconfig"] = "0";
    params["sendstorenumber"] = "0";
    params["senddeptnumber"] = "0";
    params["server"] = "127.0.0.1";
    params["port"] = "6000";
    params["appname"] = "OO Settings";
    params["style"] = "0";
    params["logo"] = "settings.png";
    params["title"] = "";
    params["background"] = "#000000";
    params["fontcolor"] = "#FFFFFF";
    params["fontpointsize"] = "18";
    params["fontfamily"] = "Arial";

    // load params
    QString configFile( argv[0] + QString( ".config" ) );
    configFile.replace( ".exe.config", ".config" );

    loadParameters( configFile, params );
    
    // read some configs
    QSqlDatabase scale_db = QSqlDatabase::addDatabase("QSQLITE");
    scale_db.setDatabaseName(getDBPath());
    
    if (!scale_db.open()) qWarning() << "Failed to open DB!";
    
    params["onlineOrderingServer"] = mainWindow::loadConfig("onlineOrderingServer");
    params["server"] = params["onlineOrderingServer"];
    
    if (params["onlineOrderingServer"] != "")
    {
        params["url"] = "http://" + params["onlineOrderingServer"] + "/OnlineOrdering/default/settings.html";
    }

    // checking http request for failures
    NetworkAccessManager *myManager = new NetworkAccessManager();
    QNetworkAccessManager *manager = new QNetworkAccessManager(myManager);
    
    manager->get(QNetworkRequest(QUrl(params["url"])));
    QObject::connect(manager, &QNetworkAccessManager::finished, myManager, &NetworkAccessManager::replyFinished);
 

    // log out params
    for ( QMap<QString, QString>::iterator p( params.begin() ); p != params.end(); ++p )
        qDebug() << p.key() << ": " << p.value();

    // virtual keyboard
    inputWidgetHook virtualInputDevice;

    // launch!
    static const QString windowTitle( params["appname"] );

    mainWindow w( params );
    w.setWindowTitle( windowTitle );
	
#ifdef Q_OS_LINUX
    Display *disp( XOpenDisplay( NULL ) );

    if ( disp )
    {
        XStoreName( disp, w.winId(), qPrintable( windowTitle ) );
        XCloseDisplay( disp );
    }
#endif

#ifdef Q_OS_LINUX
    w.showFullScreen();
#else
    w.resize(QSize(1024,600));
    w.show();
#endif

    if ( "0" == params["allowkeyboard"] )
        virtualInputDevice.setDisabled( true );
    else
    {
        QApplication::connect( &w, SIGNAL(hideInputDevice()), &virtualInputDevice, SLOT(dismiss()) );
        QApplication::connect( &w, SIGNAL(showInputDevice(QWidget*,bool)), &virtualInputDevice, SLOT(invoke(QWidget*,bool)) );
    }

    return a.exec();
}
