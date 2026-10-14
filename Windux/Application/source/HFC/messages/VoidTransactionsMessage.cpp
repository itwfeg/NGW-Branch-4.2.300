#include "VoidTransactionsMessage.h"


#include "HobTokenizerCSV.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int VoidTransactionsMessage::_dataType_ = 0;
		std::string VoidTransactionsMessage::_dataTypeName_("hfc::VoidTransactionsMessage");
	#endif



	VoidTransactionsMessage::VoidTransactionsMessage(const char * data, int len, int formatType)
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
					setReceived( loopCnt, token.length() > 0 );
						
					switch (loopCnt)
					{
						case fieldOffsetVoidTransactionsList:
						{
							token.unQuoteForCSV();

							ids_ = HobList(token.data(), (int)token.size(), formatType);

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


	VoidTransactionsMessage::~VoidTransactionsMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.VoidTransactionsMessage", "    VoidTransactionsMessage: " + util_string::ptrtostring(this));
	}


	void VoidTransactionsMessage::init()
	{
		// ids_;
	}


}
