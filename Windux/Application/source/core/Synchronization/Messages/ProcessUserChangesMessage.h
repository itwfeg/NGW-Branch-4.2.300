#ifndef PROCESSUSERCHANGESMESSAGE_H
#define PROCESSUSERCHANGESMESSAGE_H



#include "EvolMessage.h"


#include "SynchronizationChange.h"


#include "CSmartPtr.h"



namespace synchronization
{


	class ProcessUserChangesMessage : public EvolMessage
	{
	public:
		/* --- Constructors and destructor --- */
		ProcessUserChangesMessage(Util::CSmartPtr<hfc::SynchronizationChange> record);
		ProcessUserChangesMessage(const char* buffer);
		ProcessUserChangesMessage(const ProcessUserChangesMessage& sm);
		virtual ~ProcessUserChangesMessage();


		/* --- Getters and setters --- */
		Util::CSmartPtr<hfc::SynchronizationChange> getUserChange() const { return userChange_; }
		void setUserChange(Util::CSmartPtr<hfc::SynchronizationChange> change) { userChange_ = change; }


		/* --- Function members --- */
		/* Operators*/
		ProcessUserChangesMessage& operator = (const ProcessUserChangesMessage& rhs);

		const char* serialize() const;


	private:
		/* --- Data members --- */
		Util::CSmartPtr<hfc::SynchronizationChange> userChange_;


		/* --- Constructors and destructor --- */


		/* --- Function members --- */


		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* PROCESSUSERCHANGESMESSAGE_H */
