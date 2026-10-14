#include "AbstractReport.h"
#include "Configuration.h"

#include "ReportManager.h"
#include "Label.h"
#include "Product.h"
#include "LabelType.h"
#include "LabelStockDescriptor.h"
#include "FormRecord.h"
#include "ResourceHeader.h"
#include "HobartLabel.h"
#include "ProductTransaction.h"

#include "Event.h"
#include "PrinterEvent.h"
#include "PrinterLabelComplete.h"
#include "PrinterRequestStockMetrics.h"
#include "PrinterPreImage.h"
#include "PrinterPostImage.h"

#include "e_string/utilstring.h"
#include "CEvolMsgReceiver.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AbstractReport::dataType = 0;
std::string AbstractReport::dataTypeName("AbstractReport"); 
#endif

AbstractReport::AbstractReport(ReportManager* reportManager, PrinterType type)
		: reportManager_(reportManager), contentReady_(false), printingReady_(false),
		canceled_(false), printerType_(type), stockWidth_(WideLabel), stockLength_(0),
		reportForm_(0), fakeLabel_(0), fakeProduct_(0), labelComplete_(true),
		reportComplete_(false), metricsChanged_(false) 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, 
		"Constructing an instance of AbstractReport for printer " + util_string::inttostring(printerType_));
#endif
}


AbstractReport::~AbstractReport()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Destroying the AbstractReport");
#endif

	reportManager_->deregisterEvent(Printer, PrStockMetrics);
	reportManager_->deregisterEvent(Printer, PrLabelComplete);

	// We do not delete the reportForm_ because it is part of the label and the
	// label that contains it destroys it when itself goes away.

	// For the same reason we do not delete the fakeLabel_ because it is part of
	// the product that contains it, which will destroy it when itself is destroyed

//	Do not delete fakeProduct_ as it becomes a SmartPtr in createFakeProduct()
//	if ( fakeProduct_ )
//		delete fakeProduct_;

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Destructor:  Done");
#endif
}


void AbstractReport::initialize()
{
//#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Initializing the AbstractReport");
//#endif

	reportManager_->registerEvent(Printer, PrStockMetrics);
	reportManager_->registerEvent(Printer, PrLabelComplete);


	doInitialize();


	// Generate a PrinterRequestStockMetrics and send it to the PrinterManager
//#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, 
		"Sending request for stock metrics for printer " + util_string::inttostring(printerType_));
//#endif

	Util::CSmartPtr< EvolMessage> message ( new PrinterRequestStockMetrics(reportManager_->getMessageReceiver(), printerType_));
	reportManager_->dispatchEvent(message);
}


void AbstractReport::setContent(const char* content)
{
	content_.assign(content);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, 
			"Received content of report as:\n" + content_);
	Debug::trace("AbstractReport", 20, 
		"Size of content is " + util_string::inttostring(content_.size()));
#endif
}


void AbstractReport::setContent(const std::string& content)
{
	content_ = content;

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, 
			"Received content of report as:\n" + content_);
	Debug::trace("AbstractReport", 20, 
		"Size of content is " + util_string::inttostring(content_.size()));
#endif
}


void AbstractReport::appendContent(const std::string& content)
{
	content_.append(content);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, 
			"Received additional content for report as:\n" + content);
	Debug::trace("AbstractReport", 20, 
			"New content of report is:\n" + content_);
	Debug::trace("AbstractReport", 20, 
		"Size of content is " + util_string::inttostring(content_.size()));
#endif
}


void AbstractReport::handleMessage(const char* buffer)
{
	if ( !buffer )
		return;

	EvolMessage evolMessage(buffer);
	switch ( evolMessage.getDeviceType() )
	{
		case Printer:
		{
			handlePrinterMessage(evolMessage.getEventType(), buffer);

			break;
		}
		case Wrapper:
		{
			doHandleWrapperMessage(evolMessage.getEventType(), buffer);

			break;
		}
		case WrapperSystem:
		{
			doHandleWrapperSystemMessage(evolMessage.getEventType(), buffer);

			break;
		}
		default:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("AbstractReport", 10, "Received an unknown message (" +
				util_string::inttostring(evolMessage.getDeviceType()) + "," +
				util_string::inttostring(evolMessage.getEventType()) + "). Ignoring");
#endif

			break;
		}
	}
}


