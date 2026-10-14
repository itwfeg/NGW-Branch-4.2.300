#ifndef ADDNEWSESSIONMESSAGE_H
#define ADDNEWSESSIONMESSAGE_H



#include "EvolMessage.h"



class AbstractSession;



class AddNewSessionMessage : public EvolMessage
{
	public:
		/* --- Constructors and destructor --- */
		AddNewSessionMessage(AbstractSession* session);
		AddNewSessionMessage(const char * msgData);
		~AddNewSessionMessage();


		/* --- Getters and setters --- */
		AbstractSession* getSession() const { return session_; }


		/* --- Instance function members --- */
		const char * serialize() const;


	private:
		/* --- Instance data members --- */
		AbstractSession* session_;


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* ADDNEWSESSIONMESSAGE_H */
