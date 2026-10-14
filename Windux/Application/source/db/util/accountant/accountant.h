#ifndef ACCOUNTANT_H
#define ACCOUNTANT_H



#include <string>


#include "util/thread/mutex.h"



#ifndef NO_NAMESPACES
namespace util_acc
{
#endif



class DataType
{
	public:
		typedef enum
		{
			NO_TYPE,
			ACCOUNTANT,

			DB_MANAGER,
			DB_MANAGER_FACTORY_FUNCTOR,
			DB_QUERY,
			DB_QUERY_ITERATOR,
			DB_TEXT_SEARCH_QUERY,
			DB_RECORD_MANAGER,
			DB_FACTORY,

			DB_CATEGORIES_RM,
			DB_CATEGORY_DETAILED_STATISTICS_RM,
			DB_CATEGORY_STATISTICS_RM,
			DB_CLASSES_RM,
			DB_CONFIGURATIONS_RM,
			DB_COOL_CATEGORIES_RM,
			DB_COOL_CATEGORY_COUNTRIES_RM,
			DB_COOL_CATEGORY_PRE_TEXTS_RM,
			DB_COOL_CATEGORY_PRODUCTION_STEPS_RM,
			DB_COOL_COUNTRIES_RM,
			DB_COOL_LISTS_RM,
			DB_COOL_LIST_TEXTS_RM,
			DB_COOL_PRE_TEXTS_RM,
			DB_COOL_PRODUCTION_STEPS_RM,
			DB_COOL_TEXTS_RM,
			DB_DEVICES_RM,
			DB_EMAIL_ALERTS_RM,
			DB_FLASH_FILES_RM,
			DB_LABELS_RM,
			DB_LABEL_TEXTS_RM,
			DB_MESSAGES_RM,
			DB_NETWORK_ERRORS_RM,
			DB_NOTES_RM,
			DB_NUTRITIONS_RM,
			DB_OPERATORS_RM,
			DB_OPERATOR_NOTES_RM,
			DB_PRESENTATION_METHODS_RM,
			DB_PRESENTATION_METHOD_DETAILED_STATISTICS_RM,
			DB_PRESENTATION_METHOD_STATISTICS_RM,
			DB_PRODUCT_DEFAULTS_RM,
			DB_PRODUCT_DETAILED_STATISTICS_RM,
			DB_PRODUCTS_RM,
			DB_PRODUCT_STATISTICS_RM,
			DB_ROM_LABELS_RM,
			DB_SOFT_KEYS_RM,
			DB_SOFT_KEY_FEATURES_RM,
			DB_SOFT_KEY_GROUPS_RM,
			DB_SOFT_KEY_STYLES_RM,
			DB_SPECIAL_PROGRAMS_RM,
			DB_STATUS_RM,
			DB_STREAMING_GRAPHICS_RM,
			DB_TEXTS_RM,
			DB_TRANSACTION_TYPES_RM,

			DB_CATEGORY,
			DB_CATEGORY_DETAILED_STATISTIC,
			DB_CATEGORY_STATISTIC,
			DB_CLASS,
			DB_CONFIGURATION,
			DB_COOL_CATEGORY,
			DB_COOL_CATEGORY_COUNTRY,
			DB_COOL_CATEGORY_PRE_TEXT,
			DB_COOL_CATEGORY_PRODUCTION_STEP,
			DB_COOL_COUNTRY,
			DB_COOL_LIST,
			DB_COOL_LIST_TEXT,
			DB_COOL_PRE_TEXT,
			DB_COOL_PRODUCTION_STEP,
			DB_COOL_TEXT,
			DB_DEVICE,
			DB_EMAIL_ALERT,
			DB_EXCEPTION,
			DB_FLASH_FILE,
			DB_LABEL,
			DB_LABEL_TEXT,
			DB_MESSAGE,
			DB_NETWORK_ERROR,
			DB_NOTE,
			DB_NUTRITION,
			DB_OPERATOR,
			DB_OPERATOR_NOTE,
			DB_PRESENTATION_METHOD,
			DB_PRESENTATION_METHOD_DETAILED_STATISTIC,
			DB_PRESENTATION_METHOD_STATISTIC,
			DB_PRODUCT,
			DB_PRODUCT_DEFAULT,
			DB_PRODUCT_DETAILED_STATISTIC,
			DB_PRODUCT_STATISTIC,
			DB_ROM_LABEL,
			DB_SOFT_KEY,
			DB_SOFT_KEY_FEATURE,
			DB_SOFT_KEY_GROUP,
			DB_SOFT_KEY_STYLE,
			DB_SPECIAL_PROGRAM,
			DB_STATUS,
			DB_STREAMING_GRAPHIC,
			DB_TEXT,
			DB_TRANSACTION_TYPE,

			CB_DATABASE,
			CB_FILE_HANDLE,
			CB_FILE_HANDLE_IS_SAME_THREAD,
			CB_FILE_HANDLE_ORDER,
			CB_FILE_LOCKS,
			CB_FILE_LOCK,
			CB_FILE_LOCK_IS_INCOMPATIBLE,
			CB_FILE_LOCK_NEXT_LOCK,
			CB_FILE_LOCK_IS_SAME_THREAD,
			CB_FILE_LOCK_IS_SAME_FILENAME,
			CB_LOCK_MANAGER,
			CB_FACTORY_FUNCTOR,
			CB_FACTORY,
			CB_FIELD,
			CB_PRODUCT_PRODUCT_SEQUENCE_QUERY,
//			CB_QUERY,
//			CB_RECORD,
//			CB_RECORD_MANAGER,
//			CB_SEEK_QUERY,
			CB_SEEK_QUERY_FIELD_COMPARE,
//			CB_VIEW,

//			CB_AUTO_INCREMENTS_RM,
//			CB_CATEGORIES_RM,
//			CB_CATEGORY_DETAILED_STATISTICS_RM,
//			CB_CATEGORY_STATISTICS_RM,
//			CB_CLASSES_RM,
//			CB_CONFIGURATIONS_RM,
//			CB_COOL_CATEGORIES_RM,
//			CB_COOL_CATEGORY_COUNTRIES_RM,
//			CB_COOL_CATEGORY_PRE_TEXTS_RM,
//			CB_COOL_CATEGORY_PRODUCTION_STEPS_RM,
//			CB_COOL_COUNTRIES_RM,
//			CB_COOL_LISTS_RM,
//			CB_COOL_LIST_TEXTS_RM,
//			CB_COOL_PRE_TEXTS_RM,
//			CB_COOL_PRODUCTION_STEPS_RM,
//			CB_COOL_TEXTS_RM,
//			CB_DEVICES_RM,
//			CB_EMAIL_ALERTS_RM,
//			CB_FLASH_FILE_RM,
//			CB_IGNORE_WORD_RM,
//			CB_LABEL_RM,
//			CB_LABEL_TEXT_RM,
//			CB_MESSAGE_RM,
//			CB_NETWORK_ERROR_RM,
//			CB_NOTE_RM,
//			CB_NUTRITION_RM,
//			CB_OPERATOR_RM,
//			CB_OPERATOR_NOTE_RM,
//			CB_PRESENTATION_METHOD_RM,
//			CB_PRESENTATION_METHOD_DETAILED_STATISTIC_RM,
//			CB_PRESENTATION_METHOD_STATISTIC_RM,
//			CB_PRODUCT_RM,
//			CB_PRODUCT_DEFAULT_RM,
//			CB_PRODUCT_DETAILED_STATISTIC_RM,
//			CB_PRODUCT_SEQUENCE_RM,
//			CB_PRODUCT_STATISTIC_RM,
//			CB_ROM_LABEL_RM,
//			CB_SOFT_KEY_RM,
//			CB_SOFT_KEY_FEATURE_RM,
//			CB_SOFT_KEY_GROUP_RM,
//			CB_SOFT_KEY_STYLE_RM,
//			CB_SPECIAL_PROGRAM_RM,
//			CB_STREAMING_GRAPHIC_RM,
//			CB_TEXT_RM,
//			CB_TRANSACTION_TYPE_RM,

			CB_CATEGORY,
			CB_CATEGORY_DETAILED_STATISTIC,
			CB_CATEGORY_STATISTIC,
			CB_CLASS,
			CB_CONFIGURATION,
			CB_COOL_CATEGORY,
			CB_COOL_CATEGORY_COUNTRY,
			CB_COOL_CATEGORY_PRE_TEXT,
			CB_COOL_CATEGORY_PRODUCTION_STEP,
			CB_COOL_COUNTRY,
			CB_COOL_LIST,
			CB_COOL_LIST_TEXT,
			CB_COOL_PRE_TEXT,
			CB_COOL_PRODUCTION_STEP,
			CB_COOL_TEXT,
			CB_DEVICE,
			CB_EMAIL_ALERT,
			CB_FLASH_FILE,
			CB_IGNORE_WORD,
			CB_LABEL,
			CB_LABEL_TEXT,
			CB_MESSAGE,
			CB_NETWORK_ERROR,
			CB_NOTE,
			CB_NUTRITION,
			CB_OPERATOR,
			CB_OPERATOR_NOTE,
			CB_PRESENTATION_METHOD,
			CB_PRESENTATION_METHOD_DETAILED_STATISTIC,
			CB_PRESENTATION_METHOD_STATISTIC,
			CB_PRODUCT,
			CB_PRODUCT_DEFAULT,
			CB_PRODUCT_DETAILED_STATISTIC,
			CB_PRODUCT_PRODUCT_SEQUENCE,
			CB_PRODUCT_SEQUENCE,
			CB_PRODUCT_STATISTIC,
			CB_ROM_LABEL,
			CB_SOFT_KEY,
			CB_SOFT_KEY_FEATURE,
			CB_SOFT_KEY_GROUP,
			CB_SOFT_KEY_STYLE,
			CB_SPECIAL_PROGRAM,
			CB_STREAMING_GRAPHIC,
			CB_TEXT,
			CB_TRANSACTION_TYPE,

			UTIL_CL_OPTIONS,
			UTIL_CL_OPTION,
			UTIL_REGISTRY,
			UTIL_REGISTRY_ENTRY,
			UTIL_EXCEPTION,
			UTIL_DIRECTORY,
			UTIL_DIRECTORY_ENTRY,
			UTIL_FILE,
			UTIL_FILENAME,
			UTIL_COLLATE_EN,
			UTIL_COLLATE_ES,
			UTIL_CTYPE_EN,
			UTIL_CTYPE_ES,
			UTIL_LOCALE,
			UTIL_LOCALE_ID,
			UTIL_LOCALE_FACET,
			UTIL_PLATFORM,
			UTIL_VXWORKS_UTIL,
			UTIL_PROPERTIES,
			UTIL_PROPERTIES_VALUE,
			UTIL_PROPERTIES_TREE,
			UTIL_PROPERTIES_TREE_ITERATOR,
			UTIL_LOCK,
			UTIL_MUTEX,
			UTIL_THREAD,
			UTIL_VXWORKS_TASKS,
			UTIL_DATE,
			UTIL_DATE_TIME,
			UTIL_DATE_TIME_INTERVAL,
			UTIL_TIME,
			UTIL_TIME_INTERVAL,


			HFC_PRODUCT,

			LAST_TYPE     // Keep always at the end
		} Type;

