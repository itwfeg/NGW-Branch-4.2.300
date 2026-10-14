#include "FormRecord.h"
#include "Blob.h"



#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h" 

#include "utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int FormRecord::acc_dataType = 0;
std::string FormRecord::acc_dataTypeName("FormRecord"); 
#endif


FormRecord::FormRecord() : flag(0), form_type(STORE_NAME),
		h_justify(FullJustify), v_justify(0), type(0), rotation(0), pointSize(0),
		data(0), constData(0), dataPointer(0), constDataSize_(0), dataType(DataType_Text)
{
#ifdef USE_ACCOUNTANT
	if (acc_dataType == 0)
		acc_dataType = util_acc::registerDataType(acc_dataTypeName);

	util_acc::addInstance(acc_dataType);
#endif

 	Debug::trace("FormRecord", "ctor");

	memset(familyName, ' ', MaxFamilyName);
	memset(tag, ' ', MaxTagChars);
}


FormRecord::FormRecord(INT16 x, INT16 y, INT16 w, INT16 h, UINT16 f, FieldType type, 
		HorizontalJustify hjust, UINT16 vjust, UINT8 subtype, UINT16 rot)
		: origin(x, y), size(w, h), flag(f), form_type(type), h_justify(hjust),
		v_justify(vjust), type(subtype), rotation(rot), pointSize(0), data(0),
		constData(0), dataPointer(0), constDataSize_(0), dataType(DataType_Text)
{
#ifdef USE_ACCOUNTANT
	if (acc_dataType == 0)
		acc_dataType = util_acc::registerDataType(acc_dataTypeName);

	util_acc::addInstance(acc_dataType);
#endif

 	Debug::trace("FormRecord", "ctor");

 	memset(familyName, ' ', MaxFamilyName);
	memset(tag, ' ', MaxTagChars);
}

//	GCDR-212: Created a copy constructor to avoid pointer issues [CONSTANT_TEXT] raised with the default copy constructor.
FormRecord::FormRecord(FormRecord &record)
{
#ifdef USE_ACCOUNTANT
	if (acc_dataType == 0)
		acc_dataType = util_acc::registerDataType(acc_dataTypeName);

	util_acc::addInstance(acc_dataType);
#endif

 	Debug::trace("FormRecord", "ctor");

 	data = 0;
	constData = 0;
	dataPointer = 0;
	constDataSize_ = 0;
	memset(familyName, ' ', MaxFamilyName);
	memset(tag, ' ', MaxTagChars);

	setOrigin(record.getOriginHorizontal(), record.getOriginVertical());
	setSize(record.getSizeHorizontal(), record.getSizeVertical());
	setFlag(record.getFlag());
	setFieldType(record.getFieldType());
	setHorizontalJustify(record.getHorizontalJustify());
	setVerticalJustify(record.getVerticalJustify());
	setFieldSubType(record.getFieldSubType());
	setFieldRotation(record.getFieldRotation());
	setFamilyName(record.getFamilyName());
	setPointSize(record.getPointSize());
	setTag(record.getTag());

	setDataType(record.getDataType());		//	Make sure this is set before we set data or constData.
	if (dataType == DataType_ConstText)
	{
		setConstantData((UINT8 *) record.getData());
	}
	else if (dataType == DataType_BitMap)
	{
		setBitMapData(record.getBitMapData());
	}
	else if (dataType == DataType_Stream)
	{
		setStream((UINT8*)record.getStream());
	}
	else
	{
		setData(record.getData());
	}
}

FormRecord::~FormRecord()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(acc_dataType);
#endif

 	Debug::trace("FormRecord", "dtor");

 	if ( data )
		delete[] data;
	if ( constData )
		delete[] constData;
	// the dataPointer does not need to be deleted, this FormRecord does not 'own' the pointer.
}

Point FormRecord::getOrigin() const
{
	return origin;
}

INT16 FormRecord::getOriginVertical() const
{
	return origin.getVertical();
}

INT16 FormRecord::getOriginHorizontal() const
{
	return origin.getHorizontal();
}

void FormRecord::setOrigin(INT16 x, INT16 y)
{
	origin.setHorizontal(x);
	origin.setVertical(y);
}
void FormRecord::setOriginHorizontal(INT16 x)
{
	origin.setHorizontal(x);
}
void FormRecord::setOriginVertical(INT16 y)
{
	origin.setVertical(y);
}

Point FormRecord::getSize() const
{
	return size;	
}

