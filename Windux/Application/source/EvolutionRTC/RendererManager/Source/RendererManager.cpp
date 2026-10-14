#include "RendererManager.h"

#include "Renderer.h"
#include "ImageControl.h"
#include "UltimaObjects.h"
#include "DrawingPrimitives.h"
#include "FormRecord.h"

#include "EvolMessage.h"
#include "RendererSessionId.h"
#include "RendererRequestSession.h"
#include "RendererSetDestination.h"
#include "RendererSetSource.h"
#include "RendererRequestRendering.h"
#include "RendererRenderComplete.h"
#include "RendererRenderStart.h"
#include "RendererCloseSession.h"
#include "RendererInitDotHeadWear.h"
#include "RendererNewDotHeadWear.h"

#include "DiagnosticsPrintLabelStats.h"

#include "Configuration.h"

#include "CRendererUtil.h"
#include "util/e_string/utilstring.h"
#include "util/thread/lock.h"

#include "Debug.h"
#include "HobString.h"

#include "BitMap.h"

#include "Hex.h"

#if !defined( __linux__)
#include <algorithm>
#endif


// SOF-451
#ifdef __linux__

#include <dsmsdk.h>
#include <PayloadHelper.h>

#endif

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RendererManager::dataType = 0;
std::string RendererManager::dataTypeName("RendererManager"); 
#endif


RendererManager* RendererManager::_instance_ = 0;
util_thread::Mutex<> RendererManager::_instanceMutex_ = util_thread::Mutex<>();



RendererManager::RendererManager() 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	subtractDescent_ = Configuration::getInstance()->toBoolCfgI(CfgInternal::SUBTRACTDESCENT);

	registerEvent(Renderer, RdReqSession);
	registerEvent(Renderer, RdSetSource);
	registerEvent(Renderer, RdSetDest);
	registerEvent(Renderer, RdReqRendering);
	registerEvent(Renderer, RdCloseSession);
	registerEvent(Renderer, RdInitDotHeadWear);
	registerEvent(Renderer, RdNewDotHeadWear);

	sessionCount = 0;
	printheadSize = 0;
	marginalBits = nullptr;
}


RendererManager *RendererManager::getInstance()
{
	if ( _instance_ == 0 )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_instanceMutex_);

		if ( _instance_ == 0 )
			_instance_ = new RendererManager;
	}

	return _instance_;
}


// Hopefully this code is never executed since nobody seems to call the destructor
// of any instance of this class. Otherwise we may need to make it more thread safe
// at least in relation to the 'rendererMap' data member.
RendererManager::~RendererManager()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	ShutdownNicely();
	
	// delete elements from the map
	dataLocationContainerIterator iter = rendererMap.begin();
	while ( iter != rendererMap.end() )
	{
		RendererDataLocation *pDataLocation = rendererMap[iter->first];

		rendererMap.erase(iter++);

		delete pDataLocation;
	}

	//delete code checker bit array
	if ( marginalBits )
		delete [] marginalBits;
}


void RendererManager::processEvent( Util::CSmartPtr<EvolMessageWrapper> & msg )
{
	Debug::trace( "RendererManager", "===== processEvent " + util_string::uinttostring(msg->getEventType()) + " =====" );

	switch ( msg->getEventType() )
	{
	case RdReqSession:
		handleOpenSession(msg->Data());
		break;
	case RdSetSource:
		handleSetSource(msg->Data());
		break;
	case RdSetDest:
		handleSetDestination(msg->Data());
		break;
	case RdReqRendering:
		handleRendering(msg->Data());
		break;
	case RdCloseSession:
		handleCloseSession(msg->Data());
		break;
	case RdInitDotHeadWear:
		handleInitDotHeadWear(msg->Data());
		break;
	case RdNewDotHeadWear:
		handleNewDotHeadWear(msg->Data());
		break;
	default:
		break;
	}

	Debug::trace( "RendererManager", "===== event complete =====" );
}


void RendererManager::handleOpenSession(const char *buffer)
{
	sessionCount++;
	RendererRequestSession rdRequestSession(buffer);
	
	Debug::trace("RendererManager.OpenSession", "Creating session: " + util_string::uinttostring(sessionCount) +
			"Object: 0x" + util_string::ptrtostring(rdRequestSession.getSourceObject()));

	RendererSessionId rendererSessionId(this, sessionCount);

	// We probably should not call the send method directly. We could just post the
	// message to the PrinterManager using our own dispatchEvent method as in:
	dispatchEvent(&rendererSessionId, rdRequestSession.getSourceObject());

	// create a new session enviroment
	RendererDataLocation *pDataLocation = new RendererDataLocation;
	pDataLocation->printerType = rdRequestSession.getPrinterType();
    pDataLocation->sourceId = NULL;
    pDataLocation->destinationId = NULL;

	// add sessionId, sourceID = NULL, destinationID = NULL to the renderer's map
	rendererMap.insert(std::make_pair(sessionCount, pDataLocation));	
}


