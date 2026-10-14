#include "ListCameraSystemPredictions.h"

#include "evolution.h"
#include "HobTokenizerCSV.h"
#include "MessageFormat.h"
#include "Bool.h"
#include "Debug.h"

#include "util/e_string/utilstring.h"

namespace hfc
{
	ListCameraSystemPredictionsMessage::ListCameraSystemPredictionsMessage() :
		result(-1)
	{
	}

	ListCameraSystemPredictionsMessage::ListCameraSystemPredictionsMessage(const char* data, int len, int formatType) :
		result(-1)
	{
		switch ( formatType )
		{
		case InternalFormat:
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
				case fieldOffsetResult:
					setResult(util_string::stringtoint(token));
					break;
				case fieldOffsetImage:
					setImage(token);
					break;
				case fieldOffsetProducts:
					token.unQuoteForCSV();
					deserialize(token, products);
					break;
				case fieldOffsetConfidenceLevels:
					token.unQuoteForCSV();
					deserialize(token, confidenceLevels);
					break;
				default:
					break;
				}

				++loopCnt;
			}

			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
		}
	}

	const char *ListCameraSystemPredictionsMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
		case InternalFormat:
		case BrowserFormat:
			serializedData.append(util_string::inttostring(result));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(image);
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(products.serialize(BrowserFormat));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(confidenceLevels.serialize(BrowserFormat));
			break;

		default:
			serializedData = "ReadTransactionGroupMessage::serialize - unknown format: " + util_string::inttostring(format);
			// todo - throw exception("invalid format");
			break;
		}

		return serializedData.data();
	}

	void ListCameraSystemPredictionsMessage::deserialize(const HobString& list, HobList& result)
	{
		HobTokenizerCSV tokenizer(&list, BrowserFormatSeparator);  //use the CSV Tokenizer to parse the string
		HobString token;

		while (tokenizer.next(&token) != -1)
		{
			token.unQuoteForCSV();
			result.push_back(token);
		}
	}

}
