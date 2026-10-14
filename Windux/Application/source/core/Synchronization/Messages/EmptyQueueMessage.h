#ifndef EMPTYQUEUEMESSAGE_H
#define EMPTYQUEUEMESSAGE_H



#include "EvolMessage.h"


#include "QueueManager.h"



namespace synchronization
{


	class EmptyQueueMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		EmptyQueueMessage(QueueManager::QueueType queue);
		EmptyQueueMessage(const char* buffer);
		EmptyQueueMessage(const EmptyQueueMessage& sm);
		virtual ~EmptyQueueMessage();


		/* --- Getters and setters --- */
		QueueManager::QueueType getQueueType() const { return queue_; }
		void setQueueType(QueueManager::QueueType queue) { queue_ = queue; }


		/* --- Function members --- */
		/* Operators*/
		EmptyQueueMessage& operator = (const EmptyQueueMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		QueueManager::QueueType queue_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* EMPTYQUEUEMESSAGE_H */