void RendererManager::handleSetSource(const char *buffer)
{
	RendererSetSource rendererSetSource(buffer);
	UINT16 sessionId = rendererSetSource.getSessionId();
	const UINT8 *pSourceId = rendererSetSource.getSourcePointer();

	if ( rendererMap.find(sessionId) != rendererMap.end() )
	{
		//update this sessions sourceId
		RendererDataLocation *pDataLocation = rendererMap[sessionId];
		pDataLocation->sourceId = pSourceId;

		Debug::trace("RendererManager.SetSource",
				"Session id: " + util_string::uinttostring(sessionId) +
				"setting sourceId: 0x" + util_string::ptrtostring(pSourceId));
	}
	else
	{
		Debug::trace("RendererManager.Error", "SetSource -- Session id not found: " +
				util_string::uinttostring(sessionId));
	}
}


void RendererManager::handleSetDestination(const char *buffer)
{
	RendererSetDestination rendererSetDest(buffer);
	UINT16 sessionId = rendererSetDest.getSessionId();
	const UINT8 *pDestinationId = rendererSetDest.getDestinationPointer();

	if ( rendererMap.find(sessionId) != rendererMap.end() )	
	{
		//update this sessions destinationId
	   	RendererDataLocation *pDataLocation = rendererMap[sessionId];
		pDataLocation->destinationId = pDestinationId;

		Debug::trace("RendererManager.SetDestination",
				"Session id: " + util_string::uinttostring(sessionId) +
				"setting destinationId: 0x" + util_string::ptrtostring(pDestinationId));
	}
	else
	{
		Debug::trace("RendererManager.Error", "SetDestination -- Session id not found: " +
				util_string::uinttostring(sessionId));
	}
}


void RendererManager::handleRendering(const char *buffer)
{
	RendererRequestRendering rdReqRendering(buffer);
	UINT16 sessionId = rdReqRendering.getSessionId();
	 
	if ( rendererMap.end() == rendererMap.find(sessionId) )
	{
		Debug::trace("RendererManager.Error", "Rendering -- Session id not found: " + util_string::uinttostring(sessionId));
		return;
	}

	RendererDataLocation *pDataLocation = rendererMap[sessionId];
	ImageControl *ic = (ImageControl *) pDataLocation->sourceId;

	if ( !ic )
	{
		Debug::trace("RendererManager.Error", "Rendering -- Invalid Image Control" );
		return;
	}

	if ( !pDataLocation->destinationId )
	{
		Debug::trace("RendererManager.Error", "Rendering -- Invalid Destination" );
		return;
	}

	RendererRenderStart rdRenderStart(this, PAL_absclass::PAL_getTickCount());

	// We probably should not call the send method directly. We could just post the
	// message to the PrinterManager using our own dispatchEvent method as in:
	dispatchEvent(&rdRenderStart, rdReqRendering.getSourceObject());

	Debug::trace("RendererManager.handleRendering", "Rendering START printerType " + Long::toString( pDataLocation->printerType ) + " ticks " + Long::toString( PAL_absclass::PAL_getTickCount() ));
	renderLabel(ic, pDataLocation->printerType);

	// If we need to rotate the whole label, we'll do it now
	UINT16 rotation = ic->getLabelOrientationDegrees();
	bool postImaging = ic->isImageFlagPostImage();
	if ( rotation && postImaging )
	{
		Debug::trace("RendererManager.landscapeLabels", "Rendered landscape label. Now going to rotate it.");

		Hobart::Rectangle rect(ic->getImageBitMapBounds());
		Hobart::Rectangle rotatedRect(ic->getRotatedBitMapBounds());


		if (rotation == 90 || rotation == 270)
		{
			//	Set the left and right coordinates of the rotatedRect so it can be centered in the rotated Bitmap.
			rotatedRect.setRectangleLeft(rect.getRectangleTop() + ic->getRotatedImgOffset());
			rotatedRect.setRectangleRight(rect.getRectangleBottom() + ic->getRotatedImgOffset());
		}

	// ***** WARNING: this function is not ported yet in DrawingPrimitives.cpp
		eraseRectangle(ic->getRotatedBitMap(), &rotatedRect, 0x0000);


		// A portrait label is oriented vertically and the top of the label
		// is printed last. A landscape label (rotation = 90) is oriented
		// horizontally and in order to make the left hand side be printed
		// last (as if it was the top) in needs to be rotated -90 degrees.
		// That is why the rotation is passed as "360 - rotation".
		rotateImage(ic->getImageBitMap(), &rect, ic->getRotatedBitMap(), &rotatedRect, 360 - rotation);
	}

	// Reset the image flags as an indication that this image control block is done being imaged (pre- or post- imaged).
	// This was done as part of the virtual printer changes.
	ic->setImageFlags(0);

	//anounce that the rendering has completed
	if ( postImaging && ic->getSaveToFilenameString().find(Configuration::getInstance()->toStringCfgI(fieldOffsetCfgIPrintToScreenBarcodeFilename)) != std::string::npos )
	{
		// Yea, that's right, this is how I'm notifying that the self serve barcode label imaging is done rendering, by setting the tick counts to -42.
		// At least that's how I started to do it.  When we made a rotationg number of barcode image files, I now use the negative of the filename's counter value.
		// And this is why we don't rotate back to 0, since you can't have -0.
		// Really, it's a great idea, and it'll work!
		string fnameNumericPart = ic->getSaveToFilenameString().substr( ic->getSaveToFilenameString().find(Configuration::getInstance()->toStringCfgI(fieldOffsetCfgIPrintToScreenBarcodeFilename)) + Configuration::getInstance()->toStringCfgI(fieldOffsetCfgIPrintToScreenBarcodeFilename).length() );
		int counter = -stringtoint(fnameNumericPart);
		if (counter == 0)
			counter = -42;
		RendererRenderComplete rdRenderComplete(this, counter);
		dispatchEvent(&rdRenderComplete, rdReqRendering.getSourceObject());
	}
	else
	{
		RendererRenderComplete rdRenderComplete(this, PAL_absclass::PAL_getTickCount());

		// We probably should not call the send method directly. We could just post the
		// message to the PrinterManager using our own dispatchEvent method as in:
		dispatchEvent(&rdRenderComplete, rdReqRendering.getSourceObject());
	}

	Debug::trace("RendererManager.handleRendering", "Rendering COMPLETE printerType " + Long::toString( pDataLocation->printerType ) + " ticks " + Long::toString( PAL_absclass::PAL_getTickCount() ));
}

