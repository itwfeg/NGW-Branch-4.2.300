#ifndef HEADERRECORD_H
#define HEADERRECORD_H



#include "HfcObject.h"


#include "Scale.h"


#include "HobDateTime.h"


#include "tags/Tags.h"



namespace hfc
{


	class SynchronizationChange;


	class HeaderRecord : public HfcObject
	{
		public:
			/* --- Types and subtypes --- */
			typedef enum
			{
				TypeInvalid,

				TypeHQComm,
				TypeSyncComm,

				TypeLast
			} DataType;


			/* --- Constructors and destructor --- */
			HeaderRecord();
			HeaderRecord(const char * data, int len, int formatType);
			~HeaderRecord();


			/* --- Getters and setters --- */
			DataType getDataType() const { return dataType_; }
			void setDataType(DataType dt) { dataType_ = dt; }

			hfc::Scale::ScaleRole getScaleRole() const { return scaleRole_; }
			void setScaleRole(hfc::Scale::ScaleRole st) { scaleRole_ = st; }

			const std::string& getHostName() const { return hostname_; }
			void setHostName(const std::string& hn) { hostname_ = hn; }

			const std::string& getIPAddress() const { return ipAddress_; }
			void setIPAddress(const std::string& ip) { ipAddress_ = ip; }

			const std::string& getCurrrentDateTime() const { return currentDateTime_; }
			void setCurrentDateTime() { setCurrentDateTime(HobDateTime().convertToDBFormat()); }
			void setCurrentDateTime(const std::string & dt) { currentDateTime_ = dt; }


			/* --- Instance function members --- */
			const char * serialize(int format) const;


			/* --- Class function members --- */
			static void initFieldList();

			static const std::string& getDataTypeName(DataType dt);


		private:
			/* --- Data members --- */
			DataType dataType_;
			hfc::Scale::ScaleRole scaleRole_;
			std::string hostname_;
			std::string ipAddress_;
			std::string currentDateTime_;


			/* --- Class Data Members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
			static FieldOffsetsList _fieldList_;

			static const std::string _datatypeHQCommName_;
			static const std::string _datatypeSyncCommName_;
			static const std::string _datatypeUnknownName_;


			/* --- Constructors --- */


			/* --- Instance function members --- */
			void init();
	};


}



#endif /* HEADERRECORD_H */
