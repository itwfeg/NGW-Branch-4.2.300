#ifndef OPERATORTOTAL_H
#define OPERATORTOTAL_H



#include "Total.h"


#include "tags/Tags.h"



namespace hfc
{


	class OperatorTotal : public Total
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				fieldOffsetTotalOperatorNum = Total::endTotalFieldOffsets,

				endOperatorTotalFieldOffsets
			} OperatorTotalFieldOffsets;


			/* --- Constructors and destructor --- */
			OperatorTotal(const std::string id);
			~OperatorTotal();


			/* --- Getter and setter methods --- */
			const std::string& getOperatorId() const { return operatorId_; }
			void setOperatorId(const std::string& operatorId) { operatorId_ = operatorId; }


			/* --- Instance function members --- */
			void getFieldAsString(int fieldCode, HobString &result, int format) const;
			const char *serialize(int format) const;

			bool isRecordValid() const { return true; }


		private:
			/* --- Instance data members --- */
			std::string operatorId_;


			/* --- Instance function members --- */
			void init();


			/* --- Class data members --- */
			static FieldOffsetsList _fieldList_;

#ifdef USE_ACCOUNTANT
			static int dataType;
			static std::string dataTypeName;
#endif


			/* --- Class function members -- */
			static void initFieldList();
	};


}



#endif //OPERATORTOTAL_H