void RendererManager::renderLabel( const ImageControl *ic, int printerType )
{
	int barcodeType( -1 );
	std::string barcodeData;
	std::string barcodeDataRss;

	// SOF-2814 - Based on the digimarcExpandedFreshEnabled flag below, the label may be completely re-imaged, both pre-imaging and post-imaging.
	// This happens after the label has already been pre-imaged.  There is something about the rendering of REPORT_STREAM form objects that will
	// cause the data (garbage / unallocated memory) beyond the actual report data to be used in the second pre-imaging of the REPORT_STREAM form.
	// The rendering class updates the data pointer at the end of rendering it, causing it to get set to the byte beyond the actual data when it's
	// completely rendered.  I tried not updating the pointer at the end of rendering, so it still pints the to beginning of the data, allowing the
	// second pre-imaging of REPORT_STREAM to find the correct report data to image, but then the report just continued to print over and over to
	// the end of eternity. It was taking too much time to figure out how to let it correctly perform the second pre-imaging, so I just prevented it,
	// so that it works the same as before the existance of digimarcExpandedFreshEnabled.  So I don't set the renderEntireLabel variable to true if
	// this is a report (contains the REPORT_STREAM form).
	if ( !ic )
		return;

	bool isReport( false );

	ic->initFormListIter();

	for ( const FormRecord *form( ic->getFormFromList() ); form; form = ic->getFormFromList() )
		if ( REPORT_STREAM == form->getFieldType() )
		{
			isReport = true;
			break;
		}

	bool digimarcExpandedFreshEnabled( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgDigimarcExpandedFreshEnabled) );
	bool renderEntireLabel( digimarcExpandedFreshEnabled && (ic->getImageFlags() & PostImage) && !isReport );

	std::vector<Hobart::Rectangle> digimarcKeepOut;
	std::vector<Hobart::Rectangle> codeCheckerDisabled;

	Debug::trace("RendererManager.Render", "Going to render label for imageControlBlock 0x" + util_string::ptrtostring(ic));

	// when rendering entire label we need to preimage and postimage
	if ( renderEntireLabel )
	{
		// zero out image
		memset(ic->getImageBitMapBase(), 0, ic->getImageBitMapSize());

		UINT16 flags(ic->getImageFlags());
		flags &= ~PostImage;
		flags |= PreImage;

		const_cast<ImageControl*>(ic)->setImageFlags(flags);
	}

