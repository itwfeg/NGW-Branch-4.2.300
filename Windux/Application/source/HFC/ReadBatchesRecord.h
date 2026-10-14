#ifndef READBATCHESRECORD_H
#define READBATCHESRECORD_H



#include "HfcObject.h"


#include "Scale.h"


#include "tags/Tags.h"



namespace hfc
{


	class SynchronizationChange;


	class ReadBatchesRecord : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				TypeInvalid,

				TypeLocal,
				TypeRemote,

				TypeLast
			} BatchType;


			/* --- Constructors and destructor --- */
			ReadBatchesRecord(BatchType type, int referenceVersion);
			ReadBatchesRecord(const char * data, int len, int formatType);
			~ReadBatchesRecord();


			/* --- Getters and setters --- */
			BatchType getBatchType() const { return batchType_; }
			void setBatchType(BatchType bt) { batchType_ = bt; }

			int getReferenceVersion() const { return referenceVersion_; }
			void setReferenceVersion(int rv) { referenceVersion_ = rv; }


			/* --- Instance function members --- */
			const char * serialize(int format) const;


			/* --- Class function members --- */
			static void initFieldList();

			static const std::string& getBatchTypeName(BatchType bt);


		private:
			/* --- Data members --- */
			BatchType batchType_;
			int referenceVersion_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
			static FieldOffsetsList _fieldList_;

			static const std::string _batchTypeLocalName_;
			static const std::string _batchTypeRemoteName_;
			static const std::string _batchTypeUnknownName_;


			/* --- Constructors --- */


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* READBATCHESRECORD_H */
