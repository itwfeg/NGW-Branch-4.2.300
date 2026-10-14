//#define USE_QDEBUG
/**
 * @file dynamicsoftkeybutton.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "dynamicsoftkeybutton.h"
#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../sessioninfo.h"
#include "../modes/operatemode.h"
#include "../../sessionHandlerWidgets/softkeytogglemanager.h"
#include <backgroundwidgettypes.h>
#include <logmanager.h>
#include <pixmap.h>

#include <QPainter>
#include <QDebug>
#include <Session.h>
#include <ScaleConfiguration.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicSoftkeyButton::dynamicSoftkeyButton( QWidget *parent ) :
    _Mybase( parent ),
    scaleConfig_( NULL ),
	operateMode_( NULL ),
    activePLU_( NULL )
{
#ifdef USE_QDEBUG
    qDebug() << "dynamicSoftkeyButton::dynamicSoftkeyButton():  parent = " << parent;
#endif

    // set object name
    setObjectName( "dynamicSoftkeyButton" );
    LOG_CLASS_INFO( "dynamicSoftkeyButton", "dynamicSoftkeyButton" );
    LOG_DEBUG( "Constructor", NULL );

    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();
    Session *session = info()->session();
    scaleConfig_ = session->scaleConfig;

    if ( mainWindow )
        operateMode_ = mainWindow->retrieveMode<OperateMode>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicSoftkeyButton::~dynamicSoftkeyButton()
{
    LOG_CLASS_INFO( "dynamicSoftkeyButton", "~dynamicSoftkeyButton" );
    LOG_DEBUG( "Destructor", NULL );

    if (scaleConfig_ )
        scaleConfig_ = NULL;

#ifdef USE_QDEBUG
    qDebug() << "dynamicSoftkeyButton::~dynamicSoftkeyButton(*)";
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicSoftkeyButton::setAction( int value )
{
    _Mybase::setAction( value );

    sessionInfo info( this );
    SessionMainWindow *mainWindow = info();
    if ( mainWindow )
        operateMode_ = mainWindow->retrieveMode<OperateMode>();
#ifdef USE_QDEBUG
    else
        qDebug() << "dynamicSoftkeyButton::setAction(" << value << "):  Can't get mainWindow!";
#endif

    if ( operateMode_ )
	{
        connect( operateMode_, SIGNAL( currentProductUpdateField(DomUpdateRunModeField) ), SLOT( onCurrentProductUpdateField(DomUpdateRunModeField) ) );
        connect( operateMode_, SIGNAL( currentProductChanged(bool) ), SLOT( onCurrentProductChanged(bool) ) );
	}
#ifdef USE_QDEBUG
    else
        qDebug() << "dynamicSoftkeyButton::setAction(" << value << "):  operateMode_ is NULL so can't set up the slot for currentProductUpdateField!";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicSoftkeyButton::onCurrentProductUpdateField( const DomUpdateRunModeField& m )
{
    LOG_CLASS_INFO( "dynamicSoftkeyButton", "onCurrentProductUpdateField" );

    const QString newValue( const_cast<DomUpdateRunModeField*>( &m )->newValue() );

    if ( Action_ChangeLabelType == action() )
    {
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::onCurrentProductUpdateField():  Action_ChangeLabelType softkey.  actionParam = " << actionParam();
#endif
	    LOG_DEBUG( "Action_ChangeLabelType softkey.  actionParam = %s.", qPrintable(actionParam()) );

        //  Do we have an active PLU?
        int pluNum = getActivePluNumber();
        if (pluNum > 0)
        {
            QString theActionParamStr( actionParam() );
#ifdef USE_QDEBUG
            qDebug() << "dynamicSoftkeyButton::onCurrentProductUpdateField():  pluNum = " << pluNum;
#endif
            int printer = -1;
            int labelType = -1;

			parseChangeLabelTypeActionParam(printer, labelType);

			int offset = (OperateMode::ProductWriteSingleFieldOffset) m.offsetIndex();

#ifdef USE_QDEBUG
            qDebug() << "dynamicSoftkeyButton::onCurrentProductUpdateField():  printer = " << printer << ",  labelType = " << labelType << ",  offset = " << offset;
#endif

			if ( (( printer == 0 ) && ( offset == OperateMode::OffsetPrimaryLabelType )) ||
				 (( printer == 1 ) && ( offset == OperateMode::OffsetSecondLabelType )) ||
				 (( printer == 2 ) && ( offset == OperateMode::OffsetThirdLabelType )) )
            {
#ifdef USE_QDEBUG
            	qDebug() << "dynamicSoftkeyButton::onCurrentProductUpdateField():  printer = " << printer << ",  labelType = " << labelType << ",  offset = " << offset << "  --    Q_EMIT pressed()";
#endif
                Q_EMIT pressed();
            }
		}
	}
#ifdef USE_QDEBUG
    qDebug() << "dynamicSoftkeyButton::onCurrentProductUpdateField():  Done.";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int dynamicSoftkeyButton::getActivePluNumber()
{
    //  Do we have an active PLU?
    int pluNum = 0;

    if ( operateMode_ )
        activePLU_ = operateMode_->currentProduct();
#ifdef USE_QDEBUG
    else
        qDebug() << "dynamicSoftkeyButton::getActivePluNumber():  operateMode_ is NULL!";
#endif
    if (activePLU_ && !activePLU_->itemNumber().isEmpty())
        pluNum = activePLU_->itemNumber().toInt();		//	FT-1676
#ifdef USE_QDEBUG
    else
		qDebug() << "dynamicSoftkeyButton::getActivePluNumber():  activePLU_ is NULL or activePLU_.itemNumber().isEmpty()!";

	qDebug() << "dynamicSoftkeyButton::getActivePluNumber():  return pluNum = " << pluNum;
#endif
	return pluNum;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int dynamicSoftkeyButton::getActiveLabelType(int printer)
{
    //  Do we have an active PLU?
    int labelType = 0;

    if ( operateMode_ )
        activePLU_ = operateMode_->currentProduct();
#ifdef USE_QDEBUG
    else
        qDebug() << "dynamicSoftkeyButton::getActiveLabelType(" << printer << "):  operateMode_ is NULL!";
#endif
    if (activePLU_)
	{
        // TODO - JOE AUTO LABELS - This just assumes the default, which is obviosuly not the case.
        // TODO - JOE AUTO LABELS - There is a lot of code that gives you correct label,
        // TODO - JOE AUTO LABELS - and I would hate to just copy it all and plop it in.
        switch (printer)
        {
            case 2:
                labelType = activePLU_->thirdDefaultLabelType();      // thirdLabelTypes();
            break;
            case 1:
                labelType = activePLU_->secondDefaultLabelType();      // secondLabelTypes();
            break;
            case 0:
            default:
                labelType = activePLU_->primaryDefaultLabelType();      //primaryLabelTypes();
            break;
        }
	}
#ifdef USE_QDEBUG
    else
		qDebug() << "dynamicSoftkeyButton::getActiveLabelType(" << printer << "):  activePLU_ is NULL!";
#endif
#ifdef USE_QDEBUG3
		qDebug() << "dynamicSoftkeyButton::getActiveLabelType(" << printer << "):  labelType = " << labelType;
#endif
	return labelType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicSoftkeyButton::parseChangeLabelTypeActionParam(int &printer, int &labelType)
{
    QString theActionParamStr( actionParam() );
    printer = -1;
    labelType = -1;
    QStringList list = theActionParamStr.split( "," );
    if ( !list.empty() && (list.size() == 2) )
    {
        printer = list[0].toInt();
        labelType = list[1].toInt();
    }
#ifdef USE_QDEBUG3
	qDebug() << "dynamicSoftkeyButton::parseChangeLabelTypeActionParam():  printer = " << printer << ",  labelType = " << labelType;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicSoftkeyButton::onCurrentProductChanged( bool /*nof*/ )
{
#ifdef USE_QDEBUG
    qDebug() << "dynamicSoftkeyButton::onCurrentProductChanged()";
#endif
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dynamicSoftkeyButton::paintEvent( QPaintEvent *e )
{
    LOG_CLASS_INFO( "dynamicSoftkeyButton", "paintEvent" );

    // invoke base method
    _Mybase::paintEvent( e );

    bool softkeyIsSelected = false;
    if ( Action_ChangeLabelType == action() )
    {
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  Action_ChangeLabelType softkey.  actionParam = " << actionParam();
#endif
	    LOG_DEBUG( "Action_ChangeLabelType softkey.  actionParam = %s.", qPrintable(actionParam()) );

        //  Do we have an active PLU?
        int pluNum = getActivePluNumber();
        if (pluNum > 0)
        {
		    int printer = -1;
		    int labelType = -1;
            int activeLabelType = -1;

			parseChangeLabelTypeActionParam(printer, labelType);

            if ((printer >= 0) && (labelType > 0))
            {
                activeLabelType = getActiveLabelType(printer);
#ifdef USE_QDEBUG
            qDebug() << "dynamicSoftkeyButton::paintEvent():  labelType = " << labelType << ",  activeLabelType = " << activeLabelType;
#endif
                // If both labels are the same or in the same family, we will draw the flashkey as selected.
                // AND the clicked flashkey matches this flashkey
                DomSoftkey *clickedSoftkey( &scaleConfig_->softkeyItem );
                // allows families but could have trouble un-selecting flashkey
                //if ( (labelType == activeLabelType) || ( ((labelType / 10) * 10) == ((activeLabelType / 10) * 10) ) )

                if ( labelType == activeLabelType )
                    if( ( clickedSoftkey->groupNum() ==  group() ) && ( clickedSoftkey->hPosition() == gridPos().x() ) && ( clickedSoftkey->vPosition() == gridPos().y() ) )
                        softkeyIsSelected = true;
            }
        }
#ifdef USE_QDEBUG
		else
            qDebug() << "dynamicSoftkeyButton::paintEvent():  There is no active PLU.";
#endif
    }

#ifdef USE_QDEBUG
    if ( softkeyIsSelected )
    	qDebug() << "dynamicSoftkeyButton::paintEvent():  softkey IS selected";
	else
    	qDebug() << "dynamicSoftkeyButton::paintEvent():  softkey is NOT selected";
#endif
    if ( softkeyIsSelected )
	{
		//	Starting with the inverted we need to clear out the center, leaving a border of a certain size.
	    QRect mainRect( rect() );
        int borderWidth = 20;

        //  Make sure the border width does not take up the majority of the softkey.
        int width = mainRect.size().width();
        int height = mainRect.size().height();

#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  Button width = " << width << ", button height = " << height << ",  borderWidth = " << borderWidth;
#endif

        if ( ( height - (borderWidth * 2) ) < borderWidth )
        {
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  The borderWidth of " << borderWidth << " is too large for the height.";
#endif
            borderWidth = height / 4;
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  Reduced borderWidth to " << borderWidth;
#endif
        }

        if ( ( width - (borderWidth * 2) ) < borderWidth )
        {
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  The borderWidth of " << borderWidth << " is too large for the width.";
#endif
            borderWidth = width / 4;
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  Reduced borderWidth to " << borderWidth;
#endif
        }

#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  Creating the dynamicSelectedImage_ image with an inverted border of " << borderWidth << "pixels.";
#endif

        QRect openRect = mainRect.adjusted( borderWidth, borderWidth, -borderWidth, -borderWidth);
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  mainRect = " << mainRect.left() << ", " << mainRect.right() << ", " << mainRect.top() << ", " << mainRect.bottom();
        qDebug() << "dynamicSoftkeyButton::paintEvent():  openRect = " << openRect.left() << ", " << openRect.right() << ", " << openRect.top() << ", " << openRect.bottom();
#endif

        QPainter p(this);
        int innerHeight = openRect.height();
        int innerWidth = openRect.width();

        invertPixmap inverter;
        QPixmap myInverted = inverter(releasedImage_);

        //  Copy the completed inverted image
        p.setCompositionMode(QPainter::CompositionMode_Source);
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  p.drawPixmap(" << mainRect.left() << ", " << mainRect.top() << ", " << width << ", " << height << ", myInverted)";
#endif
        p.drawPixmap(mainRect.left(), mainRect.top(), width, height, myInverted);

        //  Now copy the inner area of the releasedImage_ back to the inverted image
#ifdef USE_QDEBUG
        qDebug() << "dynamicSoftkeyButton::paintEvent():  p.drawPixmap(" << openRect.left() << ", " << openRect.top() << ", " << innerWidth << ", " << innerHeight << ", releasedImage_, " << openRect.left() << ", " << openRect.top() << ", " << innerWidth << ", " << innerHeight << ")";
#endif
        p.drawPixmap(openRect.left(), openRect.top(), innerWidth, innerHeight, releasedImage_, openRect.left(), openRect.top(), innerWidth, innerHeight);
        
		//	Draw the text on the softkey
		writeText();
    }
#ifdef USE_QDEBUG
    qDebug() << "dynamicSoftkeyButton::paintEvent():  Done";
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicSoftkeyButtonFactory::dynamicSoftkeyButtonFactory() :
    _Mybase()
{
#ifdef USE_QDEBUG
    qDebug() << "dynamicSoftkeyButtonFactory::dynamicSoftkeyButtonFactory()";
#endif
    LOG_CLASS_INFO( "dynamicSoftkeyButton", "dynamicSoftkeyButtonFactory" );
    LOG_DEBUG( "Constructor", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dynamicSoftkeyButtonFactory::~dynamicSoftkeyButtonFactory()
{
#ifdef USE_QDEBUG
    qDebug() << "dynamicSoftkeyButtonFactory::~dynamicSoftkeyButtonFactory()";
#endif
    LOG_CLASS_INFO( "dynamicSoftkeyButton", "~dynamicSoftkeyButtonFactory" );
    LOG_DEBUG( "Destructor", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
softkeyButton *dynamicSoftkeyButtonFactory::create( QWidget *parent ) const
{
#ifdef USE_QDEBUG
    qDebug() << "dynamicSoftkeyButtonFactory::create()";
#endif
    LOG_CLASS_INFO( "dynamicSoftkeyButton", "create" );
    LOG_DEBUG( "return new dynamicSoftkeyButton( parent )", NULL );
    // create new button
    return new dynamicSoftkeyButton( parent );
}
