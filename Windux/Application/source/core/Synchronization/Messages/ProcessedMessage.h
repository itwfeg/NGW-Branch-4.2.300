#ifndef PROCESSEDMESSAGE_H
#define PROCESSEDMESSAGE_H



#include "EvolMessage.h"

#include "Batch.h"



namespace synchronization
{


	class ProcessedMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessedMessage(EvolDeviceType deviceType, UINT8 eventType, int status = 0);
		ProcessedMessage(const char* buffer);
		ProcessedMessage(const ProcessedMessage& pm);
		virtual ~ProcessedMessage();


		/* --- Getters and setters --- */
		int getStatus() const { return status_; }
		void setStatus(int status) { status_ = status; }



		/* --- Function members --- */
		/* Operators*/
		ProcessedMessage& operator = (const ProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int status_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* PROCESSEDMESSAGE_H */
