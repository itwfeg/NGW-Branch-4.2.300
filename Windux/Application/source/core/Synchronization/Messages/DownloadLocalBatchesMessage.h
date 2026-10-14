#ifndef DOWNLOADLOCALBATCHESMESSAGE_H
#define DOWNLOADLOCALBATCHESMESSAGE_H



#include "EvolMessage.h"



namespace synchronization
{


	class DownloadLocalBatchesMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		DownloadLocalBatchesMessage(int scaleId);
		DownloadLocalBatchesMessage(const char* buffer);
		virtual ~DownloadLocalBatchesMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int scaleId) { scaleId_ = scaleId; }


		/* --- Function members --- */
		/* Operators*/
		DownloadLocalBatchesMessage& operator = (const DownloadLocalBatchesMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		int scaleId_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* DOWNLOADLOCALBATCHESMESSAGE_H */
