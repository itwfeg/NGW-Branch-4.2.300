/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TARE_H
#define TARE_H

#include "evolution.h"


class Tare
{
	public:
		Tare(const Tare&);
		Tare();
		Tare(HobWeight val);

		~Tare();

		HobWeight getValue() const;
		void setValue(HobWeight value);

		bool isForced() const;
		void setForced(bool forced);


	private:

		HobWeight value;

		bool forced;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //TARE_H
