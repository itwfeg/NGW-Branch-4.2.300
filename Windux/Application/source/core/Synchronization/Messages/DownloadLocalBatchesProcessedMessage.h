#ifndef DOWNLOADLOCALBATCHESPROCESSEDMESSAGE_H
#define DOWNLOADLOCALBATCHESPROCESSEDMESSAGE_H



#include "ProcessedMessage.h"



namespace synchronization
{


	class DownloadLocalBatchesProcessedMessage : public ProcessedMessage
	{
	public:
		/* --- Constructors and destructor --- */
		DownloadLocalBatchesProcessedMessage(int status, int scaleId);
		DownloadLocalBatchesProcessedMessage(const char* buffer);
		virtual ~DownloadLocalBatchesProcessedMessage();


		/* --- Getters and setters --- */
		int getScaleId() const { return scaleId_; }
		void setScaleId(int status) { scaleId_ = status; }


		/* --- Instance Function members --- */
		/* Operators*/
		DownloadLocalBatchesProcessedMessage& operator = (const DownloadLocalBatchesProcessedMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Instance Data members --- */
		int scaleId_;


		/* --- Constructors and destructor --- */


		/* --- Instance Function members --- */


		/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* DOWNLOADLOCALBATCHESPROCESSEDMESSAGE_H */
