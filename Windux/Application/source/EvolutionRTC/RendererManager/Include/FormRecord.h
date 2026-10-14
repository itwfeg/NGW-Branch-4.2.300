#ifndef FORMRECORD_H
#define FORMRECORD_H

#include "Point.h"
#include "Label.h"
#include "Rectangle.h"

class BitMap;
class Blob;


class FormRecord
{
	public:
		FormRecord();
		FormRecord(INT16 x, INT16 y, INT16 w, INT16 h, UINT16 flag, FieldType type, 
				HorizontalJustify hjust, UINT16 vjust, UINT8 subtype, UINT16 rotation);
		FormRecord(FormRecord &record);		//	Copy constructor
		~FormRecord();

		Point getOrigin() const;

		INT16 getOriginVertical() const;

		INT16 getOriginHorizontal() const;

		void setOrigin(INT16 x, INT16 y);
		void setOriginHorizontal(INT16 x);
		void setOriginVertical(INT16 y);

		Point getSize() const;

		INT16 getSizeVertical() const;

		INT16 getSizeHorizontal() const;

		void setSize(INT16 x, INT16 y);
		void setSizeHorizontal(INT16 x);
		void setSizeVertical(INT16 y);

		Hobart::Rectangle getRect() const;

		UINT16 getFlag() const;

		bool isFormPreImage() const;
		bool isFormPostImage() const;

		bool isFormFlagFixed() const;
		bool isFormFlagFloating() const;
		bool isFormFlagStretch() const;
		bool isFormFlagVariable() const;
		bool isFormFieldTextEntity() const;
		bool isFormFlagBarCodeEntity() const;
		bool isFormFlagGraphicEntity() const;
		bool isFormFlagLineEntity() const;
		bool isFormFlagRectBorder() const;
		bool isFormFlagInverted() const;
		bool isFormFlagDigimarcKeepOut() const;
		bool isFormFlagCodeCheckerDisabled() const;
		bool isFormFlagDiscPrOnBarCode() const;
		void setFlag(UINT16 formFlag);

		FieldType getFieldType() const;
		void setFieldType(FieldType type);

		HorizontalJustify getHorizontalJustify() const;
		void setHorizontalJustify(HorizontalJustify justify);

		UINT16 getVerticalJustify() const;
		void setVerticalJustify(UINT16 justify);
	
		UINT8 getFieldSubType() const;
		void setFieldSubType(UINT8 subType);
	
		UINT16 getFieldRotation() const;
		void setFieldRotation(UINT16 rotate);

		const char *getFamilyName() const;
		void setFamilyName(const UINT8 *name);
		void setFamilyName(const char *name);

		UINT16 getPointSize() const;
		void setPointSize(UINT16 size);

		const char *getTag() const;
		void setTag(const UINT8 *newTag);
		void setTag(const char *newTag);

		int getDataType() const;
		void setDataType(int dataType);

		const UINT32 *getData() const;
		void setData(const UINT32 *textOrGraphic);

		const UINT8* getConstantData() const { return constData; }
		void setConstantData(const UINT8 *text);
		int getConstantDataSize() const { return constDataSize_; } // This size already includes the NULL character

		BitMap *getBitMapData() const;
		void setBitMapData(BitMap *graphicBitMap);

		const UINT8* getStream() const;
		void setStream(UINT8* stream);

		bool isDataPresent() const;

		void show() const;
	//	void setFormBitMapBase(UINT16 *base);
	// 	void setFormBitMapBounds(INT16 recLeft,INT16 recRight, INT16 recTop, INT16 recBottom);
	//	void setPitch(INT16 delta);
	//	void setFormBitMap(BitMap *bitMap);

		void writeBinaryData(Blob& binaryData, UINT32 constDataPosition);

		static int getFormRecordSize()
				{ return 4 + 4 + 2 + 1 + 1 + 2 + 1 + 1 + 2 + MaxFamilyName + 2 + MaxTagChars + 4; }


	private:

		Point origin;
		Point size;
		UINT16 flag;
		FieldType form_type;
		HorizontalJustify h_justify;
		UINT16 v_justify;
		UINT8 type;
		UINT16 rotation;

		char familyName[MaxFamilyName];
		UINT16 pointSize;
		char tag[MaxTagChars];

		UINT32 *data;
		UINT8* constData;
		
		void* dataPointer;

		int constDataSize_;
		int dataType;

		const static int DataType_Text = 0;
		const static int DataType_BitMap = 1;
		const static int DataType_ConstText = 2;
		const static int DataType_Stream = 3;

#ifdef USE_ACCOUNTANT
		static int acc_dataType;
		static std::string acc_dataTypeName;
#endif
};

#endif //FORMRECORD_H
