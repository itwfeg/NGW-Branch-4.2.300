/* Dominator Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRINTEDLABELFIELDS_H
#define PRINTEDLABELFIELDS_H

#include "DateTimeType.h"

/**
 * We may want to determine a different way of handling these fields, so that it is easier in the future
 * to add additional label fields that can be configurably printed.
 *
 * If we allow the application to pass in the 'name' of the field when checking if a field should be printed,
 * and the field name is not in the list, we really don't know if it should be printed or not.  We really only
 * know that the field is not configured in this class, not whether it should be printed or not.
 */
class PrintedLabelFields
{
	protected:

	PrintedLabelFields();

	public:

	bool getSellByDate();

	void setSellByDate(bool sellByDate);

	bool getSellByDateJulian();

	void setSellByDateJulian(bool sellByDateJulian);

	bool getPackedOnDate();

	void setPackedOnDate(bool packedOnDate);

	bool getPackedOnDateJulian();

	void setPackedOnDateJulian(bool packedOnDateJulian);

	bool getPackedOnTime();

	void setPackedOnTime(bool packedOnTime);

	bool getProductLifeDate();

	void setProductLifeDate(bool productLifeDate);

	bool getProductLifeDateJulian();

	void setProductLifeDateJulian(bool productLifeDateJulian);

	bool getUnitPrice();

	void setUnitPrice(bool unitPrice);

	bool getByCount();

	void setByCount(bool byCount);

	bool getWeightOnByCount();

	void setWeightOnByCount(bool weightOnByCount);

	bool getBarCode();

	void setBarCode(bool barCode);

	bool getSecondCheckDigit();

	void setSecondCheckDigit(bool secondCheckDigit);

	bool getPreprintTextNames();

	void setPreprintTextNames(bool preprintTextNames);

	bool shouldPrint(int field);

	static PrintedLabelFields * getInstance();

	private:
	bool sellByDate;

	/**
	 * When the user is configuring the fields to print on the label, via Supervisor,
	 * this field should not show if the 'sellByDate' is set to NO.
	 */
	bool sellByDateJulian;
	bool packedOnDate;

	/**
	 * When the user is configuring the fields to print on the label, via Supervisor,
	 * this field should not show if the 'packedOnDate' is set to NO.
	 */
	bool packedOnDateJulian;

	/**
	 * This field should only print if the packedOnDate field is printed.
	 * The time would be meaningless without the corresponding date.
	 */
	bool packedOnTime;
	bool productLifeDate;

	/**
	 * When the user is configuring the fields to print on the label, via Supervisor,
	 * this field should not show if the 'productLifeDate' is set to NO.
	 */
	bool productLifeDateJulian;
	bool unitPrice;
	bool byCount;
	bool weightOnByCount;
	bool barCode;

	/**
	 * When the user is configuring the fields to print on the label, via Supervisor,
	 * this field should not show if 'barCode' is set to NO.
	 */
	bool secondCheckDigit;
	bool preprintTextNames;

	/**
	 * @link
	 * @shapeType PatternLink
	 * @pattern <{Singleton}>
	 * @supplierRole Singleton factory
	 */

    /*# PrintedLabelFields _printedLabelFields; */

	static PrintedLabelFields * instance;
};

#endif //PRINTEDLABELFIELDS_H
