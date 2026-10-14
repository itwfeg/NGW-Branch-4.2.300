#include "opermsg.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <QGuiApplication>

///////////////////////////////////////////////////////////////////////////////////////////////////
operMsg::operMsg( QWidget *parent ) :
    _Mybase( parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint )
{
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
operMsg::~operMsg()
{    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setMessage( const QString& value )
{
    message_->setText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString operMsg::message() const
{
    return message_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setMessageFontFamily( const QString& value )
{
    QFont f( message_->font() );
    f.setFamily( value );
    message_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString operMsg::messageFontFamily() const
{
    QFont f( message_->font() );
    return f.family();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setMessageFontPointSize( int value )
{
    QFont f( message_->font() );
    f.setPointSize( value );
    message_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int operMsg::messageFontPointSize() const
{
    QFont f( message_->font() );
    return f.pointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setTimer( int value )
{
    timer_->setSingleShot( true );
    timer_->start( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize operMsg::defaultSize()
{
    return QSize( 800, 600 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::resizeEvent( QResizeEvent *e )
{
    Q_UNUSED( e );

    //
    // scale logo to dialog width
    //

    QPixmap logo( ":/images/hobartMetal.png" );

    QSize scaledSize( logo.size() );
    scaledSize.scale( size(), Qt::KeepAspectRatio );

    scaledSize.setWidth(scaledSize.width() * 0.60);
    scaledSize.setHeight(scaledSize.height() * 0.60);

    QPixmap background( scaledSize );
    background.fill( Qt::transparent );

    QPainter p( &background );
    p.setRenderHint( QPainter::Antialiasing, true );

    p.drawPixmap( QRect( QPoint( 0, 0 ), scaledSize ), logo );

    //
    // map logo to widget
    //

    QPalette newPalette( logo_->palette() );
    newPalette.setBrush( logo_->backgroundRole(), background );


    logo_->setFixedSize( scaledSize );
    logo_->setPalette( newPalette );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::initialize()
{
    logo_ = new QLabel( this );
    logo_->setAutoFillBackground( true );
    logo_->setObjectName( "logo" );

    message_ = new QLabel( this );
    message_->setObjectName( "message" );
    QFont msgFont = message_->font();
    msgFont.setPointSize(16);
    msgFont.setBold(true);
    message_->setFont(msgFont);
    message_->setText(tr("Choose a Scale Model"));

    checkOnIcon = QIcon(":/images/OSX-Checkbox-ON.png");		//	HTA-158
    checkOnIcon.actualSize(QSize(48,48));

    noIcon = QIcon("");

    serviceCounterButton_ = new QPushButton(this);
    serviceCounterButton_->setObjectName("serviceCounterButton");
    serviceCounterButton_->setText("Service Counter");
    //serviceCounterButton_->setStyleSheet("QPushButton#serviceCounterButton_ {border-radius: 12px; color: rgb(255,255,255); border: 2px solid gray; background: rgb(0,0,0);}");
    serviceCounterButton_->setGeometry(100,200,250,150);
    serviceCounterButton_->setFixedSize(250,150);
    serviceCounterButton_->setCheckable(true);

    QSize theSize = serviceCounterButton_->size();

    QFont font = serviceCounterButton_->font();
    font.setPointSize(16);
    font.setBold(true);
    serviceCounterButton_->setFont(font);

    prePackButton_ = new QPushButton(this);
    prePackButton_->setObjectName("prePackButton");
    prePackButton_->setText("Pre-Pack");
    prePackButton_->setGeometry(250,200,250,150);
    prePackButton_->setFixedSize(theSize);
    prePackButton_->setCheckable(true);
    prePackButton_->setFont(font);

    okayButton_ = new QPushButton(this);
    okayButton_->setObjectName("okayButton");
    okayButton_->setText("OK");
    okayButton_->setFixedSize(200,75);
    okayButton_->setCheckable(false);
    okayButton_->setFont(font);

    timer_ = new QTimer( this );
    timer_->setObjectName( "timer" );
    connect(okayButton_, SIGNAL(clicked()), this, SLOT(okayButtonClick()));
    connect(serviceCounterButton_, SIGNAL(toggled(bool)), this, SLOT(scButtonToggle(bool)));
    connect(prePackButton_, SIGNAL(toggled(bool)), this, SLOT(ppButtonToggle(bool)));

    connect( timer_, SIGNAL( timeout() ), SLOT( reject() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::createLayout()
{
    // delete old layout
    if ( layout() )
        delete layout();

    QHBoxLayout *h3layout( new QHBoxLayout() );
    h3layout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    h3layout->addStretch();
    h3layout->addWidget( logo_);
    h3layout->addStretch();

    QHBoxLayout *hlayout( new QHBoxLayout() );
    hlayout->setContentsMargins( QMargins( 0, 30, 0, 0 ) );

    hlayout->addStretch();
    hlayout->addWidget( message_ );
    hlayout->addStretch();

    QHBoxLayout *h1layout( new QHBoxLayout() );
    h1layout->setContentsMargins( QMargins( 0, 20, 20, 0 ) );
    h1layout->addStretch();
    h1layout->addWidget(serviceCounterButton_);
    h1layout->addWidget(prePackButton_);
    h1layout->addStretch();
    h1layout->setSpacing(125);

    QHBoxLayout *h2layout( new QHBoxLayout() );
    h2layout->setContentsMargins( QMargins( 0, 70, 0, 0 ) );
    h2layout->addStretch();
    h2layout->addWidget(okayButton_);
    h2layout->addStretch();

    QVBoxLayout *vlayout( new QVBoxLayout( this ) );
    vlayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    //vlayout->addWidget( logo_ );
    vlayout->addStretch();
    vlayout->addLayout(h3layout);
    vlayout->addLayout(hlayout);
    vlayout->addLayout(h1layout);
    vlayout->addLayout(h2layout);

    vlayout->addStretch();
}

void operMsg::ppButtonToggle(bool isDown)
{
	if(isDown)
    {
		serviceCounterButton_->setChecked(false);
		serviceCounterButton_->setIcon(noIcon);
    }
    prePackButton_->setIcon(checkOnIcon);
    prePackButton_->setIconSize(QSize(48,48));
}

void operMsg::scButtonToggle(bool isDown)
{
    if(isDown)
    {
        prePackButton_->setChecked(false);
        prePackButton_->setIcon(noIcon);
    }
    serviceCounterButton_->setIcon(checkOnIcon);
    serviceCounterButton_->setIconSize(QSize(48,48));
}

void operMsg::okayButtonClick()
{
    if(serviceCounterButton_->isChecked())
    {
        changeScaleModel("3");
        exit(0);
    }
    else if(prePackButton_->isChecked())
    {
        changeScaleModel("2");
        exit(0);
    }
    else
    {
        ;
    }
}

bool operMsg::changeScaleModel(QString modelNumber)
{
    QString rootDirStr;
    QString rootFileStr;
    QString configFileName;
    QString configTempFileName;
    QString datExt = "dat";
    QString tempExt = "TEMP";
	QString scaleModelName = "scaleModel";
    QString newScaleModel = scaleModelName + "=" + modelNumber;
    QString printerSNRMTUI = "printerStatusNonRepeatingMsgsToUI";
    QString printerSNRMTUI_prepack = printerSNRMTUI + "=F";
    bool isPrepack = ((modelNumber == "2") ? true : false);

	//	We need to remove 'scaleModel' from config.dat or config_linux.dat.
    if (QGuiApplication::platformName() == "windows")
    {
        rootDirStr = "C:\\Program Files\\Hobart\\";
        rootFileStr = rootDirStr + "config.";
    }
    else
    {
        //It is linux
        rootDirStr = "/etc/hobart/";
        rootFileStr = rootDirStr + "config_linux.";
    }
    configFileName = rootFileStr + datExt;
    configTempFileName = rootFileStr + tempExt;

    QDir dir;

	//	First, rename the file if it exists.
    if (QFile::exists(configFileName))
    {
	    //	Make sure our temporary file does not exist already.
		if (QFile::exists(configTempFileName))
	    {
        	dir.remove(configTempFileName);
		}
        dir.rename(configFileName, configTempFileName);

        QStringList fileContents;
        QString line;
        QStringList theLine;
        if (QFile::exists(configTempFileName))
	    {
		    QFile fileIn(configTempFileName);	//	Read the renamed config file
		    if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text))
		    {
		        return false;
		    }

		    QTextStream in(&fileIn);
		    fileContents.clear();

		    while (!in.atEnd())
		    {
		        line = in.readLine();
                theLine = line.split("=", QString::KeepEmptyParts);

                if (!((theLine.at(0) == scaleModelName) || (isPrepack && (theLine.at(0) == printerSNRMTUI))))
				{
					fileContents.append(line);
				}
		    }
		    fileIn.close();

			//	We are ready to write our fileContents to a new config file.
			writeContentsToFile(configFileName, fileContents);

            //	We need to delete the temporary file now.
            dir.remove(configTempFileName);
		}
		else
		{
			//	We have a problem with no config file.
			return false;
		}

		//	HA-158: We need to make sure that configc.dat contains the scaleModel - per Larry Pevoar.
	    rootFileStr = rootDirStr + "configc.";
	    configFileName = rootFileStr + datExt;
	    configTempFileName = rootFileStr + tempExt;

	    if (QFile::exists(configFileName))
	    {
		    //	Make sure our temporary file does not exist already.
			if (QFile::exists(configTempFileName))
		    {
	        	dir.remove(configTempFileName);
			}
	        dir.rename(configFileName, configTempFileName);
		    if (QFile::exists(configTempFileName))
		    {
			    QFile fileIn2(configTempFileName);	//	Read the renamed config file
			    if (!fileIn2.open(QIODevice::ReadOnly | QIODevice::Text))
			    {
			        return false;
			    }
                QTextStream in2(&fileIn2);
			    fileContents.clear();

			    while (!in2.atEnd())
			    {
			        line = in2.readLine();
                    theLine = line.split("=", QString::KeepEmptyParts);

	                if (!((theLine.at(0) == scaleModelName) || (isPrepack && (theLine.at(0) == printerSNRMTUI))))
					{
						fileContents.append(line);
					}
			    }
				fileContents.append(newScaleModel);					//	HTA-168: Now append it to the end.
				if (isPrepack)
					fileContents.append(printerSNRMTUI_prepack);	//	HTA-101: Now append it to the end.
			    fileIn2.close();

				//	We are ready to write our fileContents to a new config file.
				writeContentsToFile(configFileName, fileContents);

                //	We need to delete the temporary file now.
                dir.remove(configTempFileName);
            }
	    }

	}
	return true;
}

bool operMsg::writeContentsToFile(QString fileName, QStringList theContents)
{
    QFile fileOut(fileName);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    QTextStream outStream(&fileOut);

    for(int i = 0; i < theContents.count(); i++)
    {
        outStream << theContents.at(i) + "\n";
    }

    fileOut.close();
	return true;
}