renderLabelLoop:

	UINT16 accumItemListPosition = 0;
	erasePostImageFormFields(ic);

	ic->initFormListIter();
	while ( !ic->isCurrentFormNull() )	//added to process the whole label
	{	
		bool renderEmbeddedLabel = false;
		FormRecord* pForm = ic->getFormFromList();
		BitMap* pBitMap = ic->getImageBitMap();
		const LabelStockDescriptor* embeddedLSD = 0;

		Debug::trace("RendererManager.Render", "Going to render label form " +
				util_string::ptrtostring(pForm));

		Debug::trace("RendererManager.Render", " Field Type: " + util_string::inttostring(pForm->getFieldType()));
		
		if (( pForm->getFieldType() == BARCODE ) || ( pForm->isFormFlagCodeCheckerDisabled() ))
		{
			const Hobart::Rectangle r(
				pForm->getOriginVertical(),
				pForm->getOriginVertical() + pForm->getSizeVertical(),
				pForm->getOriginHorizontal() + pForm->getSizeHorizontal(),
				pForm->getOriginHorizontal() );

			codeCheckerDisabled.push_back( r );
		}

		if ( pForm->getFieldType() == INDIRECT_LABEL_REFERENCE )
		{
			Debug::trace("RendererManager.embeddedLabels", "Going to render embedded label " +
					util_string::uinttostring(pForm->getVerticalJustify()) +
					" with bitmap " + util_string::ptrtostring(pForm->getBitMapData()) +
					" and it " + (pForm->isDataPresent() ? "has" : "has no") + " data present");

			UINT16 labelNum = pForm->getVerticalJustify();
			if (labelNum == Configuration::getInstance()->toIntCfgI(CfgInternal::ACCUMITEMSITEMLABELTYPE))
				labelNum = (labelNum * 100) + (++accumItemListPosition);

			const ImageControl* embeddedLabel = ic->getEmbeddedImageControl(labelNum);
			if ( embeddedLabel )
			{
				renderLabel(embeddedLabel);

				Debug::trace("RendererManager.embeddedLabels", "Completed rendering embedded label " +
						util_string::uinttostring(pForm->getVerticalJustify()) +
						" with bitmap " + util_string::ptrtostring(pForm->getBitMapData()) +
						" and it " + (pForm->isDataPresent() ? "has" : "has no") + " data present");
		
				if ( ic->isImageFlagPostImage() )
				{
					Debug::trace("RendererManager.embeddedLabels", "Going to embedded rendered label " +
							util_string::uinttostring(pForm->getVerticalJustify()) + " into current label");

					embeddedLSD = embeddedLabel->getLabelStockDescriptor();
					renderEmbeddedLabel = true;
				}
			}
			else
			{
				Debug::trace("RendererManager.embeddedLabels",
						"Unable to find embedded image control to render embedded label " +
						util_string::uinttostring(pForm->getVerticalJustify()));
			}
		}

		// Ensure that the current form is to be printed at this time
		// (pre-image vs. post-image)
		if ( (ic->getImageFlags() & pForm->getFlag()) || renderEmbeddedLabel )
        {
			renderField(ic, pForm, pBitMap, embeddedLSD, subtractDescent_);
			// HT-497
			if (pForm->isFormFlagInverted() )
			{
				//-------------------------------------
				// Print white text on black background
				//-------------------------------------

				UCHAR *buffer = (UCHAR *)pBitMap->getBitMapBase();
				int bmPitch = pBitMap->getBitMapPitch();

				int x, y = 0;

				for (y = 0; y < pForm->getSizeVertical() ; y++)
				{
					for (x = 0; x < pForm->getSizeHorizontal() ; x++ )
					{
						Debug::trace("RendererManager.Invert", "Ready to Flip bits on/off");

						UCHAR w = buffer[ ( ( pForm->getOriginVertical() + y ) * ( bmPitch * 2 ) ) + ( ( pForm->getOriginHorizontal() + x ) / 8 ) ];
						UCHAR bit = 0x80 >> ( ( pForm->getOriginHorizontal() + x ) % 8 );

						if (w & bit)
						{
							Debug::trace("RendererManager.Invert", "Ready to Flip Bit OFF");

							//------------------
							// On -> Flip to Off
							//------------------

							w &= ~(bit);
						}
						else
						{
							Debug::trace("RendererManager.Invert", "Ready to Flip Bit ON");

							//------------------
							// Off -> Flip to On
							//------------------
							w |= bit;
						}

						buffer[((pForm->getOriginVertical() + y) * (bmPitch * 2)) + ((pForm->getOriginHorizontal() + x) / 8)] = w;
					}
				}
			}

			// SOF-451
			if (( pForm->isFormFlagBarCodeEntity() ) && ( barcodeData.empty() ))
			{
				barcodeType = pForm->getFieldSubType();

				if ( !pForm->isDataPresent() )
					Debug::trace( "RendererManager.Render", "No barcode data present " + util_string::inttostring( barcodeType ) );
				else if (( UPCA == barcodeType ) || ( UPCE == barcodeType ) || ( EAN13 == barcodeType ))
				{
					barcodeData = (char*) pForm->getData();

					// SOF-927 as per Digimarc, they do not want expanded fresh on UPCA and EAN13 barcodes
					digimarcExpandedFreshEnabled = false;
				}
				else if (( RSS14E == barcodeType ) || ( RSS14E_GTIN_13 == barcodeType ) || ( RSS14E_GTIN_12 == barcodeType ) || ( RSS14E_GTIN_8 == barcodeType ) || (GS1_DATABAR_EXPANDED == barcodeType))
				{
					barcodeDataRss = barcodeData = (char*) pForm->getData();

					// extract the 14 digit code
					if (( barcodeData.length() < 18 ) || ( "(01)" != barcodeData.substr( 0, 4 ) ))
					{
						Debug::trace( "RendererManager.Render", "BAD RSS14 Code " + barcodeData );
						barcodeData.clear();
					}
					else
					{
						barcodeData = barcodeData.substr( 4, 14 );

						// Digimarc cannot encode extra information with a GTIN14 code... so if first digit
						// is '0' lets pretend we have an EAN13
						if ( '0' == barcodeData[0] )
						{
							barcodeData = barcodeData.substr( 1 );
							barcodeType = EAN13;
						}
					}
				}
			}

			// SOF-1245
			if (( digimarcExpandedFreshEnabled ) && ( pForm->isFormFlagDigimarcKeepOut() ))
			{
				const UCHAR *buffer( (UCHAR*) pBitMap->getBitMapBase() );
				const int pitch( pBitMap->getBitMapPitch() );;

				// check for empty lines bottom up
				for ( int y( pForm->getSizeVertical() ); y--; )
				{
					bool hasData( false );

					for ( int x( pForm->getSizeHorizontal() ); x--; )
					{
						const UCHAR w( buffer[ ( ( pForm->getOriginVertical() + y ) * ( pitch * 2 ) ) + ( ( pForm->getOriginHorizontal() + x ) / 8 ) ] );
						const UCHAR bit( 0x80 >> ( ( pForm->getOriginHorizontal() + x ) % 8 ) );

						if ( w & bit )
						{
							hasData = true;
							break;
						}
					}

					if ( hasData )
					{
						Hobart::Rectangle r( pForm->getOriginVertical(), pForm->getOriginVertical(), pForm->getOriginHorizontal(), pForm->getOriginHorizontal() );
#if defined( __linux__)
						r.setRectangleSize( pForm->getSizeHorizontal(), std::min( y + 4, (int) pForm->getSizeVertical() ) );
#else
						r.setRectangleSize(pForm->getSizeHorizontal(), min(y + 4, (int)pForm->getSizeVertical()));
#endif
						digimarcKeepOut.push_back( r );
						break;
					}
				}
			}
        }
		else
		{
			Debug::trace("RendererManager.Render", "Form not rendered during " +
					std::string(pForm->isFormPreImage() ? "post-image" : "pre-image"));
		}
	}

	// when rendering entire label we need to preimage and postimage
	if ( renderEntireLabel )
	{
		renderEntireLabel = false;

		UINT16 flags( ic->getImageFlags() );
		flags &= ~PreImage;
		flags |= PostImage;

		const_cast<ImageControl*>( ic )->setImageFlags( flags );

		goto renderLabelLoop;
	}

	// ---- //

	// POST IMAGE
	if (( ic ) && ( ic->getImageFlags() & PostImage ))
	{

#ifdef __linux__

		// SOF-451: Digimarc
		if (( ic->isDigimarcEnabled() ) && ( !barcodeData.empty() ))
		{
			DSM_API hSDKHandle( NULL );							// SDK handle

			// Create the primary SDK objects
			DSM_RESULT sdkResult( DSM_OK );						// Common way to catch results from various calls to the SDK
			DSM_DATADESC sdkImageDesc;							// An instance of image data descriptions
			DSM_EMBEDDER_OPTIONS sdkEmbedderOptions;			// An instance of embedder options

			Debug::trace( "RendererManager.Render", "Digimarc BEGIN" );

			// Initialize Embedder Options
			if ( DSM_OK != (sdkResult = DSM_InitializeEmbedderOptions( &sdkEmbedderOptions )) )
			{
				Debug::trace( "RendererManager.Render", "Digimarc error DSM_InitializeEmbedderOptions() = " +
					util_string::inttostring( sdkResult ) );
			}
			// Initialize the SDK handle
			else if ( DSM_OK != (sdkResult = DSM_Open( &hSDKHandle )) )
			{
				Debug::trace( "RendererManager.Render", "Digimarc error DSM_Open() = " +
					util_string::inttostring( sdkResult ) );
			}
			else
			{
				BitMap* pBitMap( ic->getImageBitMap() );

				// Pointer to a buffer that holds the watermark image data
				const unsigned int imageDataLen( pBitMap->getBitMapBaseSize() * 2 );

				uint8_t *imageData( new uint8_t[imageDataLen] );

				memcpy( imageData, pBitMap->getBitMapBase(), imageDataLen );

				// Digimarc SDK expects bits to be 0=black, 1=white so lets invert some bits...
				for ( unsigned int i( imageDataLen ); i--; )
					imageData[i] ^= 0xff;

				// Assign user specified data to image descriptions
				sdkImageDesc.BytesPerRow = pBitMap->getBitMapPitch() * 2;
				sdkImageDesc.DataDescType = DSM_DATADESC_ONE_BPP;
				sdkImageDesc.Height = pBitMap->getBitMapBoundsBottom() - pBitMap->getBitMapBoundsTop();
				sdkImageDesc.pData = imageData;
				sdkImageDesc.Resolution = 203; // print head dpi: 8 dots/mm
				sdkImageDesc.Width = pBitMap->getBitMapBoundsRight() - pBitMap->getBitMapBoundsLeft();

				// Determine dot density and watermark resolution
				//
				// Digimarc recommends the following for 203 DPI printer
				// WPI (resolution):  75    150
				// dot density:       40    35
				// dot size:          1     1
				unsigned short dotDensity( Configuration::getInstance()->toIntCfg( fieldOffsetCfgDigimarcDotDensity ) );
				unsigned short dotSize( Configuration::getInstance()->toIntCfg( fieldOffsetCfgDigimarcDotSize ) );
				unsigned short watermarkRes( Configuration::getInstance()->toIntCfg( fieldOffsetCfgDigimarcWatermarkRes ) );

				// valid range: 10 <= dot density <= 100
				if (( dotDensity < 10 ) || ( 100 < dotDensity ))
				{
					Debug::trace( "RendererManager.Render", "Digmarc dot density invalid " + util_string::inttostring( dotDensity ) );
					dotDensity = 40;
				}

				// valid range: printer_dpi/8 <= WPI <= printer_dpi
				if (( watermarkRes < 25 ) || ( 203 < watermarkRes ))
				{
					Debug::trace( "RendererManager.Render", "Digmarc watermark res invalid " + util_string::inttostring( watermarkRes ) );
					watermarkRes = 67;
				}

				unsigned short maxDotSize( 203 / watermarkRes );

				if ( maxDotSize <= 0 )
					maxDotSize = 1;

				// valid range: less than or equal to ratio of WPI to printer_dpi
				if (( dotSize <= 0 ) || ( maxDotSize < dotSize ))
				{
					Debug::trace( "RendererManager.Render", "Digmarc dot size invalid " + util_string::inttostring( dotSize ) );
					dotSize = 1;
				}

				// Assign user specified options to embedder options
				sdkEmbedderOptions.nDotDensity = dotDensity;
				sdkEmbedderOptions.nDotSize = dotSize;
				sdkEmbedderOptions.nWatermarkRes = watermarkRes;
				sdkEmbedderOptions.eDotPattern = DSM_DOT_PATTERN_CIRCULAR;
				sdkEmbedderOptions.eImageBlendMode = DSM_IMAGE_BLEND_MODE_DILATE;

				// Buffer to store the converted payload
		        char convertedPayload[MAX_PAYLOAD_OUT_BUFFER] = {0};

				if ( UPCE == barcodeType )
				{
					Debug::trace( "RendererManager.Render", "Digmarc embedding UPCE " + barcodeData );

					sdkResult = DSM_GetUpceJson( barcodeData.c_str(), convertedPayload );
				}
				else if (( UPCA == barcodeType ) || ( EAN13 == barcodeType ))
				{
					while ( barcodeData.length() < 13 )
						barcodeData.insert( 0, "0" );

					if ( digimarcExpandedFreshEnabled )
					{
						std::string sellByDate( util_string::inttostring( ic->getDigimarcSellByDate() ) );
						std::string price( util_string::inttostring( ic->getDigimarcPrice() ) );
						std::string weightLb( util_string::inttostring( ic->getDigimarcWeightLb() ) );
						std::string weightKg( util_string::inttostring( ic->getDigimarcWeightKg() ) );
						std::string count( util_string::inttostring( ic->getDigimarcCount() ) );

						// check rss encoding
						if ( barcodeDataRss.length() )
						{
							// make sure sell by date encoded
							if ( std::string::npos == barcodeDataRss.find( "(16)" ) )
								sellByDate.clear();

							// make sure price encoded
							if ( std::string::npos == barcodeDataRss.find( "(3922)" ) )
								price.clear();

							// make sure weight encoded
							if (( std::string::npos == barcodeDataRss.find( "(3103)" ) ) &&
								( std::string::npos == barcodeDataRss.find( "(3202)" ) ) &&
								( std::string::npos == barcodeDataRss.find( "(3203)" ) ))
							{
								weightLb.clear();
								weightKg.clear();
							}

							// make sure count encoded
							if ( std::string::npos == barcodeDataRss.find( "(30)" ) )
								count.clear();
						}

						const bool digimarcSellByDate( ( ic->getDigimarcSellByDate() ) && ( 6 == sellByDate.length() ) );
						const bool digimarcPrice( ic->getDigimarcPrice() ? true : false );
						const bool digimarcWeightLb( ic->getDigimarcWeightLb() ? true : false );
						const bool digimarcWeightKg( ic->getDigimarcWeightKg() ? true : false );
						const bool digimarcCount( ic->getDigimarcCount() ? true : false );

						Debug::trace( "RendererManager.Render", "Digmarc embedding GTIN13:\n" +
							barcodeDataRss + "\n" +
							barcodeData + "\n" +
							"  " + (digimarcSellByDate ? "* " : "  ") + sellByDate + "\n" +
							"  " + (digimarcPrice ? "* " : "  ") + price + "\n" +
							"  " + (digimarcWeightLb ? "* " : "  ") + weightLb + "\n" +
							"  " + (digimarcWeightKg ? "* " : "  ") + weightKg + "\n" +
							"  " + (digimarcCount ? "* " : "  ") + count );

						sdkResult = DSM_GetExpandedFreshJson(
							barcodeData.c_str(),
							digimarcSellByDate ? sellByDate.c_str() : NULL,
							digimarcPrice ? price.c_str() : NULL,
							digimarcWeightLb ? weightLb.c_str() : NULL,
							digimarcWeightKg ? weightKg.c_str() : NULL,
							digimarcCount ? count.c_str() : NULL,
							convertedPayload );
					}
					else
					{
						while ( barcodeData.length() < 14 )
							barcodeData.insert( 0, "0" );

						Debug::trace( "RendererManager.Render", "Digmarc embedding GTIN13 " + barcodeData );

						sdkResult = DSM_GetGtinJson(
							barcodeData.c_str(),
							convertedPayload );
					}
				}
				else if (( RSS14E == barcodeType ) || ( RSS14E_GTIN_13 == barcodeType ) || ( RSS14E_GTIN_12 == barcodeType ) || ( RSS14E_GTIN_8 == barcodeType ) || (GS1_DATABAR_EXPANDED == barcodeType))
				{
					Debug::trace( "RendererManager.Render", "Digmarc embedding GTIN14 " + barcodeData );

					sdkResult = DSM_GetGtinJson(
						barcodeData.c_str(),
						convertedPayload );
				}

				// If payload has been properly converted, then set the payload in embedder options
				if ( DSM_OK != sdkResult )
					Debug::trace( "RendererManager.Render", "Digimarc error DSM_GetJson() = " +
						util_string::inttostring( sdkResult ) );
				else
				{
					DSM_KEEP_OUT_REGION *pKeepOutRegionArray( NULL );

					// Set the payload in embedder options
					sdkEmbedderOptions.pPayload = convertedPayload;

					// Setup embedder keep out regions
					if ( digimarcKeepOut.size() )
					{
						Debug::trace( "RendererManager.Render", "Digimarc keep out regions = " + util_string::inttostring( digimarcKeepOut.size() ) );

						sdkEmbedderOptions.nNumKeepOutRegions = digimarcKeepOut.size();
						sdkEmbedderOptions.pKeepOutRegionArray = pKeepOutRegionArray = new DSM_KEEP_OUT_REGION[digimarcKeepOut.size()];

						for ( int i( 0 ); i < sdkEmbedderOptions.nNumKeepOutRegions; ++i )
						{
							pKeepOutRegionArray[i].TopLeftX = digimarcKeepOut[i].getRectangleLeft();
							pKeepOutRegionArray[i].TopLeftY = digimarcKeepOut[i].getRectangleTop();
							pKeepOutRegionArray[i].Width = digimarcKeepOut[i].getRectangleWidth();
							pKeepOutRegionArray[i].Height = digimarcKeepOut[i].getRectangleHeight();
						}
					}

					if ( Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgISaveLabel) )
						writeImageToPGMFile( "/tmp/before.pgm", sdkImageDesc.Height, sdkImageDesc.Width, sdkImageDesc.BytesPerRow, imageData );

					// Embed watermark
					if ( DSM_OK != (sdkResult = DSM_EmbedSparseMark( hSDKHandle, &sdkImageDesc, &sdkEmbedderOptions )) )
					{
						Debug::trace( "RendererManager.Render", "Digimarc error DSM_EmbedSparseMark() = " +
							util_string::inttostring( sdkResult ) );
					}
					else
					{
						Debug::trace( "RendererManager.Render", "Digimarc embed success" );

						if ( Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgISaveLabel) )
							writeImageToPGMFile( "/tmp/after.pgm", sdkImageDesc.Height, sdkImageDesc.Width, sdkImageDesc.BytesPerRow, imageData );

						// Restore bitmap bits from earlier flip...
						for ( unsigned int i( imageDataLen ); i--; )
							imageData[i] ^= 0xff;

						memcpy( pBitMap->getBitMapBase(), imageData, imageDataLen );
					}

					// Delete keep out regions
					if ( pKeepOutRegionArray )
						delete [] pKeepOutRegionArray;
				}

				// Delete image data
				delete [] imageData;

				// Close the SDK handle
				if ( DSM_OK != (sdkResult = DSM_Close( &hSDKHandle )) )
				{
					Debug::trace( "RendererManager.Render", "Digimarc error DSM_Close() = " +
						util_string::inttostring( sdkResult ) );
				}
			}

			Debug::trace( "RendererManager.Render", "Digimarc END" );
		}