INT16 FormRecord::getSizeVertical() const
{
	return size.getVertical();
}

INT16 FormRecord::getSizeHorizontal() const
{
	return size.getHorizontal();
}

void FormRecord::setSize(INT16 x, INT16 y)
{
	size.setHorizontal(x);
	size.setVertical(y);
}

void FormRecord::setSizeHorizontal(INT16 x)
{
	size.setHorizontal(x);
}

void FormRecord::setSizeVertical(INT16 y)
{
	size.setVertical(y);
}

Hobart::Rectangle FormRecord::getRect() const
{
	Hobart::Rectangle rect;
	rect.setRectangleTop( getOriginVertical() );
	rect.setRectangleBottom( getOriginVertical() + getSizeVertical() );
	rect.setRectangleLeft( getOriginHorizontal() );
	rect.setRectangleRight( getOriginHorizontal() + getSizeHorizontal() );

	return rect;
}

UINT16 FormRecord::getFlag() const
{
	return flag;
}

bool FormRecord::isFormPreImage() const
{
	return ((flag & PreImage) == PreImage);
}

bool FormRecord::isFormPostImage() const
{
	return ((flag & PostImage) == PostImage);
}

bool FormRecord::isFormFlagFixed() const
{
	return ((flag & Fixed) == Fixed);
}

bool FormRecord::isFormFlagFloating() const
{
	return ((flag & Floating) == Floating);
}

bool FormRecord::isFormFlagStretch() const
{
	return ((flag & Stretch) == Stretch);
}

bool FormRecord::isFormFlagVariable() const
{
	return ((flag & Variable) == Variable);
}

bool FormRecord::isFormFieldTextEntity() const
{
	return ((flag & TextEntity) == TextEntity);
}

bool FormRecord::isFormFlagBarCodeEntity() const
{
	return ((flag & BarcodeEntity) == BarcodeEntity);
}

bool FormRecord::isFormFlagGraphicEntity() const
{
	return ((flag & GraphicEntity) == GraphicEntity);
}

bool FormRecord::isFormFlagLineEntity() const
{
	return ((flag & LineEntity) == LineEntity);
}

bool FormRecord::isFormFlagRectBorder() const
{
	return ((flag & RectBorder) == RectBorder);
}

bool FormRecord::isFormFlagInverted() const
{
	return ((flag & Inverted) == Inverted);
}

bool FormRecord::isFormFlagDigimarcKeepOut() const
{
	return ((flag & DigimarcKeepOut) == DigimarcKeepOut);
}

bool FormRecord::isFormFlagCodeCheckerDisabled() const
{
	return ((flag & CodeCheckerDisabled) == CodeCheckerDisabled);
}

bool FormRecord::isFormFlagDiscPrOnBarCode() const
{
	// SOF-4682 Raley's embed FS price in barcode

	if (flag)
	{
		Debug::trace("FormRecord.DiscountPrice", "FormRecord::isFormFlagDiscPrOnBarCode() flag in label: " + util_string::inttostring(flag) + "     DiscPrOnBarCode: " + util_string::inttostring( DiscPrOnBarCode) );
	    return ((flag & DiscPrOnBarCode) == DiscPrOnBarCode);
	}
	else
	{
		Debug::trace("FormRecord.DiscountPrice", "FormRecord::isFormFlagDiscPrOnBarCode() flag is null: ");
		return false;
	}

	return false;
}

void FormRecord::setFlag(UINT16 formFlag)
{
	flag = formFlag;
}

FieldType FormRecord::getFieldType() const
{
	return form_type;
}
void FormRecord::setFieldType(FieldType type)
{
	form_type = type;	
}

HorizontalJustify FormRecord::getHorizontalJustify() const
{
	return h_justify;
}
void FormRecord::setHorizontalJustify(HorizontalJustify justify)
{
	h_justify = justify;
}

UINT16 FormRecord::getVerticalJustify() const
{
	return v_justify;
}
void FormRecord::setVerticalJustify(UINT16 justify)
{
	v_justify = justify;
}

UINT8 FormRecord::getFieldSubType() const
{
	return type;
}
void FormRecord::setFieldSubType(UINT8 subType)
{
	type = subType;
}

UINT16 FormRecord::getFieldRotation() const
{
	return rotation;
}
void FormRecord::setFieldRotation(UINT16 rotate)
{
	rotation = rotate;
}

const char *FormRecord::getFamilyName() const
{
	return familyName;
}
void FormRecord::setFamilyName(const UINT8 *name)
{
	memcpy(familyName, name, MaxFamilyName);	
}
void FormRecord::setFamilyName(const char *name)
{
	memcpy(familyName, name, MaxFamilyName);	
}

