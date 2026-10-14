#include "ReadTransactionGroupMessage.h"


#include "HobTokenizerCSV.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int ReadTransactionGroupMessage::_dataType_ = 0;
		std::string ReadTransactionGroupMessage::_dataTypeName_("hfc::ReadTransactionGroupMessage");
	#endif



	ReadTransactionGroupMessage::ReadTransactionGroupMessage(const char * data, int len, int formatType)
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
						case fieldOffsetReadTransactionGroupPLUNumber:
						{
							setPLUNumber(token);

							break;
						}
						case fieldOffsetReadTransactionGroupOperator:
						{
							setOperatorID(token);

							break;
						}
						case fieldOffsetReadTransactionGroupStartDate:
						{
							token.unQuoteForCSV();

							setStartDateTime(token);

							break;
						}
						case fieldOffsetReadTransactionGroupEndDate:
						{
							token.unQuoteForCSV();

							setEndDateTime(token);

							break;
						}
						case fieldOffsetReadTransactionGroupReadingGroup:
						{
							setReadingGroup((ReadingGroup)util_string::stringtoint(token));

							break;
						}
						case fieldOffsetReadTransactionGroupMaxQuantity:
						{
							setMaxQuantity(util_string::stringtoint(token));

							break;
						}
						case fieldOffsetReadTransactionViewChangesWhere:
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


	ReadTransactionGroupMessage::~ReadTransactionGroupMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.ReadTransactionGroupMessage", "    ReadTransactionGroupMessage: " + util_string::ptrtostring(this));
	}


	void ReadTransactionGroupMessage::init()
	{
		pluNumber_.clear();
		operatorId_.clear();
		startDateTime_.clear();
		endDateTime_.clear();
		group_ = GroupLast;
		maxQuantity_ = 0;
	}


	const char *ReadTransactionGroupMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				for (int loopCnt = 1; loopCnt < endReadTransactionGroupOffsets; loopCnt++)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);

					switch ( loopCnt )
					{
						case fieldOffsetReadTransactionGroupReadingGroup:
							serializedData.append(util_string::inttostring(getReadingGroup()));
							break;

						case fieldOffsetReadTransactionGroupMaxQuantity:
							serializedData.append(util_string::inttostring(getMaxQuantity()));
							break;

						default: break;
					}
				}

				break;	
			}
			default:
			{
				serializedData = "ReadTransactionGroupMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}



}
