#ifndef SCALESYNCHRONIZATIONTYPE_H
#define SCALESYNCHRONIZATIONTYPE_H



typedef enum ScaleSynchronizationType
{
	ScaleSynchronizationTypeInvalid = -1,

	ScaleSynchronizationTypeStandAlone,
	ScaleSynchronizationTypePrimary,
	ScaleSynchronizationTypeSecondary,

	ScaleSynchronizationTypeLast
}   ScaleSynchronizationType_t;


typedef enum ScaleOnlineType
{
	ScaleOnlineTypeInvalid = -1,

	ScaleOnlineTypeUnknown,
	ScaleOnlineTypeOnline,
	ScaleOnlineTypeOffline,

	ScaleOnlineTypeLast
}   ScaleOnlineType_t;



#endif /* SCALESYNCHRONIZATIONTYPE_H */
