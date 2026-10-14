#ifndef REGISTERSECONDARYSCALEPROCESSEDMESSAGE_H
#define REGISTERSECONDARYSCALEPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class RegisterSecondaryScaleProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		RegisterSecondaryScaleProcessedMessage(int status, int scaleId);
		RegisterSecondaryScaleProcessedMessage(const char* buffer);
		RegisterSecondaryScaleProcessedMessage(const RegisterSecondaryScaleProcessedMessage& sm);
		virtual ~RegisterSecondaryScaleProcessedMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		RegisterSecondaryScaleProcessedMessage& operator = (const RegisterSecondaryScaleProcessedMessage& rhs);

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



#endif /* REGISTERSECONDARYSCALEPROCESSEDMESSAGE_H */
