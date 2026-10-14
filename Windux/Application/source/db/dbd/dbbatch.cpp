#include "edb.h"
#include DBBATCH_INC


#include <ostream>


#ifndef NO_NAMESPACES
namespace edb
{
	using namespace std;
#endif



DBBatch::DBBatch(edb_adapter::DBBatchI* d)
		: decorated_(d)
{
}


DBBatch::DBBatch(const DBBatch& rec)
		: decorated_(rec.decorated_->clone())
{
}


DBBatch::~DBBatch()
{
	if ( decorated_ )
		delete decorated_;
}


std::ostream& operator<<(std::ostream& os, const DBBatch& rec)
{
	os << "DBBatch Number " << rec.getNumber() << " (" << (rec.isRecordValid()?"valid":"not valid") << ") [";
	if ( !rec.isScaleIdNull() )
		os << ", ScaleId: " << rec.getScaleId();
	if ( !rec.isVersionNull() )
		os << ", Version: " << rec.getVersion();
	if ( !rec.isRemoteVersionNull() )
		os << ", RemoteVersion: " << rec.getRemoteVersion();
	if ( !rec.isOriginNull() )
		os << ", Origin: " << rec.getOrigin();
	if ( !rec.isTypeNull() )
		os << ", Type: " << rec.getType();
	if ( !rec.isCreatedDateTimeNull() )
		os << ", CreatedDateTime: " << rec.getCreatedDateTime();
	if ( !rec.isCompletedNull() )
		os << ", Completed: " << rec.getCompleted();
	if ( !rec.isCompletedDateTimeNull() )
		os << ", CompletedDateTime: " << rec.getCompletedDateTime();
	if ( !rec.isSignatureNull() )
		os << ", Signature: " << rec.getSignature();
	if ( !rec.isPendingApplicationNull() )
		os << ", PendingApplication: " << rec.getPendingApplication();
	if ( !rec.isAppliedDateTimeNull() )
		os << ", AppliedDateTime: " << rec.getAppliedDateTime();
	if ( !rec.isPendingSynchronizationNull() )
		os << ", PendingSynchronization: " << rec.getPendingSynchronization();
	if ( !rec.isSynchronizedDateTimeNull() )
		os << ", SynchronizedDateTime: " << rec.getSynchronizedDateTime();
	if ( !rec.isPendingDeletionNull() )
		os << ", PendingDeletion: " << rec.getPendingDeletion();
	if ( !rec.isDeletedDateTimeNull() )
		os << ", DeletedDateTime: " << rec.getDeletedDateTime();

	return os << "]" << endl;
}


#ifndef NO_NAMESPACES
} // namespace edb
#endif
