/*
 * NGWRequestHeightBox.h
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */

#ifndef NGWREQUESTHEIGHTBOX_H_
#define NGWREQUESTHEIGHTBOX_H_
#include "WrapperGeneric.h"

class NGWRequestHeightBox : public WrapperGeneric
{

	public:
	NGWRequestHeightBox(Hobart::CEvolMsgReceiver *pHobThread = NULL);
	NGWRequestHeightBox(const char *buffer);
	~NGWRequestHeightBox();

};

#endif /* NGWREQUESTHEIGHTBOX_H_ */