#endif
		// ============
		// CODE CHECKER
		// ============
#ifdef CODE_CHECKER_BLITING
		if (( marginalBits ) && ( 0 <= printerType ))
		{
			// SOF-3204: track dots printed and removed
			std::vector<int> pixelsPrinted; // number of pixels printed, per printer head dot
			std::vector<int> pixelsRemoved; // number of dots removed from label, per printer head dot

			BitMap* pBitMap( ic->getImageBitMap() );

			// Assign user specified data to image descriptions
			const INT16 Height( pBitMap->getBitMapBoundsBottom() - pBitMap->getBitMapBoundsTop() );
			const INT16 Width( pBitMap->getBitMapBoundsRight() - pBitMap->getBitMapBoundsLeft() );

			pixelsPrinted.resize( Width, 0 );
			pixelsRemoved.resize( Width, 0 );

			// Convert to grayscale
			uint8_t *base( (uint8_t*) pBitMap->getBitMapBase() );

			for ( INT16 y( 0 ); y < Height; ++y )
			{
				uint8_t *basep( &base[y * pBitMap->getBitMapPitch() * 2] );

				for ( INT16 x( 0 ); x < Width; x += 8 )
				{
					// all dots good, nothing to blit out
					if ( 0xFF == marginalBits[x / 8] )
						continue;

					uint8_t bits( basep[x / 8] );
					uint8_t mask( 1 );

					for ( INT16 i( 8 ); i--; )
					{
						// check bit set
						if ( bits & mask )
						{
							// when marginalBits is 0 dot is bad, when marginalBits is 1 dot is okay
							bool removeFlag( (marginalBits[x / 8] & mask) ? false : true );

							// when pixel falls within boundary of a barcode or masked form *DO NOT* remove it
							if ( removeFlag )
							{
								for ( std::vector<Hobart::Rectangle>::const_iterator r( codeCheckerDisabled.begin() ); r != codeCheckerDisabled.end(); ++r )
									if (( y >= r->getRectangleTop() ) && ( y <= r->getRectangleBottom() ) &&
										( x+i >= r->getRectangleLeft() ) && ( x+i <= r->getRectangleRight() ))
									{
										Debug::trace("CodeChecker", "Point In disabled field: (" + Integer::toString(x+i) + " , " + Integer::toString(y) + ")");
										removeFlag = false;
										break;
									}
							}

							if ( removeFlag )
							{
								bits &= ~mask;
								++pixelsRemoved[x+i];
							}
							else
							{
								++pixelsPrinted[x+i];
							}
						}

						mask <<= 1;
					}

					Debug::trace("CodeChecker", "Marginal bit Index: " + Integer::toString(x / 8) + " Value: " + Integer::toString(marginalBits[x / 8]));
					basep[x / 8] = bits;
				}
			}

			// SOF-3204
			// send statistics to diagnostics manager
			if ( Configuration::getInstance()->isLabelFeedHeelFirst() )
			{
				std::reverse( pixelsPrinted.begin(), pixelsPrinted.end() );
				std::reverse( pixelsRemoved.begin(), pixelsRemoved.end() );
			}

			DiagnosticsPrintLabelStats stats;
			stats.printerType_ = printerType;

			for ( const int& i : pixelsPrinted )
				stats.pixelsPrinted_.append( std::string( (const char*) &i, sizeof(int) ) );

			for ( const int& i : pixelsRemoved )
				stats.pixelsRemoved_.append( std::string( (const char*) &i, sizeof(int) ) );

			dispatchEvent( &stats );
		}
