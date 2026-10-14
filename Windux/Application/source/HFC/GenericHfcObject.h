/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef GENERICHFCOBJECT_H
#define GENERICHFCOBJECT_H

#include "evolution.h"
#include "HfcObject.h"

/**
 * This class is used to just define some generic message to send.
 * It allows us to send data of any format, without needing to
 * translate the data either to or from char strings.
 *
 */
class GenericHfcObject : public HfcObject
{
	public:
		GenericHfcObject();
		GenericHfcObject(const char *initialData);
		GenericHfcObject(const std::string &initialData);
		GenericHfcObject(const char *data, int len, int formatType);

		~GenericHfcObject();

		virtual const char *serialize(int format) const;

		const HobString &getData() const;
		void setData(const HobString &data);

	private:
		HobString data;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //GENERICHFCOBJECT_H




