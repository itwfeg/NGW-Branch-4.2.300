#ifndef SYNCHRONIZESECONDARYSCALEPROCESSEDMESSAGE_H
#define SYNCHRONIZESECONDARYSCALEPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class SynchronizeSecondaryScaleProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		SynchronizeSecondaryScaleProcessedMessage(int status, int scaleId);
		SynchronizeSecondaryScaleProcessedMessage(const char* buffer);
		SynchronizeSecondaryScaleProcessedMessage(const SynchronizeSecondaryScaleProcessedMessage& sm);
		virtual ~SynchronizeSecondaryScaleProcessedMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		SynchronizeSecondaryScaleProcessedMessage& operator = (const SynchronizeSecondaryScaleProcessedMessage& rhs);

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



#endif /* SYNCHRONIZESECONDARYSCALEPROCESSEDMESSAGE_H */
