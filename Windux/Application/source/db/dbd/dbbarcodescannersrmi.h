#ifndef DBBARCODESCANNERSRMI_H
#define DBBARCODESCANNERSRMI_H


#include <string>


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif



	class DBBarcodeScannerI;
	template <class DBEntityI> class DBQueryI;


	class DBBarcodeScannersRMI
	{
		public:

			virtual ~DBBarcodeScannersRMI() {}


			virtual DBQueryI<DBBarcodeScannerI>* retrieveAll() = 0;

			virtual DBBarcodeScannerI* findNumber(const std::string& code) = 0;
			//virtual DBBarcodeScannerI* findName(const std::string& name) = 0;		// jhc ss elev: I feel like I will really need this

			virtual DBBarcodeScannerI* createRecord(const std::string& code) = 0;
			virtual void saveRecord(const DBBarcodeScannerI* rec) = 0;
			virtual int deleteRecord(const DBBarcodeScannerI* rec) = 0;
			virtual int deleteAll() = 0;
			virtual int countAll() = 0;
			virtual int calculateAverageSpace() = 0;
			virtual bool execute(const std::string sql) = 0;

			virtual void show() = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // DBBARCODESCANNERSRMI_H
