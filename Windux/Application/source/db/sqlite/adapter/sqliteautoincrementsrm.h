#ifndef SQLITEAUTOINCREMENTSRM_H
#define SQLITEAUTOINCREMENTSRM_H


#include <string>


#include "edb.h"
#include "sqliterecordmanager.h"

#include PROPERTIES_INC
#include MUTEX_INC
#include REGISTRY_INC


#ifndef NO_NAMESPACES
namespace edb_adapter
{
#endif


	class SqliteDatabase;


	class SqliteAutoIncrementsRM : public SqliteRecordManager
	{
		public:

			// --- Constructors and destructor ---
			SqliteAutoIncrementsRM(const SqliteDatabase& sdb, const util_env::Properties& props);
			virtual ~SqliteAutoIncrementsRM();


			// --- Processing methods ---
			void open();
			void close();
			static void openAll();
			static void closeAll();

			const std::string operator()(const std::string& key);
			void ensureAutoIncrementValue(const std::string& key, int value);


			static const std::string PRODUCTID_FIELD;
			static const std::string OPERATORCHANGEID_FIELD;
			static const std::string SYNCHRONIZATIONCHANGEID_FIELD;
			static const std::string NETWORKERRORID_FIELD;
			static const std::string SCALEID_FIELD;
			static const std::string COOLTOTALID_FIELD;
			static const std::string COOLPRODUCTIONSTEPID_FIELD;
			static const std::string COOLCOUNTRYID_FIELD;
			static const std::string COOLLISTID_FIELD;
			static const std::string EXPIRATIONLABELID_FIELD;
			static const std::string TRANSACTIONID_FIELD;
			static const std::string WRAPPERERRORID_FIELD;


		private:

			// --- Data members ---
			bool opened_;
			int firstAutoIncrement_;
			int maximumIncrement_;


			// Registry methods
			static util_registry::Registry<SqliteAutoIncrementsRM>* _registry_;
			static util_thread::Mutex<> _registryMutex_;
			static bool registerRM(SqliteAutoIncrementsRM* kai);
			static bool deregisterRM(SqliteAutoIncrementsRM* kai);
			static void disableAll();

			static util_thread::Mutex<> _incrementMutex_;

			static const std::string DATA_FILENAME;

			// Do not allow the default copy constructor.
			SqliteAutoIncrementsRM(const SqliteAutoIncrementsRM& rm);
			// Do not allow the default assignment operator.
			SqliteAutoIncrementsRM operator=(const SqliteAutoIncrementsRM& rhs);


			// --- Class Members ---
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};


#ifndef NO_NAMESPACES
} // namespace edb_adapter
#endif


#endif // SQLITEAUTOINCREMENTSRM_H
