/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef STREAMINGGRAPHIC_H
#define STREAMINGGRAPHIC_H

#include "evolution.h"
#include "HfcObject.h"
#include <list>

class DBAccess;
namespace edb 
{
class DBStreamingGraphic;
}

typedef enum StreamingGraphicFieldOffsets
{
	fieldOffsetSGNum = 1,
	fieldOffsetSGNextNum,
	fieldOffsetSGImage,
	fieldOffsetSGDisplayTime,
	fieldOffsetSGActionType,
	fieldOffsetSGActionParameter,
	endStreamingGraphicFieldOffsets
}   StreamingGraphicFieldOffsets_t;


class StreamingGraphic : public HfcObject
{
	public:
		StreamingGraphic();
		StreamingGraphic(const char *data, int len, int formatType);
		StreamingGraphic(edb::DBStreamingGraphic *dbStreamingGraphic, DBAccess *access = (DBAccess *)NULL);

		~StreamingGraphic();


		HobRecordNum getStreamingGraphicNum() const;
		void setStreamingGraphicNum(const HobRecordNum num);

		HobRecordNum getNextStreamingGraphicNum() const;
		void setNextStreamingGraphicNum(const HobRecordNum num);

		const HobString &getImageFileName() const;
		void setImageFileName(const HobString &name);

		// in seconds
		int getDisplayTime() const;
		void setDisplayTime(const int time);

		int getActionType() const;
		void setActionType(const int type);

		const HobString &getActionParameter() const;
		void setActionParameter(const HobString &parameter);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode code);


		void update(StreamingGraphic *streamingGraphic); 
		void updateStreamingGraphic(const StreamingGraphic *newStreamingGraphic);

		virtual const char *serialize(int format) const;


		void setDBAccess(DBAccess *dbAccess);

		bool isRecordValid();
		
		int deleteRecord();
		void saveRecord(); 


		static StreamingGraphic *findStreamingGraphic(HobRecordNum sgNum, DBAccess *dbAccess);

		static void initFieldList();

		void show();


	private:

		HobRecordNum streamingGraphicNum;

		HobRecordNum nextStreamingGraphicNum;

		// the name of the file to display; does not need to contain the directory name,
		// nor the extension (if it conforms to the configurable default extension).
		HobString imageFileName;

		// in seconds
		int displayTime;

		int actionType;

		HobString actionParameter;

		HobErrorCode errorCode;


		DBAccess *dbAccess;

		edb::DBStreamingGraphic *myDbRecord; 

		static std::list<StreamingGraphicFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //STREAMINGGRAPHIC_H




