/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "mainwindow.h"
#include "webview.h"

#include <QApplication>
#include <QByteArray>
#include <QColor>
#include <QPalette>
#include <QDir>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QLabel>
#include <QTcpSocket>
#include <QToolButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QWindow>
#include <QWidget>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QPen>
#include <QProcess>
#include <QPainterPath>
#include <QDateTimeEdit>
#include <QStringList>

#include <regex>
#include <fstream>
#include <sstream>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( const QMap<QString, QString> &params, QWidget *parent ) :
    _Mybase( parent ),
    params_( params ),
    style_( StyleNormal ),
    store_( 0 ),
    dept_( 0 ),
    address_( "" )
{
    QString url( params_["url"] );

    // check style
    if ( "1" == params_["style"] )
        style_ = StyleCompact;
    else if ( "2" == params_["style"] )
        style_ = StyleFloatingLeft;
    else if ( "3" == params_["style"] )
        style_ = StyleFloatingCenter;
    else if ( "4" == params_["style"] )
        style_ = StyleFloatingRight;
    else if ( "5" == params_["style"] )
        style_ = StyleFloatingOffsetLeft;
    else if ( "6" == params_["style"] )
        style_ = StyleFloatingOffsetRight;

    // init
    initialize();
    createLayout();
    translate();

    // retrieve store settings
    if (( "1" == params_["sendstorenumber"] ) || ( "1" == params_["senddeptnumber"] ))
        if ( requestStoreSettings() )
        {
            if (( "1" == params_["sendstorenumber"] ) && ( 0 < store_ ))
            {
                url.append( url.contains( "?" ) ? "&" : "?" );
                url.append( "store=" );
                url.append( QString::number( store_ ) );
            }

            if (( "1" == params_["senddeptnumber"] ) && ( 0 < dept_ ))
            {
                url.append( url.contains( "?" ) ? "&" : "?" );
                url.append( "dept=" );
                url.append( QString::number( dept_ ) );
            }
        }

    // browse to page
    webView_->setUrl( QUrl( url ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::translate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::resizeEvent( QResizeEvent *e )
{
    if (( isFloatingButtons() ) && ( buttonBar_ ))
    {
        const QColor c( params_["background"] );

        // update button bar size and placement
        buttonBar_->setFixedSize( QSize( width() / 5, 40 ) );

        if ( StyleFloatingLeft == style_ )
            buttonBar_->move( 0, 0 );
        else if ( StyleFloatingCenter == style_ )
            buttonBar_->move( (width() - buttonBar_->width()) / 2, 0 );
        else if ( StyleFloatingRight == style_ )
            buttonBar_->move( width() - buttonBar_->width(), 0 );
        else if ( StyleFloatingOffsetLeft == style_ )
            buttonBar_->move( (width() / 4) - (buttonBar_->width() / 2), 0 );
        else if ( StyleFloatingOffsetRight == style_ )
            buttonBar_->move( ((3 * width()) / 4) - (buttonBar_->width() / 2), 0 );

        buttonBar_->raise();

        // generate 'hobart' look for floating bar
        QPixmap pixmap( buttonBar_->size() );
        pixmap.fill( Qt::transparent );

        QPainter painter( &pixmap );
        painter.setBrush( QBrush( c, Qt::SolidPattern ) );
        painter.setPen( c );
        painter.setRenderHint( QPainter::Antialiasing, true );

        QPainterPath path( QPoint( 0, 0 ) );

        if ( StyleFloatingLeft == style_ )
            path.lineTo( 0, pixmap.height() );
        else
            path.lineTo( (pixmap.height() * 40) / 100, pixmap.height() );

        if ( StyleFloatingRight == style_ )
            path.lineTo( pixmap.width(), pixmap.height() );
        else
            path.lineTo( pixmap.width() - ((pixmap.height() * 40) / 100), pixmap.height() );

        path.lineTo( pixmap.width(), 0 );
        path.closeSubpath();

        painter.drawPath( path );

        // set floating bar graphic
        QPalette p( buttonBar_->palette() );
        p.setBrush( buttonBar_->backgroundRole(), pixmap );
        buttonBar_->setPalette( p );
    }

    _Mybase::resizeEvent( e );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool mainWindow::isDateOrderCorrect()
{
    QDateTime startDT = startDTE_->dateTime();
    startDT.setTime(QTime(0,0,0));

    QDateTime endDT = endDTE_->dateTime();
    endDT.setTime(QTime(23,59,59));

    if ( endDT < startDT )
    {
        QMessageBox msgBox;
        msgBox.setText( "Start date/time must be before End date/time" );
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onDateTimeChanged(const QDateTime &dt)
{
    isDateOrderCorrect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onButtonClicked()
{
    if ( toggleKeyboard_ == sender() )
        if ( stack_->currentWidget() == webView_ )
            Q_EMIT showInputDevice( webView_, true );
        else
            Q_EMIT showInputDevice( secondPageWidget_, true );
    else if ( refresh_ == sender() )
    {
        webView_->reload();
    }
    else if ( minimize_ == sender() )
        //setWindowState( Qt::WindowMinimized );
        close();
    else if ( exit_ == sender() )
        close();
    else if ( toggleConfig_ == sender() )
    {
        requestZones();

#ifdef Q_OS_LINUX
        if( stack_->currentWidget() == webView_ )
        {
            stack_->setCurrentWidget( secondPageWidget_ );
            toggleConfig_->setIcon( QPixmap( "res/backIcon.png" ) );
        }
        else
        {
            stack_->setCurrentWidget( webView_ );
            toggleConfig_->setIcon( QPixmap( "res/settingsIcon.png" ) );
        }
#else
        stack_->setCurrentWidget( secondPageWidget_ );
        toggleConfig_->setIcon( QPixmap( "res/backIcon.png" ) );
#endif
    }
    else if ( backup_ == sender() )
    {
        if( address_ != "" )
        {
            QString dir = QFileDialog::getExistingDirectory( this, tr("Open"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
            //TODO: find out how to create non-native dialog, so keyboard accept input

            qDebug() << "backup dir path: " << dir;

            if ( dir != "" )
            {
                QString command("python3");
                QStringList params = { "oo_settings_backup_script.py", address_, dir };

                QProcess *process = new QProcess();
                process->setWorkingDirectory( "/usr/local/hobart/features/oo_settings/" );
                process->start(command, params);
                
                qDebug() << "Backup began";

                QMessageBox msgBox;
                msgBox.setText( "Backup in progress..." );
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();  
                
                process->waitForFinished();

                QByteArray result = process->readAllStandardOutput();
                process->close(); 

                QString rlt = QString(result);

                qDebug() << "backup script return result:" << rlt;

                if ( rlt == "success\n" )
                {
                    qDebug() << "Backup completed successfully!";

                    QMessageBox msgBox;
                    msgBox.setText( "Backup completed successfully!" );
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.exec();  
                }
                else if ( rlt == "failure\n" )
                {
                    qDebug() << "Error while performing backup. Check the given server address";

                    QMessageBox msgBox;
                    msgBox.setText( "Error while performing backup. Check the given server address" );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();  
                }
                else if ( rlt == "timeout failure\n" )
                {
                    qDebug() << "Backup took too long.";

                    QMessageBox msgBox;
                    msgBox.setText( "Timeout Error: Backup took too long to complete." );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();       
                }
            }
            else
            {
                qDebug() << "Cannot Backup. No Target Directory Selected.";

                QMessageBox msgBox;
                msgBox.setText( "Cannot Backup. No Target Directory Selected." );
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec(); 
            }
        }
        else
        {
            qDebug() << "Please submit a server address before attempting to backup settings.";

            QMessageBox msgBox;
            msgBox.setText( "Please submit a server address before attempting to backup settings." );
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
    }
    else if ( restore_ == sender() )
    {   
        if( address_ != "" )
        {
            QString file = QFileDialog::getOpenFileName( this, tr("Choose File"), "/", tr("Files (*.json)"));
            //TODO: find out how to create non-native dialog, so that keyboard accepts input

            qDebug() << "restore file path: " << file;

            if ( file != "" )
            {
                QString command("python3");
                QStringList params = { "oo_settings_restore_script.py", address_, file };

                QProcess *process = new QProcess();
                process->setWorkingDirectory( "/usr/local/hobart/features/oo_settings/" );
                process->start(command, params);
                
                qDebug() << "Restoration began";

                QMessageBox msgBox;
                msgBox.setText( "Restore in progress..." );
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();  
                
                process->waitForFinished();

                QByteArray result = process->readAllStandardOutput();
                process->close(); 

                QString rlt = QString(result);

                qDebug() << "restore script return result:" << rlt;

                if ( rlt == "success\n" )
                {
                    qDebug() << "Restoration completed successfully!";

                    QMessageBox msgBox;
                    msgBox.setText( "Restoration completed successfully!" );
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.exec();  
                }
                else if ( rlt == "failure\n" )
                {
                    qDebug() << "Error while performing restore. Check the given server address";

                    QMessageBox msgBox;
                    msgBox.setText( "Error while performing restore. Check the given server address" );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();  
                }
                else if ( rlt == "timeout failure\n" )
                {
                    qDebug() << "Restoration took too long.";

                    QMessageBox msgBox;
                    msgBox.setText( "Timeout Error: Restoration took too long to complete." );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();       
                }
            }
            else
            {
                qDebug() << "Cannot Restore. No Backup File Selected.";

                QMessageBox msgBox;
                msgBox.setText( "Cannot Restore. No Backup File Selected." );
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec(); 
            }
        }
        else
        {
            qDebug() << "Please submit a server address before attempting to restore settings.";

            QMessageBox msgBox;
            msgBox.setText( "Please submit a server address before attempting to restore settings." );
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
    }
    else if ( export_ == sender() )
    {
        if( address_ != "" )
        {
            QString dir = QFileDialog::getExistingDirectory( this, tr("Open"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
            //TODO: find out how to create non-native dialog, so keyboard accepts input

            qDebug() << "dir path: " << dir;

            if ( dir != "" )
            {
                QString command("python3");
                QStringList params = { "oo_export_script.py", address_, dir };

                QProcess *process = new QProcess();
                process->setWorkingDirectory( "/usr/local/hobart/features/oo_settings/" );
                process->start(command, params);
                
                qDebug() << "Export began";

                QMessageBox msgBox;
                msgBox.setText( "Exporting..." );
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();  
                
                process->waitForFinished();

                QByteArray result = process->readAllStandardOutput();
                process->close(); 

                QString rlt = QString(result);

                qDebug() << "export return result:" << rlt;

                if ( rlt == "success\n" )
                {
                    qDebug() << "Export completed successfully!";

                    QMessageBox msgBox;
                    msgBox.setText( "Export completed successfully!" );
                    msgBox.setIcon(QMessageBox::Information);
                    msgBox.exec();  
                }
                else if ( rlt == "failure\n" )
                {
                    qDebug() << "Error while exporting. Check the given server address";

                    QMessageBox msgBox;
                    msgBox.setText( "Error while exporting. Check the given server address" );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();  
                }
                else if ( rlt == "timeout failure\n" )
                {
                    qDebug() << "Exporting took too long.";

                    QMessageBox msgBox;
                    msgBox.setText( "Timeout Error: Exporting took too long to complete." );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();       
                }
            }
            else
            {
                qDebug() << "Cannot Export. No Target Directory Selected.";

                QMessageBox msgBox;
                msgBox.setText( "Cannot Export. No Target Directory Selected." );
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec(); 
            }
        }
        else
        {
            qDebug() << "Please submit a server address before attempting to export data.";

            QMessageBox msgBox;
            msgBox.setText( "Please submit a server address before attempting to export data." );
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
    }
    else if ( statistics_ == sender() )
    {
        if ( isDateOrderCorrect() )
        {
            if ( address_ != "" )
            {
                QString dir = QFileDialog::getExistingDirectory( this, tr("Open"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );

                qDebug() << "dir path: " << dir;

                if ( dir != "" )
                {
                    QString command("python3");

                    QDateTime startDT = startDTE_->dateTime();
                    startDT.setTime(QTime(0,0,0));

                    QDateTime endDT = endDTE_->dateTime();
                    endDT.setTime(QTime(23,59,59));


                    QString zz = startDT.toString();
                    zz = startDTE_->dateTime().toString("yyyy-MM-dd HH:mm:ss");
                    QStringList params = { "oo_export_statistics_script.py", address_, dir, reportSelector_->currentText(), startDT.toString("yyyy-MM-dd HH:mm:ss"), endDT.toString("yyyy-MM-dd HH:mm:ss") };

                    QProcess *process = new QProcess();
#ifdef Q_OS_LINUX
                    process->setWorkingDirectory( "/usr/local/hobart/apps/oo_settings/" );
#else
                    process->setWorkingDirectory( "/Projects/Project_Fastrack/svn_trunk/Windux/3RDPartySoftware/oo_settings/oo_settings_webapp/scripts/" );
#endif
                    process->start(command, params);

                    qDebug() << "Export statistics began with params: " << params;

                    //QMessageBox msgBox;
                    //msgBox.setText( "Exporting statistics..." );
                    //msgBox.setIcon(QMessageBox::Information);
                    //msgBox.exec();

                    process->waitForFinished();

                    QByteArray result = process->readAllStandardOutput();
                    process->close();

                    QString rlt = QString(result);

                    qDebug() << "export statistics return result:" << rlt;

                    if ( rlt == "success\n" )
                    {
                        qDebug() << "Export of statistics completed successfully!";

                        QMessageBox msgBox;
                        msgBox.setText( "Exporting Statistics completed successfully!" );
                        msgBox.setIcon(QMessageBox::Information);
                        msgBox.exec();
                    }
                    else if ( rlt == "failure\n" )
                    {
                        qDebug() << "Error while exporting statistics. Check the given server address";

                        QMessageBox msgBox;
                        msgBox.setText( "Error while exporting statistics. Check the given server address" );
                        msgBox.setIcon(QMessageBox::Critical);
                        msgBox.exec();
                    }
                    else if ( rlt == "timeout failure\n" )
                    {
                        qDebug() << "Exporting statistics took too long.";

                        QMessageBox msgBox;
                        msgBox.setText( "Timeout Error: Exporting Statistics took too long to complete." );
                        msgBox.setIcon(QMessageBox::Critical);
                        msgBox.exec();
                    }
                    else
                    {
                        qDebug() << "Exporting statistics something happened.";

                        QString msg("Exporting Statistics: Something happened but I don't know what" );
                        msg+= "\n\nReturn status: " + rlt;
                        QMessageBox msgBox;
                        msgBox.setText( msg );
                        msgBox.setIcon(QMessageBox::Critical);
                        msgBox.exec();
                    }

                }
                else
                {
                    qDebug() << "Cannot Export Statistics. No Target Directory Selected.";

                    QMessageBox msgBox;
                    msgBox.setText( "Cannot Export Statistics. No Target Directory Selected." );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();
                }
            }
            else
            {
                qDebug() << "Please submit a server address before attempting to export statistics.";

                QMessageBox msgBox;
                msgBox.setText( "Please submit a server address before attempting to export data." );
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();
            }
        }
    }
    else if ( submitZone_ == sender())
    {
      // TODO: validation/safety
      writeTag("oz", QString::number(scaleZone_->currentData().toInt()));

      QMessageBox msgBox;
      msgBox.setText( "Scale zone received." );
      //msgBox.setText(QString::number(scaleZone_->currentData().toUInt()));
      msgBox.setIcon(QMessageBox::Information);
      msgBox.exec();
    }
    else if ( submit_ == sender() )
    {   
        if( serverIp_->text() != "" )
        {
            std::string ip = serverIp_->text().toStdString();
            std::smatch match;
            const std::regex reg("^[[:digit:]]{1,3}[.][[:digit:]]{1,3}[.][[:digit:]]{1,3}[.][[:digit:]]{1,3}$");

            if ( std::regex_search( ip, match, reg) )
            {
                address_ = QString::fromStdString(ip);
                qDebug() << "Server IP: " << address_;

                webView_->setUrl( QUrl( "http://" + address_ + "/OnlineOrdering/default/settings.html" ) );

                QSqlError err = saveConfig("onlineOrderingServer", address_);
                if (err.type() != QSqlError::NoError)
                {
                    QString msg ("Could not save server address:\n" + err.text());
                    qDebug() << msg.data_ptr();
                    QMessageBox msgBox;
                    msgBox.setText( "Invalid server address format." );
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.exec();
                }

                txt->setText("Server Address: " + address_);
                
                // save config into scale configs
                writeTag("oS",address_);

                // fetch zones from the new server address
                requestZones();

                QMessageBox msgBox;
                msgBox.setText( "Server address received." );
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();
            }
            else
            {
                qDebug() << "Invalid server address format";
                QMessageBox msgBox;
                msgBox.setText( "Invalid server address format." );
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.exec();
            }
            serverIp_->clear();
        }
    }
    else if ( upload_ == sender() )
    {
      if( address_ != "" )
      {
        QString command("python3");
        QStringList params = { "blackboxSync.py", "--operators", "--products", "--departments", "--address", address_, "push", "merge"};

        QProcess *process = new QProcess();
        process->setWorkingDirectory( "/usr/local/hobart/python/" );
        process->start(command, params);
            
        qDebug() << "Upload began";

        QMessageBox msgBox;
        msgBox.setText( "Uploading..." );
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();  
        
        // TODO: some kind of progress bar instead of waiting 20 minutes
        process->waitForFinished(1200000);

        QByteArray result = process->readAllStandardOutput();
        result += process->readAllStandardError();
        process->close();

        qDebug() << "Upload script output:" << QString(result);

        if (process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0)
        {
          qDebug() << "Upload OK";
          QMessageBox msgBox;
          msgBox.setText( "Upload completed successfully!" );
          msgBox.setIcon(QMessageBox::Information);
          msgBox.exec();
        }
        else
        {
          qDebug() << "Upload failed";
          QMessageBox msgBox;
          msgBox.setText( "Upload failed!" );
          msgBox.setIcon(QMessageBox::Critical);
          msgBox.exec();
        }
      }
    }
    else if ( download_ == sender() )
    {
      if( address_ != "" )
      {
        QString command("python3");
        QStringList params = { "blackboxSync.py", "--operators", "--products", "--departments", "--address", address_, "pull", "merge"};

        QProcess *process = new QProcess();
        process->setWorkingDirectory( "/usr/local/hobart/python/" );
        process->start(command, params);
            
        qDebug() << "Download began";

        QMessageBox msgBox;
        msgBox.setText( "Downloading..." );
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();  
        
        // TODO: some kind of progress bar instead of waiting 20 minutes
        process->waitForFinished(1200000);

        QByteArray result = process->readAllStandardOutput();
        result += process->readAllStandardError();
        process->close();

        qDebug() << "Download script output:" << QString(result);

        if (process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0)
        {
          qDebug() << "Download OK";
          QMessageBox msgBox;
          msgBox.setText( "Download completed successfully!" );
          msgBox.setIcon(QMessageBox::Information);
          msgBox.exec();
        }
        else
        {
          qDebug() << "Download failed";
          QMessageBox msgBox;
          msgBox.setText( "Download failed!" );
          msgBox.setIcon(QMessageBox::Critical);
          msgBox.exec();
        }
      }
    }
    else if ( connectionLogs_ == sender() )
    {

        QString dir = QFileDialog::getExistingDirectory( this, tr("Open"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );
        //TODO: find out how to create non-native dialog, so keyboard accepts input

        qDebug() << "copying connection log to" << dir;

        if ( dir != "" )
        {
            QString command("cp");
            QStringList params = { "/usr/local/hobart/oo_connectivity.log", dir };

            QProcess *process = new QProcess();
            //process->setWorkingDirectory( "/usr/local/hobart/features/oo_settings/" );
            process->start(command, params);

            process->waitForFinished();

            QByteArray result = process->readAllStandardOutput();
            process->close();

            qDebug() << "copy done";
            QMessageBox msgBox;
            msgBox.setText( "Logs exported successfully!" );
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
        else
        {
            qDebug() << "no export directory selected";

            QMessageBox msgBox;
            msgBox.setText( "Cannot Export. No Target Directory Selected." );
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::initialize()
{
    // setup font
    QFont f( font() );
    f.setPointSize( params_["fontpointsize"].toInt() );
    f.setFamily( params_["fontfamily"] );
    setFont( f );
    QApplication::setFont(f);

    // setup palette
    QPalette p( palette() );
    p.setColor( backgroundRole(), QColor( params_["background"] ) );
    p.setColor( QPalette::ButtonText, QColor( params_["fontcolor"] ) );
    p.setColor( QPalette::Text, QColor( params_["fontcolor"] ) );
    p.setColor( QPalette::WindowText, QColor( params_["fontcolor"] ) );
    setPalette( p );

    // ---- //

    const QPixmap logo( params_["logo"] );

    centralWidget_ = new QWidget( this );
    centralWidget_->setObjectName( "centralWidget" );

    stack_ = new QStackedWidget( centralWidget_ );

    secondPageWidget_ = new QWidget();
    secondPageWidget_->setObjectName( "secondPageWidget" );
    secondPageWidget_->setStyleSheet( "background-color:rgb(240,240,240);" );

    backup_ = new QPushButton("Backup", secondPageWidget_);
    backup_->setStyleSheet( "background-color:red;" );
    connect( backup_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    restore_ = new QPushButton("Restore", secondPageWidget_);
    restore_->setStyleSheet( "background-color:red;" );
    connect( restore_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    export_ = new QPushButton("Export", secondPageWidget_);
    export_->setStyleSheet( "background-color:blue;" );   
    connect( export_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    connectionLogs_ = new QPushButton("Export Log", secondPageWidget_);
    connectionLogs_->setStyleSheet( "background-color:blue;" );   
    connect( connectionLogs_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    statistics_ = new QPushButton("Statistics", secondPageWidget_);
    statistics_->setStyleSheet( "background-color:blue;" );
    connect( statistics_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    upload_ = new QPushButton("Upload", secondPageWidget_);
    upload_->setStyleSheet( "background-color:blue;" );   
    connect( upload_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    download_ = new QPushButton("Download", secondPageWidget_);
    download_->setStyleSheet( "background-color:blue;" );   
    connect( download_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    //horizontalLayout->addStretch(25);
    horizontalLayout->addWidget(upload_);
    horizontalLayout->addWidget(download_);
    horizontalLayout->addWidget(backup_);
    horizontalLayout->addWidget(restore_);
    //horizontalLayout->addStretch(25);
    //horizontalLayout->addWidget(statistics_);
    horizontalLayout->addWidget(export_);
    horizontalLayout->addSpacing(10);

    txt = new QLabel( secondPageWidget_ );
    address_ = loadConfig("onlineOrderingServer");
    txt->setText("Server Address: " + address_);

    serverIp_ = new QLineEdit(secondPageWidget_);
    serverIp_->setStyleSheet( "background-color:white;" );
    serverIp_->setMaxLength(15);
    serverIp_->setMaximumWidth(300);
    serverIp_->setFixedWidth(300);
    serverIp_->setPlaceholderText("Enter the server address");

    submit_ = new QPushButton("Submit", secondPageWidget_);
    submit_->setStyleSheet( "background-color:green;" );
    connect( submit_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    QHBoxLayout *connectionLayout = new QHBoxLayout();
    QLabel* connectionFails = new QLabel( secondPageWidget_ );
    
    QString connectionStatus = "Gateway Connection Status: ";
    int countDown = countOfflineMinutes();
    
    if (countDown == 0)
    {
        connectionStatus += "100% acessible today";
    }
    else
    {
        connectionStatus += "inaccessible for " + QString::number(countDown) + " ";
        connectionStatus += (countDown == 1) ? "minute" : "minutes" " today";
    }
    connectionFails->setText(connectionStatus);

    connectionLayout->setSpacing(10);
    connectionLayout->addWidget(connectionFails);
    connectionLayout->addWidget(connectionLogs_);
    connectionLayout->addStretch();

    QHBoxLayout *serverLayout = new QHBoxLayout();
    //horizontalLayout->addStretch(25);
    serverLayout->setSpacing(10);
    serverLayout->addWidget(txt);
    serverLayout->addWidget(serverIp_);
    serverLayout->addWidget(submit_);
    serverLayout->addStretch();

    zoneLabel = new QLabel(secondPageWidget_);
    zoneLabel->setText("Scale Zone: ");

    scaleZone_ = new QComboBox(secondPageWidget_);
    scaleZone_->setStyleSheet( "background-color:white;" );
    scaleZone_->setMaximumWidth(250);
    scaleZone_->setFixedWidth(250);

    submitZone_ = new QPushButton("Submit", secondPageWidget_);
    submitZone_->setStyleSheet( "background-color:green;" );
    connect( submitZone_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    requestZones();

    QHBoxLayout *zonesLayout = new QHBoxLayout();
    zonesLayout->setSpacing(10);
    zonesLayout->addWidget(zoneLabel);
    zonesLayout->addWidget(scaleZone_);
    zonesLayout->addWidget(submitZone_);
    zonesLayout->addStretch();

    QGridLayout *reportGridLayout = new QGridLayout();
    QLabel *st = new QLabel("Statistics Type");
    QLabel *ds = new QLabel("Start Date");
    QLabel *de = new QLabel("End Date");
    reportGridLayout->addWidget(st,0,0);
    reportGridLayout->addWidget(ds,0,1);
    reportGridLayout->addWidget(de,0,2);

    reportSelector_ = new QComboBox(secondPageWidget_);
    QStringList reports = { "Summary", "Order Info", "Order Details", "Hourly Summary", "Daily Summary" };
    reportSelector_->insertItems(0, reports);
    reportSelector_->setFixedSize(200, 30);
    reportSelector_->setCurrentIndex(0);

    QDateTime startDT = QDateTime::currentDateTime();
    //startDT = startDT.addSecs( -( startDT.time().second() ) );
    startDT.setTime(QTime(0,0,0));

    startDTE_ = new QDateTimeEdit(secondPageWidget_);
    startDTE_->setCalendarPopup(true);
    startDTE_->setFixedSize(180, 30);
    startDTE_->setDateTime(startDT);
    startDTE_->setDisplayFormat("d MMM yyyy");
    connect( startDTE_, &QDateTimeEdit::dateTimeChanged, this, &mainWindow::onDateTimeChanged);
    //connect( startDTE_, SIGNAL(dateTimeChanged(QDateTime)), SLOT(onDateTimeChanged(QDateTime)) );

    endDTE_ = new QDateTimeEdit(secondPageWidget_);
    endDTE_->setCalendarPopup(true);
    endDTE_->setFixedSize(180, 30);
    endDTE_->setDateTime(startDT);
    endDTE_->setDisplayFormat("d MMM yyyy");
    connect( endDTE_, &QDateTimeEdit::dateTimeChanged, this, &mainWindow::onDateTimeChanged);
    //connect( endDTE_, SIGNAL(dateTimeChanged(QDateTime)), SLOT(onDateTimeChanged(QDateTime)) );

    reportGridLayout->addWidget(reportSelector_,1,0);
    reportGridLayout->addWidget(startDTE_,1,1);
    reportGridLayout->addWidget(endDTE_,1,2);
    reportGridLayout->addWidget(statistics_,1,3);

    QVBoxLayout *verticalLayout = new QVBoxLayout(secondPageWidget_);
    verticalLayout->setContentsMargins( QMargins( 6, 0, 0, 6 ) );
    verticalLayout->addSpacing(50);
    verticalLayout->addLayout(serverLayout);

    verticalLayout->addSpacing(50);
    verticalLayout->addLayout(zonesLayout);

    verticalLayout->addSpacing(50);
    verticalLayout->addLayout(reportGridLayout);
    verticalLayout->addSpacing(50);
    verticalLayout->addLayout(connectionLayout);

    verticalLayout->addStretch();

    verticalLayout->addLayout(horizontalLayout);

    secondPageWidget_->setLayout(verticalLayout);

    buttonBar_ = new QWidget( centralWidget_ );
    buttonBar_->setAutoFillBackground( true );
    buttonBar_->setObjectName( "buttonBar" );

    logo_ = new QLabel( buttonBar_ );
    logo_->setObjectName( "logo" );

    if ( logo.isNull() )
        logo_->hide();
    else
    {
        logo_->setPixmap( logo );
        logo_->setFixedSize( logo.size() );
    }

    title_ = new QLabel( buttonBar_ );
    title_->setAlignment( Qt::AlignCenter );
    title_->setObjectName( "title" );
    title_->setText( params_["title"] );

    toggleKeyboard_ = new QToolButton( buttonBar_ );
    toggleKeyboard_->setFocusPolicy( Qt::NoFocus );
    toggleKeyboard_->setIcon( QPixmap( "res/showIcon.png") );
    toggleKeyboard_->setIconSize( QSize( 40, 40 ) );
    toggleKeyboard_->setObjectName( "exit" );
    toggleKeyboard_->setStyleSheet( "background: transparent; border: none;" );
    toggleKeyboard_->setVisible( "0" != params_["allowkeyboard"] );
    connect( toggleKeyboard_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    toggleConfig_ = new QToolButton( buttonBar_ );
    toggleConfig_->setFocusPolicy( Qt::NoFocus );
    toggleConfig_->setIcon( QPixmap( "res/settingsIcon.png") );
    toggleConfig_->setIconSize( QSize( 40, 40 ) );
    toggleConfig_->setObjectName( "exit" );
    toggleConfig_->setStyleSheet( "background: transparent; border: none;" );
    toggleConfig_->setVisible( "0" != params_["allowconfig"] );
    connect( toggleConfig_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    refresh_ = new QToolButton( buttonBar_ );
    refresh_->setFocusPolicy( Qt::NoFocus );
    refresh_->setIcon( QPixmap( "res/refreshIcon.png") );
    refresh_->setIconSize( QSize( 40, 40 ) );
    refresh_->setObjectName( "exit" );
    refresh_->setStyleSheet( "background: transparent; border: none;" );
    refresh_->setVisible("0" != params_["allowrefresh"]);
    connect( refresh_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    minimize_ = new QToolButton( buttonBar_ );
    minimize_->setFocusPolicy( Qt::NoFocus );
    minimize_->setIcon( QPixmap( "res/minimizeIcon.png") );
    minimize_->setIconSize( QSize( 40, 40 ) );
    minimize_->setObjectName( "exit" );
    minimize_->setStyleSheet( "background: transparent; border: none;" );
    minimize_->setVisible( "0" != params_["allowminimize"] );
    connect( minimize_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    exit_ = new QToolButton( buttonBar_ );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setIcon( QPixmap( "res/exitIcon.png") );
    exit_->setIconSize( QSize( 40, 40 ) );
    exit_->setObjectName( "exit" );
    exit_->setStyleSheet( "background: transparent; border: none;" );
    exit_->setVisible( "0" != params_["allowclose"] );
    connect( exit_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    webView_ = new hobart::webView( ("1" == params_["allowcookies"]) );
    webView_->setObjectName( "webView" );
    webView_->setPalette( QApplication::palette() );
    connect( webView_, SIGNAL(hideInputDevice()), SIGNAL(hideInputDevice()) );
    connect( webView_, SIGNAL(showInputDevice(QWidget*)), SIGNAL(showInputDevice(QWidget*)) );

#ifdef Q_OS_LINUX
    stack_->addWidget( webView_ );
    stack_->addWidget( secondPageWidget_ );
#else
    stack_->addWidget( secondPageWidget_ );
#endif

    // ---- //
    setCentralWidget( centralWidget_ );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout( buttonBar_ ) );

    if ( StyleNormal != style_ )
    {
        buttons->setContentsMargins( QMargins() );
        buttons->setSpacing( 4 );

        if (( isFloatingButtons() ) && ( StyleFloatingLeft != style_ ))
            buttons->addSpacing( 16 );

        buttons->addWidget( logo_ );
        buttons->addWidget( refresh_ );
        buttons->addWidget( toggleKeyboard_ );
        buttons->addWidget( toggleConfig_ );
        buttons->addWidget( title_ );
        buttons->addStretch();
        buttons->addWidget( minimize_ );
        buttons->addWidget( exit_ );

        if (( isFloatingButtons() ) && ( StyleFloatingRight != style_ ))
            buttons->addSpacing( 16 );

        logo_->hide();
        title_->hide();
        toggleKeyboard_->setIconSize( QSize( 32, 32 ) );
        toggleConfig_ ->setIconSize( QSize( 32, 32 ) );
        refresh_->setIconSize( QSize( 32, 32 ) );
        minimize_->setIconSize( QSize( 32, 32 ) );
        exit_->setIconSize( QSize( 32, 32 ) );
    }
    else
    {
        buttons->setContentsMargins( QMargins( 5, 5, 5, 5 ) );
        buttons->setSpacing( 8 );

        buttons->addWidget( logo_ );
        buttons->addWidget( title_, 1 );
        buttons->addWidget( toggleKeyboard_ );
        buttons->addWidget( toggleConfig_ );
        buttons->addWidget( refresh_ );
        buttons->addWidget( minimize_ );
        buttons->addWidget( exit_ );
    }

    QVBoxLayout *layout( new QVBoxLayout( centralWidget_ ) );
    layout->setContentsMargins( QMargins() );
    layout->setSpacing( 0 );

    if ( !isFloatingButtons() )
        layout->addWidget( buttonBar_ );

    layout->addWidget( stack_, 1 );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mainWindow::requestStoreSettings()
{
    const QString server( params_["server"] );
    const quint16 port( params_["port"].toUShort() );

    if (( server.isEmpty() ) || ( port <= 0 ))
        return false;

    // setup buffers
    QByteArray buffer( "RT73\x1e" );

    // connect to server
    QTcpSocket s;
    s.connectToHost( QHostAddress( server ), port );

    if ( !s.waitForConnected( 2000 ) )
        return false;
    else if ( !s.write( buffer ) )
        return false;
    else if ( !s.waitForReadyRead( 3000 ) )
        return false;

    buffer = s.readAll();
    s.close();

    // parse buffer
    if (( 8 < buffer.length() ) && ( 0x1e == buffer[buffer.length()-1] ))
    {
        buffer.replace( '\x1e', '\x1f' );

        // split buffer into tags
        const QList<QByteArray> tags( buffer.split( '\x1f' ) );

        if ( 5 <= tags.size() )
            if ( "RT8A" == tags[0] )
            {
                // parse out data we are interested in
                foreach ( const QByteArray &tag, tags )
                    if ( 2 < tag.length() )
                    {
                        // store number
                        if ( "si" == tag.left( 2 ) )
                            store_ = tag.mid( 2 ).toInt();

                        // dept number
                        else if ( "Dn" == tag.left( 2 ) )
                            dept_ = tag.mid( 2 ).toInt();
                    }

                return true;
            }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString mainWindow::loadConfig(const QString &key)
{
    QString config = "";
    
    QSqlQuery query;
    query.prepare("select CoValue from configs where CoName = :key");
    query.bindValue(":key", key);
    
    qDebug() << "reading" << key << "from configs";

    if (query.exec() && query.next())
    {
        config = query.value(0).toString();
    }
    else
    {
      qWarning() << "failed to read" << key << "from configs:" << query.lastError();
    }
    
    return config;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// N.B. if you do this the backend won't know about the new value!
QSqlError mainWindow::saveConfig(const QString &key, const QString &value)
{
    QSqlQuery query;
    query.prepare("update configs set CoValue = :value where CoName = :key");
    query.bindValue(":key", key);
    query.bindValue(":value", value);
    
    qDebug() << "saving" << key << "into configs with value" << value;
    
    if (!query.exec())
    {
      qWarning() << "failed to save" << key << "config:" << query.lastError();
    }
    
    return query.lastError();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::writeTag(const QString &tag, const QString &value)
{
  QString message = "RTB9" + QString(char(0x1F)) + tag + value + QString(char(0x1E));

  qDebug() << "writeTag: " << tag;
  
  // TODO: safety checks? opening a local socket is always pretty reliable/fast
  auto socket = new QTcpSocket();
  socket->connectToHost("localhost",6000);
  socket->waitForConnected();

  socket->write("RTB9\x1f");
  socket->write(tag.toStdString().c_str());
  socket->write(value.toStdString().c_str());
  socket->write("\x1e");
  
  socket->close();      
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::populateZoneBox(QNetworkReply* reply)
{
  scaleZone_->clear();

  if(reply->error() == QNetworkReply::NoError)
  {
    QString strReply = (QString)reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    QJsonArray jsonArray = jsonResponse.array();

    // add each zone
    foreach (const QJsonValue& value, jsonArray)
    {
      QJsonObject obj = value.toObject();
      qDebug() << "zone:" << obj.value("ZoneNum") << "dept:" << obj.value("DeptNum") << "name:" << obj.value("ZoneName");

      scaleZone_->addItem(obj.value("ZoneName").toString(), obj.value("ZoneNum").toVariant());
    }

    // get the current zone and try to select it
    int currentZone = loadConfig("onlineOrderingZone").toInt();
    int i = scaleZone_->findData(currentZone);

    if (i != -1)
    {
      scaleZone_->setCurrentIndex(i);
    }
    else
    {
      qDebug() << "scale configured with zone the server doesn't know about!";
      scaleZone_->addItem("Unknown", QVariant(currentZone));
      scaleZone_->setCurrentIndex(scaleZone_->findData(currentZone));
    }
  }
  else
  {
    QMessageBox msgBox;
    msgBox.setText( "Failed to load zones from server!" );
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
  }

  reply->deleteLater();
}

void mainWindow::requestZones(void)
{
    QString dept = loadConfig("mostRecentActiveDeptNum");
    QString server = loadConfig("onlineOrderingServer");

    qDebug() << "loading zones, dept:" << dept << ", server:" << server;

    auto net = new QNetworkAccessManager(this);
    QObject::connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(populateZoneBox(QNetworkReply*)));

    QUrl url("http://" + server + "/api/v1/zones/" + dept);
    net->get(QNetworkRequest(url));
}

int mainWindow::countOfflineMinutes(void)
{
    int secondsDown = 0;
    std::ifstream log("/usr/local/hobart/oo_connectivity.log");
    std::string today = QDateTime::currentDateTime().toString("yyyy-MM-dd").toStdString();

    if (log.is_open())
    {
        std::string line;

        while (std::getline(log, line))
        {
            // check if line starts with the current date
            if (line.rfind(today) == 0)
            {
                QStringList lineWords = QString::fromUtf8(line.c_str()).split(" ");

                // fourth "word" is the number of seconds since previous check
                if (lineWords.length() > 4)
                {
                    secondsDown += lineWords[3].toInt();
                }
            }
        }
    }
    else
    {
        qDebug() << "couldn't read from OO connectivity log, assuming never offline";
    }

    qDebug() << "got" << secondsDown << "seconds offline from OO connectivity log";

    // if we have any downtime, always return at least 1
    if (secondsDown > 0 && secondsDown < 60) secondsDown = 60;

    return secondsDown / 60;
}