#ifndef DBACCESSENTITY_H
#define DBACCESSENTITY_H


#include "edb.h"


#ifndef NO_NAMESPACES
namespace edb
{
#endif



	class DBManager;


	class DBAccessEntity
	{
		public:

			// --- Types and subtypes ---
			enum DBAccessEntityType
			{
				DBAccessEntityInvalid,

				DBAccessEntityRecordManager,
				DBAccessEntityQuery,

				DBAccessEntityLast
			};

			enum DBAccessType
			{
				DBAccessInvalid,

				DBAccessDatabaseOperation,

				DBAccessOpenTable,
				DBAccessCreateRecord,
				DBAccessSemiautoIncrement,
				DBAccessFindRecord,
				DBAccessRetrieveRecords,
				DBAccessCountAllRecords,
				DBAccessCalculateAverageSpace,
				DBAccessSaveRecord,
				DBAccessDeleteRecord,
				DBAccessDeleteAllRecords,
				DBAccessCloseTable,

				DBAccessQueryBegin,
				DBAccessQueryFirst,
				DBAccessQueryReverseBegin,
				DBAccessQueryPrevious,
				DBAccessQueryNext,
				DBAccessQueryLast,
				DBAccessQuerySize,
				DBAccessQueryEmpty,
				DBAccessQueryDispose,

				DBAccessLast
			};

			enum DBRecordType
			{
				InvalidType,

				WholeDB,

				Batches,
				BarcodeScanners,
				Categories,
				CategoryDetailedStatistics,
				CategoryStatistics,
				Classes,
				Configurations,
				CoolCategories,
				CoolCategoryCountries,
				CoolCategoryPreTexts,
				CoolCategoryProductionSteps,
				CoolCountries,
				CoolLists,
				CoolListTexts,
				CoolPreTexts,
				CoolProductionSteps,
				CoolTexts,
				CuttingTests,
				Depts,
				Devices,
				EmailAlerts,
				ExpirationLabels,
				FlashFiles,
				Labels,
				LabelTexts,
				Marquees,
				Messages,
				NetworkErrors,							// will go away
				Notes,
				Nutritions,
				Operators,
				OperatorNotes,
				PresentationMethods,
				PresentationMethodDetailedStatistics,
				PresentationMethodStatistics,
				Products,
				ProductDefaults,
				ProductDetailedStatistics,
				ProductStatistics,
				ProductTexts,
				ProductTextCfgs,
				Recalls,
				ROMLabels,
				Scales,
				SoftKeys,
				SoftKeyFeatures,
				SoftKeyGroups,
				SoftKeyStyles,
				SpecialPrograms,
				Status,
				StreamingGraphics,
				SynchronizationChanges,
				Texts,
				Transactions,
				TransactionTypes,
				WrapperErrorHistory,
				WrapperKnife,

				LastRecordType
			};


			// --- Constructors and destructor ---
			virtual ~DBAccessEntity() {}


			// --- Accessor methods ---
			virtual DBAccessEntityType getAccessEntityType() const = 0;
			virtual bool isEnabled() const = 0;
			virtual DBManager* getDBManager() const = 0;
			virtual DBAccessEntity::DBRecordType getRecordType() const = 0;
	};


#ifndef NO_NAMESPACES
} // namespace edb
#endif


#endif // DBACCESSENTITY_H
