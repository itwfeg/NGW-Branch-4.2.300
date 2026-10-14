#include <string>


#include "edb.h"
#include DBPRODUCTSRM_INC
#include DBPRODUCT_INC
#include DBCLASS_INC
#include DBPRODUCTTEXT_INC
#include DBNUTRITION_INC
#include DBCATEGORY_INC
#include DBCOOLCATEGORY_INC
#include DBCOOLPRETEXT_INC
#include DBCOOLTEXT_INC
#include DBCOOLLIST_INC
#include DBPRODUCTSTATISTIC_INC
#include DBSPECIALPROGRAM_INC
#include DBSOFTKEY_INC

#include DBSTATUSRM_INC
#include UTILSTRING_INC

#include <Configuration.h>
#include <HobartCustomerHook.h>

#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBProductsRM::DBProductsRM(DBManager* dbm, DBRecordManager::DBRecordType recordType, edb_adapter::DBProductsRMI* d)
		: dbm_(dbm), recordType_(recordType), enabled_(false), decorated_(d)
{
	if ( registerRM(*this) )
		enabled_ = true;
}


DBProductsRM::~DBProductsRM()
{
	DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCloseTable);

	disable();

	deregisterRM(*this);

	// The following code is unnecessary since the disable method (called above)
	// already performs the same functionality
	if ( decorated_ )
		delete decorated_;
}


