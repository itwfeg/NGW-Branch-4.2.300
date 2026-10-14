#ifndef DISABLEINPUTPROCESSINGMESSAGE_H
#define DISABLEINPUTPROCESSINGMESSAGE_H



#include "EvolMessage.h"



class DisableInputProcessingMessage : public EvolMessage
{
	public:
		/* --- Constructors and destructor --- */
		DisableInputProcessingMessage(int textId);
		DisableInputProcessingMessage(const char * msgData);
		~DisableInputProcessingMessage();


		/* --- Getters and setters --- */
		int getTextId() const { return textId_; }
		void setTextId(int textId) { textId_ = textId; }


		/* --- Instance function members --- */
		const char * serialize() const;


	private:
		/* --- Instance data members --- */
		int textId_;


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* DISABLEINPUTPROCESSINGMESSAGE_H */
