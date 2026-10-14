#ifndef SYNCHRONIZESECONDARYSCALEMESSAGE_H
#define SYNCHRONIZESECONDARYSCALEMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class SynchronizeSecondaryScaleMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		SynchronizeSecondaryScaleMessage(int scaleId);
		SynchronizeSecondaryScaleMessage(const char* buffer);
		SynchronizeSecondaryScaleMessage(const SynchronizeSecondaryScaleMessage& sm);
		virtual ~SynchronizeSecondaryScaleMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		SynchronizeSecondaryScaleMessage& operator = (const SynchronizeSecondaryScaleMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int scaleId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* SYNCHRONIZESECONDARYSCALEMESSAGE_H */