DBQuery<DBProduct>* DBProductsRM::retrieveAll()
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveAll(), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveAllForDept(const string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveAllForDept(deptNum), *this);
	}
	else
		return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveRangePLU(const string& first, const string& last, const string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveRangePLU(first, last, deptNum), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveNearestPLU(const DBProduct* product, int count)
{
	if ( enabled_ && product )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveNearestPLU(product->decorated_, count), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveNearestPLU(const std::string& plu, const std::string& deptNum, int count)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveNearestPLU(plu, deptNum, count), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveKeywordInDescription(const string& keyword, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveKeywordInDescription(keyword, deptNum), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveKeywordInName(const std::string& keyword)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveKeywordInName(keyword), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveDescriptionStartsWith(const string& keyword, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveDescriptionStartsWith(keyword, deptNum), *this);
	}

	return 0;
}


bool DBProductsRM::isProductShortNameUsed(LanguageSelection langNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return decorated_->isProductShortNameUsed(langNum);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoProduct(const DBProduct* prod)
{
	// BUSINESS RULE: Products are currently linked to each other via the reporting class
	// field. This could change if we provide a field in the product record with the sole
	// purpose of linking products. This is why we do not pass the product record down to
	// the next layers, but the value of its reporting class field.
	if ( enabled_ && prod )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoClass(prod->getReportingClass(), prod->getDeptNumber()), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoClass(const DBClass* clas)
{
	if ( enabled_ && clas )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoClass(clas->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoNutrition(const DBNutrition* nutrition)
{
	if ( enabled_ && nutrition )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoNutrition(nutrition->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoText(const DBProductText* text)
{
	if ( enabled_ && text )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoText(text->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoMessage(const DBProductText* message)
{
	if ( enabled_ && message )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoMessage(message->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoNote(const DBProductText* note)
{
	if ( enabled_ && note )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoNote(note->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoMarquee(const DBProductText* marquee)
{
	if ( enabled_ && marquee )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoMarquee(marquee->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoProductText5(const DBProductText* prodText)
{
	if ( enabled_ && prodText )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoProductText5(prodText->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoProductText6(const DBProductText* prodText)
{
	if ( enabled_ && prodText )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoProductText6(prodText->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoProductText7(const DBProductText* prodText)
{
	if ( enabled_ && prodText )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoProductText7(prodText->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoProductText8(const DBProductText* prodText)
{
	if ( enabled_ && prodText )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoProductText8(prodText->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoProductText9(const DBProductText* prodText)
{
	if ( enabled_ && prodText )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoProductText9(prodText->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoCategory(const DBCategory* category)
{
	if ( enabled_ && category )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoCategory(category->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoCoolCategory(const DBCoolCategory* coolcategory)
{
	if ( enabled_ && coolcategory )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoCoolCategory(coolcategory->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoCoolPreText(const DBCoolPreText* coolpretext)
{
	if ( enabled_ && coolpretext )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoCoolPreText(coolpretext->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoDefaultCoolText(const DBCoolText* cooltext)
{
	if ( enabled_ && cooltext )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoDefaultCoolText(cooltext->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoCoolList(const DBCoolList* coollist)
{
	if ( enabled_ && coollist )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoCoolList(coollist->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveSpecials(const DBSpecialProgram* special)
{
	if ( enabled_ && special )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveSpecials(special->decorated_), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveAllSpecials(const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveAllSpecials(deptNum), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveLinkedtoRec(const std::string& column1Name, const std::string column1Value, const std::string& column2Name, const std::string& column2Value)
{
	if ( enabled_ && column1Name.size() && column1Value.size() )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveLinkedtoRec(column1Name, column1Value, column2Name, column2Value), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveNameStartsWith(keyword, langNum, deptNum), *this);
	}

	return 0;
}


DBQuery<DBProduct>* DBProductsRM::retrieveDescriptionOrNameStartsWith(const std::string& keyword, LanguageSelection langNum, const std::string& deptNum)
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessRetrieveRecords);

		return new DBQuery<DBProduct>(dbm_, decorated_->retrieveDescriptionOrNameStartsWith(keyword, langNum, deptNum), *this);
	}

	return 0;
}


DBProduct* DBProductsRM::findPLU(const string& plu, const string& deptNum, bool inOperateMode) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBProductI* rec = NULL;

		bool lookupPluInDB = true;
#if defined( __linux__ )
		if ( Configuration::getInstance()->getCustomerHooksEnabled() && ( inOperateMode || !Configuration::getInstance()->toBoolCfgI(fieldOffsetCfgIUtilizeCustomerHookForOperateModeOnly) ) )
			lookupPluInDB = hobart_cust_hook_before_plu_read(plu.c_str(), deptNum.c_str());
#endif
		if (lookupPluInDB)
			rec = decorated_->findPLU(plu, deptNum);

		if ( rec )
			return new DBProduct(rec);
	}

	return 0;
}


DBProduct* DBProductsRM::findLinkedRecord(const DBProductStatistic* stat) const
{
	if ( enabled_ && stat )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBProductI* rec = decorated_->findLinkedRecord(stat->decorated_);

		if ( rec )
			return new DBProduct(rec);
	}

	return 0;
}


DBProduct* DBProductsRM::findLinkedRecord(const DBSoftKey* skey) const
{
	if ( enabled_ && skey )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessFindRecord);

		edb_adapter::DBProductI* rec = decorated_->findLinkedRecord(skey->decorated_);

		if ( rec )
			return new DBProduct(rec);
	}

	return 0;
}


DBProduct* DBProductsRM::createRecord() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCreateRecord);

		edb_adapter::DBProductI* rec = decorated_->createRecord();

		if ( rec )
		{
			DBProduct* r = new DBProduct(rec);
			if ( r )
				return r;
			else
				delete rec;
		}
	}

	return 0;
}


void DBProductsRM::saveRecord(const DBProduct* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessSaveRecord);

		decorated_->saveRecord(rec->decorated_);
	}
}


int DBProductsRM::deleteRecord(const DBProduct* rec) const
{
	if ( enabled_ && rec )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteRecord);

		return decorated_->deleteRecord(rec->decorated_);
	}

	return 0;
}


int DBProductsRM::deleteAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAll();

		deregisterRM(*const_cast<DBProductsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBProductsRM*>(this));

		return result;
	}

	return 0;
}


int DBProductsRM::deleteAllForDept(const std::string& deptNum)
{
	if (enabled_)
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDeleteAllRecords);

		int result = decorated_->deleteAllForDept(deptNum);

		deregisterRM(*const_cast<DBProductsRM*>(this));
		disableAllRMs();
		registerRM(*const_cast<DBProductsRM*>(this));

		return result;
	}
	else
		return 0;
}


int DBProductsRM::countAll() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCountAllRecords);

		return decorated_->countAll();
	}

	return 0;
}


int DBProductsRM::calculateAverageSpace() const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessCalculateAverageSpace);

		return decorated_->calculateAverageSpace();
	}

	return 0;
}


bool DBProductsRM::execute(const std::string sql) const
{
	if ( enabled_ )
	{
		DBStatusRM::BusyIdleFunctor functor = DBStatusRM::getBusyIdleFunctor(this, DBAccessEntity::DBAccessDatabaseOperation);

		return decorated_->execute(sql);
	}
	else
		return false;
}


void DBProductsRM::show()
{
	Debug::trace("database.show", 20, "DBProductsRM instance address: " + 
			util_string::ptrtostring(this) + (enabled_ ? " (ENABLED)" : " (DISABLED)"));

	Debug::trace("database.show", 20, "  Active queries: " + util_string::inttostring(qsRegistry_.size()));
	showAllQueries();

	decorated_->show();
}


void DBProductsRM::showAllQueries()
{
	qsRegistry_.process_mem(&DBQuery<DBProduct>::show);
}


#ifndef EVOL_NDEBUG
void DBProductsRM::showAll(const std::string& dc)
#else
void DBProductsRM::showAll(const std::string& UNUSED1(dc))
#endif
{
#ifndef EVOL_NDEBUG
	std::string debuggingCategory(dc);

	if ( debuggingCategory.empty() )
		debuggingCategory.assign("database.show");
#else
	UNUSED1(dc);
#endif

	if ( _registry_ && _registry_->size() )
	{
#ifndef EVOL_NDEBUG
		Debug::trace(debuggingCategory, 20, "DBProductsRM INSTANCES: " + util_string::inttostring(_registry_->size()));
#endif

		// Show each of the currently registered DBProductsRM
		_registry_->process_mem(&DBProductsRM::show);
	}
#ifndef EVOL_NDEBUG
	else
		Debug::trace(debuggingCategory, 20, "DBProductsRM INSTANCES: None");
#endif
}


void DBProductsRM::disable()
{
	// Mark this record manager as not enabled so that it cannot be used
	enabled_ = false;

	// Now disable all queries associated with this record manager
	disableAllQs();

	// Now dispose of the internal decorated_ element so we do not have problems
	// later on when deleting the record manager (the session currently deletes
	// the DBManager before it deletes some of the associated record managers).
	if ( decorated_ )
	{
		delete decorated_;
		decorated_ = 0;
	}
}


bool DBProductsRM::registerQ(DBQuery<DBProduct>* query)
{
	return qsRegistry_.add(query);
}


bool DBProductsRM::deregisterQ(DBQuery<DBProduct>* query)
{
	return qsRegistry_.remove(query);
}


void DBProductsRM::disableAllQs()
{
	qsRegistry_.process_mem(&DBQuery<DBProduct>::disable);
	qsRegistry_.removeAll();
}



const std::string DBProductsRM::DESCRIPTOR = "PRODUCT";



bool DBProductsRM::registerRM(DBProductsRM& view)
{
	// We don't need to keep this registry for SQLite, so we're bypassing this.
	return true;

	/* This code was enabled. But with the return true above, this is unreachable and gives warnings.  I am disabling it
	if ( !_registry_ )
	{
		util_thread::Lock< util_thread::Mutex<> > lock(_registryMutex_);
		if ( !_registry_ )
			_registry_ = new util_registry::Registry<DBProductsRM>;
	}

	return _registry_->add(&view);
	*/
}


void DBProductsRM::disableAllRMs()
{
	if ( _registry_ )
	{
		_registry_->process_mem(&DBProductsRM::disable);
		_registry_->removeAll();
	}
}


util_registry::Registry<DBProductsRM>* DBProductsRM::_registry_ = 0;
util_thread::Mutex<> DBProductsRM::_registryMutex_ = util_thread::Mutex<>();


#ifndef NO_NAMESPACES
} // namespace edb
#endif
