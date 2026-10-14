#ifndef PROCESSREMOTEBATCHMESSAGE_H
#define PROCESSREMOTEBATCHMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class ProcessRemoteBatchMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessRemoteBatchMessage(int id);
		ProcessRemoteBatchMessage(const char* buffer);
		ProcessRemoteBatchMessage(const ProcessRemoteBatchMessage& sm);
		virtual ~ProcessRemoteBatchMessage();


		/* --- Getters and setters --- */
		int getBatchId() const { return batchId_; }
		void setBatchId(int batchId) { batchId_ = batchId; }


		/* --- Function members --- */
		/* Operators*/
		ProcessRemoteBatchMessage& operator = (const ProcessRemoteBatchMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int batchId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* PROCESSREMOTEBATCHMESSAGE_H */
