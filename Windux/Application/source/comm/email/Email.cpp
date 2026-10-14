#include "Email.h"


const std::string Email::_AddressSeperator_(",");

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int Email::_dataType_ = 0;
	std::string Email::_dataTypeName_("Email"); 
#endif

Email::Email(const std::string& from, const std::string& to, const std::string& cc,
	const std::string& bcc, const std::string& subject, const std::string& content)
		: numRetries_(1), dateTime_(), from_(from), to_(to), toList_(), cc_(cc), ccList_(), bcc_(bcc), 
		bccList_(), subject_(subject), content_(content)
{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	parseDestList(toList_, to);
	parseDestList(ccList_, cc);
	parseDestList(bccList_, bcc);
}

Email::~Email()
{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	toList_.clear();
	ccList_.clear();
	bccList_.clear();
}

void Email::parseDestList(addressList& list, const std::string& commaSepList)
{
	std::string::size_type pos1, pos2;
	std::string temp;

	if (commaSepList.size() > 0)
	{
		pos1 = 0;
		pos2 = commaSepList.find(_AddressSeperator_, pos1);

		while (pos2!= std::string::npos)
		{
			std::string::size_type tempSize = (pos2 - pos1);
			if (tempSize > 0)
			{
				temp = commaSepList.substr(pos1, tempSize);
				removeSpaces(temp);
			
				if (temp.size() > 0)
					list.push_back(temp);
			}

			pos1 = pos2 + 1;

			if (pos1 >= commaSepList.size())
				pos2 = std::string::npos;
			else
				pos2 = commaSepList.find(_AddressSeperator_, pos1);

		}

		std::string::size_type finalSize = commaSepList.size() - pos1;
		if (finalSize > 0)
		{
			temp = commaSepList.substr(pos1, finalSize);
			removeSpaces(temp);

			if (temp.size() > 0)
				list.push_back(temp);
		}
	}
}

void Email::removeSpaces(std::string& emailAddr)
{
int sourceSize = (int)emailAddr.size();
std::string temp;
bool oneRemoved = false;

	for (int i = 0; i < sourceSize; i++)
	{
		if (emailAddr[i] != ' ')
			temp += emailAddr[i];
		else
			oneRemoved = true;

	}

	if (oneRemoved)
		emailAddr = temp;

}

void Email::createCommaSepList(addressList& list, std::string& commaSepList)
{
addressListIterator i;
std::string emailAddr;

	commaSepList = "";

	for (i = list.begin(); i != list.end(); ++i)
	{
		
		emailAddr = *i;

		if (emailAddr != "")
		{

			commaSepList += emailAddr;
			commaSepList += ",";

		}

	}

	if (commaSepList.size() > 0)
		commaSepList .erase(commaSepList .length() - 1, 1);

}

void Email::buildSMTPEmail( std::string& emailText)
{
	std::string newLine("\r\n");
	std::string startString;

	emailText = "Date:" + newLine;
	emailText += "From: ";
	emailText += from_;
	emailText += newLine;

	emailText += "To: ";
	emailText += to_;
	emailText += newLine;

	emailText += "cc: ";
	emailText += cc_;
	emailText += newLine;

	emailText += "Subject: ";
	emailText += subject_;
	emailText += newLine;

	emailText += newLine;

	{
		std::string::size_type pos1, pos2;
		std::string singleLine;

		pos1 = 0;
		pos2 = content_.find(newLine, pos1);

		while (pos2!= std::string::npos)
		{
			singleLine = content_.substr(pos1, (pos2 - pos1));

			if ((singleLine.size() > 0) && (singleLine.size() <= 512))
			{
				if (singleLine[0] == '.')
					emailText += "." + singleLine + newLine;
				else
					emailText += singleLine + newLine;
	
			}
			pos1 = pos2 + newLine.size();

			pos2 = content_.find(newLine, pos1);
		}

		if (pos1 < content_.size())
		{
			singleLine = content_.substr(pos1, (content_.size() - pos1));

			if ((singleLine.size() > 0) && (singleLine.size() <= 512))
			{
				if (singleLine[0] == '.')
					emailText += "." + singleLine + newLine;
				else
					emailText += singleLine + newLine;
	
			}
		}

	}

	emailText += "." + newLine;
}

