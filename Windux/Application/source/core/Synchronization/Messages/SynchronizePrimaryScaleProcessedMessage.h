#ifndef SYNCHRONIZEPRIMARYSCALEPROCESSEDMESSAGE_H
#define SYNCHRONIZEPRIMARYSCALEPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class SynchronizePrimaryScaleProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		SynchronizePrimaryScaleProcessedMessage(int status, int scaleId);
		SynchronizePrimaryScaleProcessedMessage(const char* buffer);
		SynchronizePrimaryScaleProcessedMessage(const SynchronizePrimaryScaleProcessedMessage& sm);
		virtual ~SynchronizePrimaryScaleProcessedMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		SynchronizePrimaryScaleProcessedMessage& operator = (const SynchronizePrimaryScaleProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		int scaleId_;

		/* --- Constructors and destructor --- */


		/* --- Instance Function members --- */


		/* --- Class Data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* SYNCHRONIZEPRIMARYSCALEPROCESSEDMESSAGE_H */
