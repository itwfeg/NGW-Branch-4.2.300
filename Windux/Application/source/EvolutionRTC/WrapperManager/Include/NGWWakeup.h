/**
 * @file NGWWakeup.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief
 * Created on: Aug 22, 2017
 *
 */

#ifndef NGWWAKEUP_H_
#define NGWWAKEUP_H_
#include "EvolMessage.h"

class NGWWakeup : public EvolMessage
{
	public:

		typedef enum
		{
			NO_MODEL_,
			AWS_1RL_,
			AWS_1LR_,
			AWS_1MLR_,			 // AWS manual machine left to right
			AWS_1LR_I_,          // AWS international left to right
			AWS_1RL_I_,          // AWS international right to left
			AWS_1BWRL_,          // AWS Lite (defeatured) right to left
			AWS_1BWLR_,          // AWS Lite (defeatured) left to right
			AWS_2RL_,            // AWS 2 right to left
			AWS_2LR_,            // AWS 2 left to right
			AWS_3RL_,            // AWS 3(Enhanced, Two Roll) right to left
			AWS_3LR_,            // AWS 3(Enhanced, Two Roll) left to right
			EWS_3RL_,            // AWS 3(Enhanced, One Roll) right to left
			EWS_3LR_,            // AWS 3(Enhanced, One Roll) left to right
			NGW_2RL_,            // AWS 4(Small Footprint, Two Roll) right to left
			NGW_2LR_,            // AWS 4(Small Footprint, Two Roll) left to right
			NGW_1RL_,            // AWS 4(Small Footprint, One Roll) right to left
			NGW_1LR_             // AWS 4(Small Footprint, One Roll) left to right
		}WrapperModel;

		typedef enum
		{
			AUTOMATIC_,
			MANUAL_
		}WrapperOp;

		NGWWakeup(Hobart::CEvolMsgReceiver *pHobThread = NULL, WrapperModel model = NO_MODEL_, WrapperOp operation = AUTOMATIC_ );
		NGWWakeup(const char *buffer);
		~NGWWakeup();

		const char *serialize() const;
		void show();

		WrapperModel getModelNumber()	{ return model_; }
		void setModelNumber( WrapperModel model ) { model_ = model; }

		WrapperOp getOperation() { return operation_; }
		void setOperation( WrapperOp operation ) { operation_ = operation; }

	private:
		WrapperModel model_;
		WrapperOp operation_;

};
#endif
