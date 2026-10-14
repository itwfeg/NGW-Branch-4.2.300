#ifndef PROCESSSCALEONLINEMESSAGE_H
#define PROCESSSCALEONLINEMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class ProcessScaleOnlineMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessScaleOnlineMessage(int scaleId);
		ProcessScaleOnlineMessage(const char* buffer);
		ProcessScaleOnlineMessage(const ProcessScaleOnlineMessage& sm);
		virtual ~ProcessScaleOnlineMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		ProcessScaleOnlineMessage& operator = (const ProcessScaleOnlineMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int scaleId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* PROCESSSCALEONLINEMESSAGE_H */
