
#ifndef EVOLWRAPPERSYSTEMSTATUS_H
#define EVOLWRAPPERSYSTEMSTATUS_H

#include <vector>
#include "SystemStatus.h"
#include "Wrapper.h"
#include "EvolWrapperNode.h"

class HobThread;


class EvolWrapperSystemStatus : public EvolMessage
{
	public:
		static const unsigned int _maxBoards_ = 6;

		typedef std::vector<EvolWrapperNode> WrNodeContainer_type;
		typedef WrNodeContainer_type::iterator iterator;
		typedef WrNodeContainer_type::const_iterator const_iterator;

		EvolWrapperSystemStatus(Hobart::CEvolMsgReceiver *pHobThread = 0);
		EvolWrapperSystemStatus(const char *buffer);
		EvolWrapperSystemStatus(const EvolWrapperSystemStatus &ewss);
		~EvolWrapperSystemStatus();

		unsigned int getReportSysError() const { return reportSysError; }
		void setReportSysError(unsigned int value) { reportSysError = (UINT16)value; }

		EvolWrapperNode& getNode(unsigned int index) { return node_[index]; }
		void setNode(EvolWrapperNode& node, unsigned int index) { node_[index] = node; }

		unsigned int getNodeCount() const { return (unsigned int) node_.size(); }

		const char *serialize()const;

		void show() const;


	private:
		WrapperKind				wrKind;
		UINT16					reportSysError;
		WrNodeContainer_type	node_;

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif //EVOLWRAPPERSYSTEMSTATUS_H
