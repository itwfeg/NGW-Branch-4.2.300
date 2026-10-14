/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FLASHKEY_H
#define FLASHKEY_H

#include "NumType.h"
#include "StringType.h"
#include "Data.h"

class Flashkey : Data
{
	public:

	NumType & getNum();

	void setNum(NumType & num);

	int getFkeyType();

	void setFkeyType(int fkeyType);

	short getVerticalSize();

	void setVerticalSize(short verticalSize);

	short getHorizontalSize();

	void setHorizontalSize(short horizontalSize);

	NumType & getRecordNum();

	void setRecordNum(NumType & recordNum);

	StringType & getDescription();

	void setDescription(StringType & description);

	private:

	NumType num;

	/**
	 * from new_db.h (Quantum)
	 *
	 * THE FOLLOWING DEFINES ARE REDIFINITIONS OF THE RECORD TYPE
	 * FOR A FLASHKEY. ORIGINALLY THE RECORD TYPES USED WERE FieldTags.
	 * THERE IS A POSSIBILITY THAT THE FieldTags COULD BE SHIFTED NUMERICALLY.
	 * THIS COULD RESULT IN AN INCOMPATIBILITY BETWEEN VERSIONS. SO, TO PLAY IT
	 * SAFE, THEY ARE RENAMED BUT DEFINED AS THE CURRENT FieldTag VALUES. (CCB)
	 *
	 * #define PRODUCT_FLASHKEY    2        // FORMERLY _PRODUCT_NUMBER
	 * #define OPERATOR_FLASHKEY   20    //  FORMERLY _OPERATOR_ID
	 * #define MACRO_FLASHKEY      182     //  FORMERLY _MACRO_NUMBER
	 * #define GROUP_FLASHKEY      214      //  NEW
	 */
	int fkeyType;
	short verticalSize;
	short horizontalSize;

	/**
	 * Used to indicate which record to lookup when the user selects this flash key.
	 * Used in combination with the flash key type (see corresponding description).
	 * For example, this would be the plu number, or the operator id.
	 */
	NumType recordNum;
	StringType description;
};

#endif //FLASHKEY_H