void AbstractReport::handlePrinterMessage(UINT8 event, const char* buffer)
{
#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Received Printer message");
#endif

	switch ( event )
	{
		case PrStockMetrics:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("AbstractReport", 20, "Message: Printer - PrinterStockMetrics");
#endif

			if ( !isCanceled() )
			{
				PrinterStockMetrics metrics(buffer);
				if ( metrics.getPrinterType() == printerType_ )
				{
					// If width and length are non-zero and at least one
					// of them is different from the current values ...
					if ( metrics.getLabelLengthInLines() &&
							(stockWidth_  != metrics.getLabelWidth() ||
							 stockLength_ != metrics.getLabelLengthInLines()) )
					{
						metrics_ = metrics;
						metricsChanged_ = true;

						if ( !printingReady_ )
						{
#ifndef EVOL_NDEBUG
							Debug::trace("AbstractReport", 20, 
									"Received stock metrics. Ready to Print.");
#endif

							printingReady_ = true;
						}
						else
						{
#ifndef EVOL_NDEBUG
							Debug::trace("AbstractReport", 20, "Received new stock metrics.");
#endif
						}
					}
#ifndef EVOL_NDEBUG
					else
					{
						Debug::trace("AbstractReport", 20,
								"Metrics contain no stock or same stock information (stockWidth = " +
								util_string::inttostring(metrics.getLabelWidth()) + ", stockLength = " +
								util_string::inttostring(metrics.getLabelLengthInLines()) + "). Ignoring message.");
					}
#endif
				}
#ifndef EVOL_NDEBUG
				else
					Debug::trace("AbstractReport", 20,
							"Message applies to different printer. Dropping message.");
#endif
			}
#ifndef EVOL_NDEBUG
			else
				Debug::trace("AbstractReport", 20, "Report was cancelled. Dropping message.");
#endif

			break;
		}
		case PrLabelComplete:
		{
#ifndef EVOL_NDEBUG
			Debug::trace("AbstractReport", 20, "Message: Printer - PrinterLabelComplete");
#endif

			if ( contentReady_ && printingReady_ )
			{
				PrinterLabelComplete labelComplete(buffer);
				const ProductTransaction* transaction = labelComplete.getProductTransaction();
				if ( fakeProduct_ && transaction == fakeProduct_->getCurrentTransaction() )
				{
					int printedPage = fakeProduct_->getCurrentTransaction()->getReportPageNumber();

					if ( labelComplete.isPrintOk() )
					{
#ifndef EVOL_NDEBUG
						Debug::trace("AbstractReport", 20, "Received label complete message: label " +
							util_string::inttostring(printedPage) + " printed.");
#endif
					}
					else
					{
#ifndef EVOL_NDEBUG
						Debug::trace("AbstractReport", 10, "Received label complete message: label " +
							util_string::inttostring(printedPage) + " did not print.");
#endif
					}

					fakeProduct_->getCurrentTransaction()->setReportPageNumber(printedPage + 1);

					// FT-849: It is also possible the entire stream was printed in one shot (i.e.
					// continuous stock)... in this case mark report complete too. Here we check
					// how many characters we printed versus how many characters there are total.
					size_t printedLen( 0 );

					if (( reportForm_->getStream() ) && ( content_.c_str() <= (const char*) reportForm_->getStream() ))
						printedLen = ((const char*) reportForm_->getStream()) - content_.c_str();

					if (( '\0' == *reportForm_->getStream() ) || ( content_.length() <= printedLen ))
					{
#ifndef EVOL_NDEBUG
						Debug::trace("AbstractReport", 20,
								"The last label was completed. The report is complete");
#endif

						reportComplete_ = true;
					}

					// FT-849: Moved label complete to be after report complete otherwise there is a window of time in
					// which they can rip the label causing label complete but not report complete and another label
					// will print.
					labelComplete_ = true;

                #if defined(LINUX_FIX)
					PAL_absclass::PAL_sleep( 40);
                #else
					Sleep( 40);
                #endif
				}
#ifndef EVOL_NDEBUG
				else
					Debug::trace("AbstractReport", 10, 
							"The label is associated with a different transaction. Ignoring.");
#endif
			}
#ifndef EVOL_NDEBUG
			else
				Debug::trace("AbstractReport", 10, 
						"The report is not initialized so this message must not be for us. Ignoring.");
#endif
				
			break;
		}
	}

	doHandlePrinterMessage(event, buffer);
}


