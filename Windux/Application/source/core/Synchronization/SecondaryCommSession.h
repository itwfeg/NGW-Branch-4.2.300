#ifndef SECONDARYCOMMSESSION_H
#define SECONDARYCOMMSESSION_H



#include "SynchronizationCommSession.h"

#include "Scale.h"
#include "SynchronizationChange.h"


class CommMechanism;


namespace synchronization
{


	class SecondaryCommSession: public synchronization::SynchronizationCommSession
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		SecondaryCommSession(ScaleSynchronizationType scaleSynchronizationType, CommMechanism * uiClient = 0, bool enableInput = true);
		~SecondaryCommSession();


		/* --- Getter and setter methods --- */


		/* --- Instance Function members --- */


	protected:


	private:
		/* --- Instance Data Members --- */
		bool newBatch_;
		Util::CSmartPtr<hfc::Batch> batch_, receivedBatch_, receivingBatch_;
		Util::CSmartPtr<hfc::SynchronizationChange> change_;
		bool operationFailed_;


		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		SecondaryCommSession(const SecondaryCommSession& scs);


		/* --- Function members --- */
		/* Operators */
		// Operator asignment is not available
		SecondaryCommSession& operator = (const SecondaryCommSession& rhs);

		void doProcessHeader(Util::CSmartPtr<hfc::HeaderRecord> header);

		bool doProcessCommand(Util::CSmartPtr<SessionMessage> command);

		bool doReadAllInit();
		bool doReadAllRead();
		bool doReadAllWrite(CommSessionEvent::CommError& returnedError);
		bool doReadAllDone() { return true; }

		void doProcessError(CommSessionEvent::CommError error);

		bool doProcessFooter();

		bool doConclusion();

		void concludeReceivedBatch();


		/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
	};


}



#endif /* SECONDARYCOMMSESSION_H */
