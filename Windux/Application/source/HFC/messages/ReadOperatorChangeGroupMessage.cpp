#include "ReadOperatorChangeGroupMessage.h"


#include "HobTokenizerCSV.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int ReadOperatorChangeGroupMessage::_dataType_ = 0;
		std::string ReadOperatorChangeGroupMessage::_dataTypeName_("hfc::ReadOperatorChangeGroupMessage");
	#endif



	ReadOperatorChangeGroupMessage::ReadOperatorChangeGroupMessage(const char * data, int len, int formatType)
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
						case fieldOffsetPLUNumber:
						{
							setPLUNumber(token);

							break;
						}
						case fieldOffsetOperator:
						{
							setOperatorId(token);

							break;
						}
						case fieldOffsetTransactionType:
						{
							setTransactionType(token);

							break;
						}
						case fieldOffsetStartDate:
						{
							token.unQuoteForCSV();

							setStartDateTime(token);

							break;
						}
						case fieldOffsetEndDate:
						{
							token.unQuoteForCSV();

							setEndDateTime(token);

							break;
						}
						case fieldOffsetReadingGroup:
						{
							setReadingGroup((ReadingGroup)util_string::stringtoint(token));

							break;
						}
						case fieldOffsetMaxQuantity:
						{
							setMaxQuantity(util_string::stringtoint(token));

							break;
						}
						case fieldOffsetViewChangesWhere:
						{
							setViewChangesWhere(token);

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


	ReadOperatorChangeGroupMessage::~ReadOperatorChangeGroupMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

		Debug::trace("Delete.ReadOperatorChangeGroupMessage", "    ReadOperatorChangeGroupMessage: " + util_string::ptrtostring(this));
	}


	void ReadOperatorChangeGroupMessage::init()
	{
		pluNumber_.clear();
		operatorId_.clear();
		transactionType_.clear();
		startDateTime_.clear();
		endDateTime_.clear();
		group_ = GroupLast;
		maxQuantity_ = 0;
		viewChangesWhere_.clear();
	}


}