bool AbstractReport::doReport()
{
	if ( contentReady_ && printingReady_ )
	{
		if ( !canceled_ )
		{
			if ( !reportComplete_ || !labelComplete_ )
			{
				if ( labelComplete_ )
				{
					printLabel();
					labelComplete_ = false;
				}

				return false;
			}
			else
			{
#ifndef EVOL_NDEBUG
				Debug::trace("AbstractReport", 20, 
						"The last label was issued and printed. Report complete");
#endif

				// We only return true (complete) if the last label has been issued
				// and it has completed printing
				return true;
			}
		}

#ifndef EVOL_NDEBUG
		Debug::trace("AbstractReport", 20, 
				"The report was canceled. Report complete");
#endif

		return true;
	}
	else
		return false;
}


void AbstractReport::cancelReport()
{
#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "The report has been canceled.");
#endif

	setCanceled(true);
}



void AbstractReport::updateLabelAndProduct()
{
	stockWidth_ = metrics_.getLabelWidth();
	stockLength_ = metrics_.getLabelLengthInLines();
	if ( stockLength_ > 2000 )
		stockLength_ *= 5;

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Label Width is " + util_string::inttostring(stockWidth_) +
		" and label length is " + util_string::inttostring(stockLength_));
#endif

	createFakeLabel();
	createFakeProduct();
}


void AbstractReport::createFakeLabel()
{
#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Creating fake label for report");
#endif

	// Create the label resource header
	ResourceHeader header;

	header.setResourceType("LABL");
	header.setResourceId(-1);
	header.setResourceLoaded(true);
	header.setResourceLength(0);
	header.setResourceName("Report Label");

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Created ResourceHeader");
#endif


	// Create the label stock descriptor
	LabelStockDescriptor lsd;

	lsd.setWidth(stockWidth_);
	lsd.setLength(stockLength_);
	lsd.setLabelOrientation(PortraitLabel);
	lsd.setSkew(0);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Created LabelStockDescriptor (width = " +
		util_string::inttostring(lsd.getWidthDots()) + ", length = " + util_string::inttostring(lsd.getLength()) + ")");
#endif


	// Create the forms list and the label forms. These label forms don't need to
	// be deleted because they are referenced by the forms list. This forms list
	// is copied (pointers duplicated) by the HobartLabel below, which when deleted
	// disposes of the forms pointed to by the elements of the list.
	UINT16 labelWidth = lsd.getWidthDots();
	UINT16 leftMargin = 13, rightMargin = 13;
	UINT16 topMargin = 13, bottomMargin = 13;
	UINT16 horiSeparation = 10, vertSeparation = 15;
	UINT16 numberWidth = 70, titleHeight = 25; 
	HobartLabel::FormsListType list;

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Created empty FormList");
#endif

	//	If the Operator has set up offsets to avoid printing over preprinted label info
	//	at the top and/or bottom of the label, use those offsets for the margins, as long as
	//	the offsets leave us with some printable space.
	if ((stockLength_ - Configuration::getInstance()->getReportsOffsetTop() - Configuration::getInstance()->getReportsOffsetBottom()) > 0)
	{
		if (Configuration::getInstance()->getReportsOffsetTop() > 0)
			topMargin = Configuration::getInstance()->getReportsOffsetTop();
		if (Configuration::getInstance()->getReportsOffsetBottom() > 0)
			bottomMargin = Configuration::getInstance()->getReportsOffsetBottom();
	}
#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "createFakeLabel():  stockLength_ = " + inttostring(stockLength_) +
			",  topMargin = " + inttostring(topMargin) + ",  bottomMargin = " + inttostring(bottomMargin));
#endif

	if ( lsd.getWidth() == WideLabel )
		horiSeparation = 8;

	// Title (Report's name)
	FormRecord* title = new FormRecord(leftMargin, topMargin, 
			labelWidth - (leftMargin + horiSeparation + numberWidth + rightMargin), titleHeight,
			Fixed | PreImage | TextEntity, CONSTANT_TEXT, LeftJustify, 0, 0, 0);
	title->setFamilyName((const UINT8 *)"Gravilex");
	title->setPointSize(7);
	title->setConstantData((const UINT8 *)getName().c_str());
	list.push_back(title);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Added title to FormList");