UINT16 FormRecord::getPointSize() const
{
	return pointSize;
}
void FormRecord::setPointSize(UINT16 size)
{
	pointSize = size;
}

const char *FormRecord::getTag() const
{
	return tag;
}
void FormRecord::setTag(const UINT8 *newTag)
{
	memcpy(tag, newTag, MaxTagChars);
}
void FormRecord::setTag(const char *newTag)
{
	memcpy(tag, newTag, MaxTagChars);	
}

int FormRecord::getDataType() const
{
	return dataType;
}
void FormRecord::setDataType(int dataType)
{
	if ((dataType < DataType_Text) || (dataType > DataType_Stream))
		dataType = DataType_Text;
	this->dataType = dataType;
}

const UINT32 *FormRecord::getData() const
{
	if (dataType == DataType_ConstText)
		return (UINT32*)constData;
	else
		return data;
}

void FormRecord::setConstantData(const UINT8 *text)
{
	if ( constData )
	{
		delete[] constData;
		constData = 0;
		constDataSize_ = 0;
	}

	if ( text )
	{
		int len = (int) strlen((const char *)text);
		if ( len )
		{
			++len;    // This is to account for the NULL character
			constData = new UINT8[len];
			if ( constData )
			{
				memcpy(constData, text, len);
				constDataSize_ = len;
			}
		}
	}

	dataType = DataType_ConstText;
}



void FormRecord::setData(const UINT32* text)
{
	if ( data )
	{
		delete[] data;
		data = 0;
	}

	if ( text )
	{
		int len = (int) strlen((const char *)text);
		if ( len )
		{
			++len;    // This is to account for the NULL character
			data = (unsigned int*)new char[len];
			if ( data )
				memcpy(data, text, len);
		}
	}

	dataType = DataType_Text;
}


BitMap *FormRecord::getBitMapData() const
{
	return (BitMap*)dataPointer;
}


void FormRecord::setBitMapData(BitMap *graphicBitMap)
{
	dataPointer = graphicBitMap;
	dataType = DataType_BitMap;
}


const UINT8* FormRecord::getStream() const
{
	return (const UINT8*)dataPointer;
}


void FormRecord::setStream(UINT8* stream)
{
	dataPointer = stream;
	dataType = DataType_Stream;
}


bool FormRecord::isDataPresent() const
{
	return ((dataType == DataType_BitMap && dataPointer) ||
			(dataType == DataType_Stream && dataPointer) ||
			(dataType == DataType_Text && data) ||
			(dataType == DataType_ConstText && constData));
}