		static const char* getText(DataType::Type type);


	private:
		static char* _typeNames_[];
};



class Accountant
{
	public:
		// --- Types and subtypes ---
		class Snapshot
		{
			friend class Accountant;

			public:
				~Snapshot() { delete[] counters_; delete[] totals_; delete[] names_; }


			private:
				Snapshot(int capacity)
						:	capacity_(capacity),
							last_(0),
							counters_(new int[capacity]),
							totals_(new int[capacity]),
							names_(new const std::string*[capacity])
						{}
				Snapshot(const Snapshot& ss) : capacity_(ss.capacity_), last_(0), counters_(new int[ss.capacity_]),
					totals_(new int[ss.capacity_]), names_(new const std::string*[ss.capacity_]) { *this = ss; }

				int newCounter(const std::string& name)
						{ ++last_; counters_[last_] = 0; totals_[last_] = 0; names_[last_] = &name; return last_; }

				Snapshot& operator=(const Snapshot& rhs) { last_ = rhs.last_;
						memcpy(counters_, rhs.counters_, (last_ + 1)*sizeof(int));
						memcpy(totals_, rhs.totals_, (last_ + 1)*sizeof(int));
						memcpy(names_, rhs.names_, (last_ + 1)*sizeof(std::string*)); return *this; }

				int capacity_, last_;
				int* counters_;
				int* totals_;
				const std::string** names_;
		};


		// --- Constructors and destructor ---
		static Accountant& getInstance();


		// --- Processing methods ---
		int registerDataType(const std::string& name);
		void addInstance(int type);
		void removeInstance(int type);

		Snapshot* createSnapshotCopy();
		void showCounters(bool all);
		void showCounterDeltas(const Snapshot& snapshot, bool all);
		template <class Iter> void showSortedCounters(const Snapshot* snapshot, Iter begin, Iter end);


	protected:
		Accountant();
		~Accountant();


	private:
		int capacity_;

		Snapshot* currentCounters_;
		util_thread::Mutex<> countersMutex_;  /* (SJJC port 6/19/06) */

		static Accountant* _instance_;
		static util_thread::Mutex<> _singletonMutex_;  /* (SJJC port 6/19/06) */
};



#ifndef NO_NAMESPACES
} // namespace util_acc
#endif



#endif // ACCOUNTANT_H
