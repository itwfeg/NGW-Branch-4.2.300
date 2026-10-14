#ifndef SYNCHRONIZATIONCOMMSESSION_H
#define SYNCHRONIZATIONCOMMSESSION_H



#include "CommSession.h"


#include "SynchronizationManagerProxy.h"


#include "Scale.h"
#include "ScaleSynchronizationType.h"
#include "Batch.h"



class CommMechanism;


namespace synchronization
{


	class SynchronizationCommSession: public comm::CommSession
	{
	public:
		/* --- Types and subtypes --- */


		/* --- Constructors and destructor --- */
		SynchronizationCommSession(ScaleSynchronizationType scaleType, hfc::Scale::ScaleRole scaleRole,
				CommMechanism * uiClient = 0, const char* name = 0, bool inputEnabled = true, int reason = 0);
		~SynchronizationCommSession();


		/* --- Getter and setter methods --- */
		ScaleSynchronizationType getScaleSynchronizationType() const { return scaleSynchronizationType_; }

		hfc::Scale::ScaleRole getScaleRole() const { return scaleRole_; }


		/* --- Instance Function members --- */
//		bool doReadAllInit(Util::CSmartPtr<SessionMessage> command);
//		bool doReadAllDone(Util::CSmartPtr<SessionMessage> command);


	protected:
		/* --- Data members --- */
		ScaleSynchronizationType scaleSynchronizationType_;
		hfc::Scale::ScaleRole scaleRole_;
		Util::CSmartPtr<hfc::Scale> remoteScale_;


		/* --- Instance Function members --- */
		bool writeBatch(Util::CSmartPtr<hfc::Batch> batch);
		bool writeSynchronizationChange(Util::CSmartPtr<hfc::SynchronizationChange> change);

		bool doInitialization() { return true; }

		bool doSendHeader(CommSessionEvent::CommError& returnedError);

		bool doSendFooter(CommSessionEvent::CommError& returnedError);


	private:
		/* --- Constructors and destructor --- */
		// Copy constructor is not available
		SynchronizationCommSession(const SynchronizationCommSession& scs);


		/* --- Instance Function members --- */
		/* Operators */
		// Operator asignment is not available
		SynchronizationCommSession& operator = (const SynchronizationCommSession& rhs);



		/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static const std::string _dataTypeName_;
#endif
	};


}



#endif /* SYNCHRONIZATIONCOMMSESSION_H */
