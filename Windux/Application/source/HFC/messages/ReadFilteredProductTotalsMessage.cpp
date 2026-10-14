#include "ReadFilteredProductTotalsMessage.h"


#include "HobTokenizerCSV.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int ReadFilteredProductTotalsMessage::_dataType_ = 0;
		std::string ReadFilteredProductTotalsMessage::_dataTypeName_("hfc::ReadFilteredProductTotalsMessage");
	#endif



	ReadFilteredProductTotalsMessage::ReadFilteredProductTotalsMessage(const char * data, int len, int formatType)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();

		switch ( formatType )
		{
			case BrowserFormat:
			{
				HobString evolData(data, len);                                 //convert data to HobString
				HobTokenizerCSV tokenizer(&evolData, BrowserFormatSeparator);  //use the CSV Tokenizer to parse the string
				HobString token;
				short loopCnt = 1;

				while ( tokenizer.next(&token) != -1 )
				{
					setReceived( loopCnt, token.length() > 0);
						
					switch (loopCnt)
					{
						case fieldOffsetReadFilteredProductTotalsPLUNumber:
						{
							setPLUNumber(token);

							break;
						}
						case fieldOffsetReadFilteredProductTotalsTransactionType:
						{
							setTransactionType(token);

							break;
						}
						case fieldOffsetReadFilteredProductTotalsStartDate:
						{
							token.unQuoteForCSV();

							setStartDateTime(token);

							break;
						}
						case fieldOffsetReadFilteredProductTotalsEndDate:
						{
							token.unQuoteForCSV();

							setEndDateTime(token);

							break;
						}
						default: break;
					}

					++loopCnt;
				}

				break;
			}

			default:
			{
				// todo - throw exception("invalid format");
				break;
			}
		}
	}


	ReadFilteredProductTotalsMessage::~ReadFilteredProductTotalsMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.ReadFilteredProductTotalsMessage", "    ReadFilteredProductTotalsMessage: " + util_string::ptrtostring(this));
	}


	void ReadFilteredProductTotalsMessage::init()
	{
		pluNumber_.clear();
		transactionType_.clear();
		startDateTime_.clear();
		endDateTime_.clear();
	}



}
