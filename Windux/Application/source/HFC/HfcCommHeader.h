#ifndef HFCCOMMHEADER_H
#define HFCCOMMHEADER_H



#include "HfcObject.h"
#include "HobDateTime.h"


#include "tags/Tags.h"


namespace hfc
{


	class SynchronizationChange;


	class HfcCommHeader : public HfcObject
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
			HfcCommHeader();
			HfcCommHeader(const char * data, int len, int formatType);
			~HfcCommHeader();


			/* --- Getters and setters --- */
			DataType getDataType() const { return dataType_; }
			void setDataType(DataType dt) { dataType_ = dt; }

			const std::string& getHostname() const { return hostname_; }
			void setHostname(const std::string& hn) { hostname_ = hn; }

			const std::string& getIPAddress() const { return ipAddress_; }
			void setIPAddress(const std::string& ip) { ipAddress_ = ip; }

			HobDateTime getCurrrentDateTime() const { return currentDateTime_; }
			void setCurrrentDateTime(const HobDateTime & dt = HobDateTime()) { currentDateTime_ = dt; }
			void setCurrrentDateTime(const std::string & dt) { currentDateTime_.convertFromDBFormat(dt); }


			/* --- Instance function members --- */
			const char * serialize(int format) const;


			/* --- Class function members --- */
			static void initFieldList();

			static const std::string& getDataTypeName(DataType dt);


		private:
			/* --- Data members --- */
			DataType dataType_;
			std::string hostname_;
			std::string ipAddress_;
			HobDateTime currentDateTime_;


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



#endif /* HFCCOMMHEADER_H */