#endif

		// Save Label
		if (ic->getImageFlags() & PostImage)
		{
			std::string filenameString;
			if (!ic->getSaveToFilenameString().empty())
				filenameString = ic->getSaveToFilenameString();
			else if (Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgISaveLabel))
			{
				static int count = 0;

#if defined(PAL_WIN32_RUN)
				filenameString = string("C:\\Program Files\\Hobart\\LabelImageR");
#else
				filenameString = string("/etc/hobart/LabelImageR");
#endif
				filenameString.append(inttostring(++count)).append(".bmp");
			}
			if (!filenameString.empty())
			{
				Debug::trace("RendererManager.saveLabel", "RendererManager::renderLabel - before saving label to file: " + filenameString);

				Hobart::CRendererUtil::SaveBitmapToFile(ic->getImageBitMap(), filenameString);

				Debug::trace("RendererManager.saveLabel", "RendererManager::renderLabel - back from saving label to file: " + filenameString);
			}
		}
	}

	Debug::trace("RendererManager.Render", "Completed rendering label for imageControlBlock " + util_string::ptrtostring(ic));
}


void RendererManager::handleCloseSession(const char *buffer)
{
	RendererCloseSession rendererCloseSession(buffer);
	UINT16 sessionId = rendererCloseSession.getSessionId();

	if ( rendererMap.find(sessionId) != rendererMap.end() )
	{
		Debug::trace("RendererManager.CloseSession",
				"Session id: " + util_string::uinttostring(sessionId) + "Closed" );

		//remove rendering session
		RendererDataLocation *pDataLocation = rendererMap[sessionId];
		rendererMap.erase(sessionId);		

		delete pDataLocation;
	}
	else
	{
		Debug::trace("RendererManager.Error", "CloseSession -- Session id not found: " +
				util_string::uinttostring(sessionId));
	}
}

