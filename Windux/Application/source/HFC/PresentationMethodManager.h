/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRESENTATIONMETHODMANAGER_H
#define PRESENTATIONMETHODMANAGER_H

#include "evolution.h"
#include "DataManager.h"
#include "PresentationMethod.h"

class SessionMessage;
class Session;

#include "DBPresentationMethodsRM.h"
#include "DBPresentationMethod.h"
#include "DBQuery.h"


class PresentationMethodManager : public DataManager
{
	public:

		PresentationMethodManager();
		PresentationMethodManager(Session *session);
		virtual ~PresentationMethodManager();

		PresentationMethod *firstPresentationMethod();
		PresentationMethod *nextPresentationMethod();
		int countAllPresentationMethods();

		PresentationMethod *getCurrentPresentationMethod();
		void setCurrentPresentationMethod(PresentationMethod *newPresentationMethod);

		void handleMessage(SessionMessage *msg);

		void handleDeleteAll();

		void destroyQueriesAndIters();


	private:

		Session *mySession;

		PresentationMethod *currentPresentationMethod;

		edb::DBQuery<edb::DBPresentationMethod> *allPresentationMethodsQuery;
		edb::DBQuery<edb::DBPresentationMethod>::const_iterator *allPresentationMethodsIter;


		void init();

		void handleRead(PresentationMethod *msgPresentationMethod);
		void handleWrite(PresentationMethod *modifiedPresentationMethod);
		void handleDelete(PresentationMethod *msgPresentationMethod);
		void handleSave(PresentationMethod *modifiedPresentationMethod);
		void handleRestore(PresentationMethod *restorePresentationMethod);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //PRESENTATIONMETHODMANAGER_H
