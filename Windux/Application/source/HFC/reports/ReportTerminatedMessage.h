#ifndef REPORTTERMINATEDMESSAGE_H
#define REPORTTERMINATEDMESSAGE_H



#include "EvolMessage.h"



class ReportTerminatedMessage : public EvolMessage
{
	public:
		/* --- Constructors and destructor --- */
		ReportTerminatedMessage(bool cancelled = false);
		ReportTerminatedMessage(const char * msgData);
		~ReportTerminatedMessage();


		/* --- Getters and setters --- */
		bool isCancelled() const { return cancelled_; }


		/* --- Instance function members --- */
		const char * serialize() const;


	private:
		/* --- Instance data members --- */
		bool cancelled_;


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* REPORTTERMINATEDMESSAGE_H */
