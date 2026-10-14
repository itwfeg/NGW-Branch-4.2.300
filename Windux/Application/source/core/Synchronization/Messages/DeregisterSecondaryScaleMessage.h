#ifndef DEREGISTERSECONDARYSCALEMESSAGE_H
#define DEREGISTERSECONDARYSCALEMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class DeregisterSecondaryScaleMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		DeregisterSecondaryScaleMessage(int scaleId);
		DeregisterSecondaryScaleMessage(const char* buffer);
		DeregisterSecondaryScaleMessage(const DeregisterSecondaryScaleMessage& sm);
		virtual ~DeregisterSecondaryScaleMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		DeregisterSecondaryScaleMessage& operator = (const DeregisterSecondaryScaleMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int scaleId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* DEREGISTERSECONDARYSCALEMESSAGE_H */