#endif

	// Page Number
	FormRecord* pageNumber = new FormRecord(labelWidth - (numberWidth + rightMargin), 
			topMargin, numberWidth, titleHeight,
			Fixed | PostImage | TextEntity, REPORT_PAGE_NUMBER, RightJustify, 0, 0, 0);
	pageNumber->setFamilyName((const UINT8 *)"Gravilex");
	pageNumber->setPointSize(7);
	list.push_back(pageNumber);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Added page number to FormList");
#endif

	// Content Stream
	UINT16 flag = (metrics_.isContinuousStock()) ? (Variable | PreImage | TextEntity) : (Fixed | PostImage | TextEntity);
	reportForm_ = new FormRecord(leftMargin, topMargin + titleHeight + vertSeparation, 
			labelWidth - (leftMargin + rightMargin), 
			stockLength_ - (topMargin + titleHeight + vertSeparation + bottomMargin),
			flag, REPORT_STREAM, LeftJustify, 0, 0, 0);
	reportForm_->setFamilyName((const UINT8 *)getContentFontFamily().c_str());
	reportForm_->setPointSize(getContentFontSize());
	reportForm_->setBitMapData((BitMap *)content_.c_str());   // This simply takes the pointer and keeps track of it
	list.push_back(reportForm_);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Added report stream to FormList");
#endif


	// Create the Hobart Label based on the header, label stock descriptor, 
	// and list of forms. There's no need to delete it since it is retained
	// and disposed of by the LabelType instance below.
	HobartLabel* hLabel = new HobartLabel(header, lsd, list);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Created HobartLabel (width = " +
		util_string::inttostring(hLabel->getLabelStockDescriptor()->getWidthDots()) +
		", length = " + util_string::inttostring(hLabel->getLabelStockDescriptor()->getLength()) + ")");
#endif


	// Now create the Label based on the Hobart Label already created
	if ( fakeLabel_ )
		delete fakeLabel_;

	fakeLabel_ = new LabelType((HobRecordNum)0);        // Assign it a zero number to not have a conflict
	fakeLabel_->setHobartLabel(hLabel);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Created the fake label");
#endif
#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Created fake label (width = " +
		util_string::inttostring(fakeLabel_->getWidthDots()) +
		", length = " + util_string::inttostring(fakeLabel_->getLength()) + ")");
#endif
}


void AbstractReport::createFakeProduct()
{
	int currentPage = 1;

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Creating fake product and product transaction");
#endif

	if ( !fakeProduct_ )
	{
		fakeProduct_ = new Product();
		fakeProductSmartPtr_ = fakeProduct_;
	}
	else
		currentPage = fakeProduct_->getCurrentTransaction()->getReportPageNumber();

//	fakeProduct_->setProductNum(-1);
	fakeProduct_->setLabelType(printerType_, fakeLabel_);
	fakeProduct_->getCurrentTransaction()->setReportPageNumber(currentPage);

#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport", 20, "Created and set the fake product (current page is " +
		util_string::inttostring(currentPage) + ")");
#endif
}


void AbstractReport::printLabel()
{
	if ( metricsChanged_ )
	{
#ifndef EVOL_NDEBUG
		Debug::trace("AbstractReport.print", 20, "There are new metrics. Applying values to label");
#endif

		updateLabelAndProduct();

#ifndef EVOL_NDEBUG
		Debug::trace("AbstractReport.print", 20, "Going to pre image new label");
#endif

		// Send the PreImage message
		PrinterPreImage preImage(reportManager_->getMessageReceiver(), printerType_, &fakeProductSmartPtr_);
		reportManager_->dispatchEvent(&preImage);

		metricsChanged_ = false;
	}

	// Send the PostImage message
#ifndef EVOL_NDEBUG
	Debug::trace("AbstractReport.print", 20, "Going to print label. Sending post image message");
#endif

	PrinterPostImage postImage(reportManager_->getMessageReceiver(), printerType_, fakeProduct_->getCurrentTransaction(), 0, &fakeProductSmartPtr_);
	reportManager_->dispatchEvent(&postImage);
}
