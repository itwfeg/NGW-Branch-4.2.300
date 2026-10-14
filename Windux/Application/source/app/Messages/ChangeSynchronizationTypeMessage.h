#ifndef CHANGESYNCHRONIZATIONTYPEMESSAGE_H
#define CHANGESYNCHRONIZATIONTYPEMESSAGE_H



#include "EvolMessage.h"


#include "ScaleSynchronizationType.h"



class ChangeSynchronizationTypeMessage : public EvolMessage
{
	public:
		/* --- Constructors and destructor --- */
		ChangeSynchronizationTypeMessage(ScaleSynchronizationType type, const std::string& primary);
		ChangeSynchronizationTypeMessage(const char * msgData);
		~ChangeSynchronizationTypeMessage();


		/* --- Getters and setters --- */
		ScaleSynchronizationType getSynchronizationType() const { return syncType_; }
		void setSynchronizationType(ScaleSynchronizationType type) { syncType_ = type; }

		const std::string& getPrimaryScale() const { return primaryScale_; }
		void setPrimaryScale(const std::string& primary) { primaryScale_ = primary; }


		/* --- Instance function members --- */
		const char * serialize() const;


	private:
		/* --- Instance data members --- */
		ScaleSynchronizationType syncType_;
		std::string primaryScale_;


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};



#endif /* CHANGESYNCHRONIZATIONTYPEMESSAGE_H */
