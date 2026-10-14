#ifndef PRIMARYCOMMSESSION_H
#define PRIMARYCOMMSESSION_H



#include "SynchronizationCommSession.h"

#include "Scale.h"


class CommMechanism;


namespace synchronization
{


	class PrimaryCommSession: public synchronization::SynchronizationCommSession
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		PrimaryCommSession(ScaleSynchronizationType scaleSynchronizationType, CommMechanism * uiClient = 0, bool inputEnabled = true, int reason = 0);
		~PrimaryCommSession();


		/* --- Getter and setter methods --- */


		/* --- Instance Function members --- */


	protected:
		/* --- Data members --- */
		bool scaleWasOffLine_;
		bool firstCommand_;



	private:
		/* --- Instance Data Members --- */
		bool operationFailed_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		PrimaryCommSession(const PrimaryCommSession& scs);


		/* --- Function members --- */
		/* Operators */
		// Operator asignment is not available
		PrimaryCommSession& operator = (const PrimaryCommSession& rhs);

		void doProcessHeader(Util::CSmartPtr<hfc::HeaderRecord> header);

		bool doProcessCommand(Util::CSmartPtr<SessionMessage> command);

		void doProcessError(CommSessionEvent::CommError error);

		bool doProcessFooter() { return true; }

		bool doConclusion();

		
		/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* PRIMARYCOMMSESSION_H */
