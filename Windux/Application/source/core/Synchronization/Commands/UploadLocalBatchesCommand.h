#ifndef UPLOADLOCALBATCHESCOMMAND_H
#define UPLOADLOCALBATCHESCOMMAND_H



#include "CommCommand.h"



class EvolMessage;



namespace synchronization
{


	class UploadLocalBatchesCommand : public CommCommand
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		UploadLocalBatchesCommand(int scaleId, int rversion, EvolMessage* message = 0);
		UploadLocalBatchesCommand(const char * buffer);
		virtual ~UploadLocalBatchesCommand();


		/* --- Getters and setters --- */
		int getRemoteVersion() const { return remoteVersion_; }
		void setRemoteVersion(int rversion) { remoteVersion_ = rversion; }


	private:
		/* --- Data members --- */
		int remoteVersion_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		UploadLocalBatchesCommand(const UploadLocalBatchesCommand& sm);


		/* --- Function members --- */
		/* Operators*/
		// Operator asignment is not available
		UploadLocalBatchesCommand& operator = (const UploadLocalBatchesCommand& rhs);

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* UPLOADLOCALBATCHESCOMMAND_H */
