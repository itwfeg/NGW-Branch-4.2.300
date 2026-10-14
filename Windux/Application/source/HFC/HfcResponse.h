/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCRESPONSE_H
#define HFCRESPONSE_H

#include "evolution.h"
#include "HfcObject.h"

typedef enum HfcResponseOffsets
{
	fieldOffsetHfcResponseCode = 1,
	fieldOffsetHfcResponseStyle,
	fieldOffsetHfcUserAnswer,
	fieldOffsetHfcResponseUserInput,
	endHfcResponseOffsets
}   HfcResponseOffsets_t;


/**
 * This class is used to send a message (error, warning, info, ...) to the UI.
 * The code indicates the specific message the UI should present the user.  A numeric
 * code is used so that the UI(Browser) can internationalize it.
 * The style is used to indicate what kind of input the user is allowed.
 */
class HfcResponse : public HfcObject
{
	public:

		static const short RESPONSESTYLE_STATUS			= 0;
		static const short RESPONSESTYLE_OKCANCEL		= 1;
		static const short RESPONSESTYLE_PROMPTOKCANCEL	= 2;
		static const short RESPONSESTYLE_NOINPUT		= 3;
		static const short RESPONSESTYLE_NEWPRODUCT		= 4;
		static const short RESPONSESTYLE_OK				= 5;
		static const short RESPONSESTYLE_CANCEL			= 6;
		static const short RESPONSESTYLE_PROMPTOK		= 7;
		static const short RESPONSESTYLE_PROMPTCANCEL	= 8;
		static const short RESPONSESTYLE_NODIALOG		= 9;
		static const short RESPONSESTYLE_NEWCLASS		= 10;
		static const short RESPONSESTYLE_REZEROIGNORE	= 11;
		static const short RESPONSESTYLE_SHUTDOWN           = 12;
		static const short RESPONSESTYLE_CALIBRATIONWEIGHTS = 13;
		static const short RESPONSESTYLE_FPP_PRODUCT	= 14;
		static const short RESPONSESTYLE_FPP_INGREDIENT	= 15;
		static const short RESPONSESTYLE_FPP_DUAL		= 16;
		static const short RESPONSESTYLE_REZERO			= 17;
		static const short RESPONSESTYLE_REBOOT_CANCEL	= 18;

		static const short RESPONSEANSWER_CANCEL	= 0;
		static const short RESPONSEANSWER_OK		= 1;     // ProductionPlanning: Item, Mixed = Update and Print; Ingredient = Update Only
		static const short RESPONSEANSWER_THREE		= 2;     // ProductionPlanning: Item = RePrint; Mixed = Update Only
		static const short RESPONSEANSWER_FOUR		= 3;     // ProductionPlanning: Mixed = RePrint

		HfcResponse(long code = 0L, short style = RESPONSESTYLE_STATUS, const HobString &msgData = "");
		HfcResponse(const char *data, int len, int formatType);
		~HfcResponse();

		virtual const char *serialize(int format) const;

		// This is the error device associated with the message.  It is auto-generated from the error code received.
		ErrorEvent getErrorDevice() const {return errorDevice;}

		long getCode() const {return code;}
		void setCode(long c) {code = c;}

		short getStyle() const {return style;}
		void setStyle(short s) {style = s;}

		// The following is used for the application to send text data to be displayed in the dialog,
		// as in the case when the application sends the operator note to the browser.
		const HobString &getMsgData() const {return msgData;}
		void setMsgData(const HobString &data) {msgData = data;}

		short getAnswer() const {return answer;}
		void setAnswer(short a) {answer = a;}

		// The following field is used with modalPrompts, the value the user entered in the prompt dialog.
		const HobString &getUserInput() const {return userInput;}
		void setUserInput(const HobString &input) {userInput = input;}

		bool isDismissed() const {return dismissed;}
		void setDismissed(bool d) {dismissed = d;}


	private:

		ErrorEvent errorDevice;
		long code;
		short style;
		HobString msgData;
		short answer;
		HobString userInput;
		bool dismissed;
		
		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCRESPONSE_H




