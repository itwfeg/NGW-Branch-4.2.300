/**
 * @file WeigherSystemRequestValueMax.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Doug Antonides
 * @brief Value Max FIT Parameters
 * Created on: Jan 31, 2020
 *
 **/

#ifndef WEIGHERSYSTEMREQUESTVALUEMAX_H
#define WEIGHERSYSTEMREQUESTVALUEMAX_H

#include "Weigher.h"
#include "EvolMessage.h"


class WeigherSystemRequestValueMax : public EvolMessage
{
public:

	WeigherSystemRequestValueMax(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	WeigherSystemRequestValueMax(const char *buffer);

	~WeigherSystemRequestValueMax();

	const char *serialize() const;

	/**
	* @brief getRequestFITParams
	*
	* @return the FIT Params requested
	*/
	UINT16 getRequestFITParams() const { return _fitParamRequested; }

	/**
	* @brief setRequestFITParams
	* Sets the FIT Params requested to the value passed in
	* @param type
	*/
	void setRequestFITParams(UINT16 fitRequestParam){ _fitParamRequested = fitRequestParam; }

	private:

		UINT16 _fitParamRequested;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif //WEIGHERSYSTEMREQUESTVALUEMAX_H
