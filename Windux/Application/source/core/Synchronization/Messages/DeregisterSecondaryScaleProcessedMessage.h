#ifndef DEREGISTERSECONDARYSCALEPROCESSEDMESSAGE_H
#define DEREGISTERSECONDARYSCALEPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class DeregisterSecondaryScaleProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		DeregisterSecondaryScaleProcessedMessage(int status, int scaleId);
		DeregisterSecondaryScaleProcessedMessage(const char* buffer);
		DeregisterSecondaryScaleProcessedMessage(const DeregisterSecondaryScaleProcessedMessage& sm);
		virtual ~DeregisterSecondaryScaleProcessedMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		DeregisterSecondaryScaleProcessedMessage& operator = (const DeregisterSecondaryScaleProcessedMessage& rhs);

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



#endif /* DEREGISTERSECONDARYSCALEPROCESSEDMESSAGE_H */