void RendererManager::handleInitDotHeadWear(const char *buffer)
{
	if ( marginalBits )
		return;

	RendererInitDotHeadWear init( buffer );
	Debug::trace("CodeChecker", "Got Init. Printhead Size: " + Integer::toString(init.getPrintheadSize()));

	// all dots okay
	marginalBits = new uint8_t[printheadSize = init.getPrintheadSize()];
	memset( marginalBits, 0xFF, printheadSize );
}

void RendererManager::handleNewDotHeadWear(const char *buffer)
{
	if ( !marginalBits )
		Debug::trace("CodeChecker", "Need Init Dot Head Wear First");
	else
	{
		RendererNewDotHeadWear wear( buffer );
		Debug::trace("CodeChecker", "Got New Wear Values. Bit Buffer Size: " + Integer::toString(printheadSize));

		wear.show(); 

		std::string str;
		for ( int i = 0; i < printheadSize; i++ )
		{
			marginalBits[i] = wear.sourceAt(i);
			str.append(Hex::toString(marginalBits[i]));
		}

		Debug::trace("CodeChecker", "marginalBits: " + str);
	}
}

bool RendererManager::writeImageToPGMFile(const char *Filename, int height, int width, int stride, const unsigned char *base)
{
	std::ofstream pgmFile( Filename, std::ios_base::out | std::ios_base::binary );

	if ( !pgmFile.is_open() )
		return false;

	pgmFile << "P5" << std::endl;
	pgmFile << width << " " << height << std::endl;
	pgmFile << "255" << std::endl;

	// convert to greyscale
	uint8_t *ImageData( new uint8_t[8 * stride * height] );

	for ( int y( 0 ); y < height; ++y )
	{
		const unsigned char *basep( &base[y * stride] );

		for ( int x( 0 ); x < width; x += 8 )
		{
			uint8_t bits( basep[x / 8] );

			for ( int i( 0 ); i < 8; ++i )
			{
				ImageData[(y * 8 * stride) + x + i] = (0x80 & bits ? 0xff : 0x00);
				bits <<= 1;
			}
		}
	}

	for ( int row = 0; row < height; row++)
		pgmFile.write( reinterpret_cast<const char*>(ImageData + row * 8 * stride), width );

	delete [] ImageData;

	pgmFile.close();
	return true;
}