void FormRecord::show() const
{
	bool isTracing = Debug::isTracing("FormRecord");

	if (isTracing == false)
	{
		Debug::addTraceCategory( std::string("FormRecord") );
	}

 	Debug::trace("FormRecord", 24, "Form Record:");

 	Debug::trace("FormRecord", 24, "   Origin: (" + util_string::inttostring(origin.getHorizontal()) +
 			"," + util_string::inttostring(origin.getVertical()) + ")\n" +
 			"   Size: (" + util_string::inttostring(size.getHorizontal()) + "," + util_string::inttostring(size.getVertical()) + ")");

	{
		std::string flags;
		if (isFormFlagCodeCheckerDisabled())
			flags.append("CODECHECKERDISABLED|");
		if (isFormFlagDigimarcKeepOut())
			flags.append("DIGIMARCKEEPOUT|");
		if (isFormFlagInverted())
			flags.append("INVERTED|");
		if ( isFormFlagRectBorder() )
			flags.append("RECT_BORDER|");
		if ( isFormFlagVariable() )
			flags.append("VARIABLE|");
		if ( isFormFlagStretch() )
			flags.append("STRETCH|");
		if ( isFormFlagFloating() )
			flags.append("FLOATING|");
		if ( isFormFlagFixed() )
			flags.append("FIXED|");
		if ( isFormPostImage() )
			flags.append("POST_IMAGE|");
		if ( isFormPreImage() )
			flags.append("PRE_IMAGE|");
		if ( isFormFlagLineEntity() )
			flags.append("LINE_ENTITY|");
		if ( isFormFlagGraphicEntity() )
			flags.append("GRAPHIC_ENTITY|");
		if ( isFormFlagBarCodeEntity() )
			flags.append("BARCODE_ENTITY|");
		if ( isFormFieldTextEntity() )
			flags.append("TEXT_ENTITY|");
		if ( isFormFlagDiscPrOnBarCode() )
			flags.append("DISCPR_ON_BARCODE");

	 	Debug::trace("FormRecord", 24, "   Flags: " + flags + "\n" +
	 			"   Type: " + util_string::inttostring(form_type) + "\n" +
			 	"   SubType: " + util_string::inttostring(type));

	}

	{
		std::string verticalJustification(" Vertical Justification: ");
		verticalJustification.append(util_string::inttostring(v_justify));

		if (h_justify == LeftJustify)
		{
		 	Debug::trace("FormRecord", 24, "   Hori Justification: LeftJustify" + verticalJustification);
		}
		else if (h_justify == CenterJustify)
		{
		 	Debug::trace("FormRecord", 24, "   Horizontal Justification: CenterJustify" + verticalJustification);
		}
		else if (h_justify == RightJustify)
		{
		 	Debug::trace("FormRecord", 24, "   Horizontal Justification: RightJustify" + verticalJustification);
		}
		else if (h_justify == FullJustify)
		{
		 	Debug::trace("FormRecord", 24, "   Horizontal Justification: FullJustify" + verticalJustification);
		}
		else if (h_justify == FlowJustify)
		{
		 	Debug::trace("FormRecord", 24, "   Horizontal Justification: FlowJustify" + verticalJustification);
		}
		else
		{
		 	Debug::trace("FormRecord", 24, "   Horizontal Justification: unknown -- " + HobString(Integer::toString(h_justify)) + verticalJustification);
		}
	}

	Debug::trace("FormRecord", 24, "   Font: family name " + std::string((const char *) familyName, MaxFamilyName) +
 			" point size " + util_string::inttostring(pointSize) + "\n" +
 			"   Rotation: " + util_string::inttostring(rotation) + "\n" +
			"   Tag: " + std::string((const char *) tag, MaxTagChars));

	if ( data )
	{
		if ( constData )
			Debug::trace("FormRecord", 24, "   Data: pointer " + util_string::ptrtostring(data) +
			" value '" + std::string((const char *) data) + "'\n" +
					"   ConstData: pointer " + util_string::ptrtostring(constData) +
					" value '" + std::string((const char *) constData) + "'");
		else
			Debug::trace("FormRecord", 24, "   Data: pointer " + util_string::ptrtostring(data) +
			" value '" + std::string((const char *) data) + "'\n" +
			"   ConstData: pointer " + util_string::ptrtostring(constData));
	}
	else
	{
		if ( constData )
			Debug::trace("FormRecord", 24, "   Data: pointer " + util_string::ptrtostring(data) + "\n" +
			"   ConstData: pointer " + util_string::ptrtostring(constData) +
			" value '" + std::string((const char *) constData) + "'");
		else
			Debug::trace("FormRecord", 24, "   Data: pointer " + util_string::ptrtostring(data) + "\n" +
			"   ConstData: pointer " + util_string::ptrtostring(constData));

	}

	if (isTracing == false)
	{
		Debug::removeTraceCategory( std::string("FormRecord") );
	}
}


void FormRecord::writeBinaryData(Blob& binaryData, UINT32 constDataPosition)
{
	UINT16 temp = binaryData.ShortSwapBytes(origin.getHorizontal());
	binaryData.append(temp);
	temp = binaryData.ShortSwapBytes(origin.getVertical());
	binaryData.append(temp);

	temp = binaryData.ShortSwapBytes(size.getHorizontal());
	binaryData.append(temp);
	temp = binaryData.ShortSwapBytes(size.getVertical());
	binaryData.append(temp);

	temp = binaryData.ShortSwapBytes(flag);
	binaryData.append(temp);

	binaryData.append((UINT8)form_type);

	binaryData.append((UINT8)h_justify);

	temp = binaryData.ShortSwapBytes(v_justify);
	binaryData.append(temp);

	binaryData.append(type);
	binaryData.append((UINT8)0);

	temp = binaryData.ShortSwapBytes(rotation);
	binaryData.append(temp);

	binaryData.append((UINT8*)familyName, MaxFamilyName);

	temp = binaryData.ShortSwapBytes(pointSize);
	binaryData.append(temp);

	binaryData.append((UINT8*)tag, MaxTagChars);

	if ( constData )
	{
		UINT32 temp = binaryData.ULongSwapBytes(constDataPosition);
		binaryData.append(temp);
	}
	else
		binaryData.append((UINT32)0);
}
