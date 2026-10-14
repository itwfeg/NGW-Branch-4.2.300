#ifndef REGISTERSECONDARYSCALEMESSAGE_H
#define REGISTERSECONDARYSCALEMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class RegisterSecondaryScaleMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		RegisterSecondaryScaleMessage(int scaleId);
		RegisterSecondaryScaleMessage(const char* buffer);
		RegisterSecondaryScaleMessage(const RegisterSecondaryScaleMessage& sm);
		virtual ~RegisterSecondaryScaleMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		RegisterSecondaryScaleMessage& operator = (const RegisterSecondaryScaleMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		int scaleId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */
	};


}



#endif /* REGISTERSECONDARYSCALEMESSAGE_H */
