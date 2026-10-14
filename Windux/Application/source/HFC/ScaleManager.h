#ifndef SCALEMANAGER_H
#define SCALEMANAGER_H



#include <string>



#include "DataManager.h"


#include "Scale.h"


#include "DBScalesRM.h"
#include "DBScale.h"
#include "DBQuery.h"


#include "CSmartPtr.h"



namespace hfc
{


	class HFCManagerContainer;
	class Scale;


	class ScaleManager : public DataManager
	{
		public:
			/* --- Constructors and destructor --- */
			ScaleManager(hfc::HFCManagerContainer* container);
			~ScaleManager();


			/* --- Instance function members --- */
			Scale * findScaleByIdentifier(int id);
			Scale * findScaleByIdentifier(const std::string & id);
			Scale * findScaleByHostname(const std::string & hostname);
			Scale * findScaleByIPAddress(const std::string & ipAddress);

			Scale * getThisScale();
			Scale * getPrimaryScale();

			bool initializeScalesQuery();
			Scale * getFirstScale();
			Scale * getNextScale();

			bool initializeSecondaryScalesQuery();
			Scale * getFirstSecondaryScale();
			Scale * getNextSecondaryScale();

			void saveScale(Scale* scale);
			int deleteScale(Scale* scale);
			int deleteAllScales();

			void handleMessage(SessionMessage* msg);
			void destroyQueriesAndIters();


		private:
			hfc::HFCManagerContainer* mgrContainer_;
			Util::CSmartPtr< edb::DBQuery<edb::DBScale> > allScalesQuery_, query_;
			Util::CSmartPtr< edb::DBQuery<edb::DBScale>::iterator > allScalesIterator_, iterator_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif


			/* --- Instance function members --- */
			Scale* createScale(const edb::DBScale * dbScaleRec);
	};
}



#endif /* SCALEMANAGER_H */
