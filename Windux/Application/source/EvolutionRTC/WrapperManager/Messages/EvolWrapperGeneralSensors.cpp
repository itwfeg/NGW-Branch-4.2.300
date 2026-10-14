#include "EvolWrapperGeneralSensors.h"
#include "EvolWrapperMessages.h"

#include "Debug.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperGeneralSensors::_dataType_ = 0;
std::string EvolWrapperGeneralSensors::_dataTypeName_("EvolWrapperGeneralSensors"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
EvolWrapperGeneralSensors::EvolWrapperGeneralSensors()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
EvolWrapperGeneralSensors::EvolWrapperGeneralSensors( const char *buffer )
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	void *b( const_cast<char*>( buffer ) );

	b = deserializeField( &primaryHome, sizeof(bool), b );
	b = deserializeField( &shiftHome, sizeof(bool), b );
	b = deserializeField( &shiftLeftLimit, sizeof(bool), b );
	b = deserializeField( &gripperHome, sizeof(bool), b );
	b = deserializeField( &sideFolderHome, sizeof(bool), b );
	b = deserializeField( &pusherHome, sizeof(bool), b );
	b = deserializeField( &frontFolderHome, sizeof(bool), b );
	b = deserializeField( &rearFolderHome, sizeof(bool), b );
	b = deserializeField( &elevatorHome, sizeof(bool), b );
	b = deserializeField( &labelRotationHome, sizeof(bool), b );
	b = deserializeField( &elevatorPkg, sizeof(bool), b );
	b = deserializeField( &transferPkg, sizeof(bool), b );
	b = deserializeField( &start, sizeof(bool), b );
	b = deserializeField( &stop, sizeof(bool), b );
	b = deserializeField( &sideClampRight, sizeof(bool), b );
	b = deserializeField( &sideClampHome, sizeof(bool), b );
	b = deserializeField( &filmRoll, sizeof(bool), b );
	b = deserializeField( &knifeUp, sizeof(bool), b );
	/* SOF-3838  no longer used repurpose for second label applier
	b = deserializeField( &switched24V, sizeof(bool), b );
	b = deserializeField( &highVoltageInrush, sizeof(bool), b );
	*/
	b = deserializeField( &secWandUp, sizeof(bool), b );
	b = deserializeField( &secWandDown, sizeof(bool), b );

	b = deserializeField( &filmSelectorLever, sizeof(bool), b );
	/* SOF-3838  no longer used repurpose for second label applier
	b = deserializeField( &printerPlate, sizeof(bool), b );
	*/
	b = deserializeField( &secApplierHand, sizeof(bool), b );

	b = deserializeField( &filmSelectorUp, sizeof(bool), b );
	b = deserializeField( &filmSelectorDown, sizeof(bool), b );
	b = deserializeField( &wandUp, sizeof(bool), b );
	b = deserializeField( &wandDown, sizeof(bool), b );
	b = deserializeField( &knifeDown, sizeof(bool), b );
	b = deserializeField( &applierHand, sizeof(bool), b );
	b = deserializeField( &gripperNarrow, sizeof(bool), b );
	b = deserializeField( &gripperRight, sizeof(bool), b );
	b = deserializeField( &gripperLeft, sizeof(bool), b );
	b = deserializeField( &sideClampLeft, sizeof(bool), b );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
EvolWrapperGeneralSensors::EvolWrapperGeneralSensors( const EvolWrapperGeneralSensors &other )
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	(*this) = other;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
EvolWrapperGeneralSensors::~EvolWrapperGeneralSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *EvolWrapperGeneralSensors::serialize() const
{
	serializedData = "";

	serializeField( &primaryHome, sizeof(bool) );
	serializeField( &shiftHome, sizeof(bool) );
	serializeField( &shiftLeftLimit, sizeof(bool) );
	serializeField( &gripperHome, sizeof(bool) );
	serializeField( &sideFolderHome, sizeof(bool) );
	serializeField( &pusherHome, sizeof(bool) );
	serializeField( &frontFolderHome, sizeof(bool) );
	serializeField( &rearFolderHome, sizeof(bool) );
	serializeField( &elevatorHome, sizeof(bool) );
	serializeField( &labelRotationHome, sizeof(bool) );
	serializeField( &elevatorPkg, sizeof(bool) );
	serializeField( &transferPkg, sizeof(bool) );
	serializeField( &start, sizeof(bool) );
	serializeField( &stop, sizeof(bool) );
	serializeField( &sideClampRight, sizeof(bool) );
	serializeField( &sideClampHome, sizeof(bool) );
	serializeField( &filmRoll, sizeof(bool) );
	serializeField( &knifeUp, sizeof(bool) );
	/* SOF-3838  no longer used repurpose for second label applier
	serializeField( &switched24V, sizeof(bool) );
	serializeField( &highVoltageInrush, sizeof(bool) );
	*/
	serializeField( &secWandUp, sizeof(bool) );
	serializeField( &secWandDown, sizeof(bool) );

	serializeField( &filmSelectorLever, sizeof(bool) );

	/* SOF-3838  no longer used repurpose for second label applier
	serializeField( &secApplierHand, sizeof(bool) );
	*/
	serializeField( &secApplierHand, sizeof(bool) );

	serializeField( &filmSelectorUp, sizeof(bool) );
	serializeField( &filmSelectorDown, sizeof(bool) );
	serializeField( &wandUp, sizeof(bool) );
	serializeField( &wandDown, sizeof(bool) );
	serializeField( &knifeDown, sizeof(bool) );
	serializeField( &applierHand, sizeof(bool) );
	serializeField( &gripperNarrow, sizeof(bool) );
	serializeField( &gripperRight, sizeof(bool) );
	serializeField( &gripperLeft, sizeof(bool) );
	serializeField( &sideClampLeft, sizeof(bool) );

	return serializedData.data();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 EvolWrapperGeneralSensors::getSerializedLength()const
{
	return (UINT32)serializedData.length();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EvolWrapperGeneralSensors::show()
{
	HobString msgDebug;

	if ( !Debug::isTracing( "EvolWrapperGeneralSensors" ) )
		return;

	msgDebug = "primaryHome:           ";
	msgDebug.append(Bool::toString(primaryHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "shiftHome:    ";
	msgDebug.append(Bool::toString(shiftHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "shiftLeftLimit: ";
	msgDebug.append(Bool::toString(shiftLeftLimit));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "gripperHome: ";
	msgDebug.append(Bool::toString(gripperHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "sideFolderHome:  ";
	msgDebug.append(Bool::toString(sideFolderHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "pusherHome: ";
	msgDebug.append(Bool::toString(pusherHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "frontFolderHome: ";
	msgDebug.append(Bool::toString(frontFolderHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "rearFolderHome: ";
	msgDebug.append(Bool::toString(rearFolderHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "elevatorHome: ";
	msgDebug.append(Bool::toString(elevatorHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "labelRotationHome: ";
	msgDebug.append(Bool::toString(labelRotationHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "elevatorPkg: ";
	msgDebug.append(Bool::toString(elevatorPkg));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "transferPkg: ";
	msgDebug.append(Bool::toString(transferPkg));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "start: ";
	msgDebug.append(Bool::toString(start));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "stop: ";
	msgDebug.append(Bool::toString(stop));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "sideClampRight: ";
	msgDebug.append(Bool::toString(sideClampRight));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "sideClampHome: ";
	msgDebug.append(Bool::toString(sideClampHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "filmRoll: ";
	msgDebug.append(Bool::toString(filmRoll));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "knifeUp: ";
	msgDebug.append(Bool::toString(knifeUp));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);
	/* SOF-3838  no longer used repurpose for second label applier
	msgDebug = "switched24V: ";
	msgDebug.append(Bool::toString(switched24V));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "highVoltageInrush: ";
	msgDebug.append(Bool::toString(highVoltageInrush));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);
	*/
	
	msgDebug = "secWandUp: ";
	msgDebug.append(Bool::toString(secWandUp));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "secWandDown: ";
	msgDebug.append(Bool::toString(secWandDown));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "filmSelectorLever: ";
	msgDebug.append(Bool::toString(filmSelectorLever));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	/* SOF-3838  no longer used repurpose for second label applier
	msgDebug = "printerPlate: ";
	msgDebug.append(Bool::toString(printerPlate));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);
	*/

	msgDebug = "secApplierHand: ";
	msgDebug.append(Bool::toString(secApplierHand));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "filmSelectorUp: ";
	msgDebug.append(Bool::toString(filmSelectorUp));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "filmSelectorDown: ";
	msgDebug.append(Bool::toString(filmSelectorDown));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "wandUp: ";
	msgDebug.append(Bool::toString(wandUp));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "wandDown: ";
	msgDebug.append(Bool::toString(wandDown));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "knifeDown: ";
	msgDebug.append(Bool::toString(knifeDown));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "applierHand: ";
	msgDebug.append(Bool::toString(applierHand));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "gripperNarrow: ";
	msgDebug.append(Bool::toString(gripperNarrow));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "gripperRight: ";
	msgDebug.append(Bool::toString(gripperRight));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "gripperLeft: ";
	msgDebug.append(Bool::toString(gripperLeft));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);

	msgDebug = "sideClampLeft: ";
	msgDebug.append(Bool::toString(sideClampLeft));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EvolWrapperGeneralSensors::setStatus( UINT32 statusBits )
{
	const EvolWrSwitchSensorBits *flags( (const EvolWrSwitchSensorBits*) &statusBits );

	primaryHome = flags->primaryHomeSwitch;
	shiftHome = flags->shiftHomeSwitch;
	shiftLeftLimit = flags->shiftLeftLimitSwitch;
	gripperHome = flags->gripperHomeSwitch;
	sideFolderHome = flags->sideFolderHomeSwitch;
	pusherHome = flags->pusherHomeSwitch;
	frontFolderHome = flags->frontFolderHomeSwitch;
	rearFolderHome = flags->rearFolderHomeSwitch;
	elevatorHome = flags->elevatorHomeSwitch;
	labelRotationHome = flags->labelRotationHomeSwitch;
	elevatorPkg = flags->elevatorPkgSensor;
	transferPkg = flags->transferPkgSensor;
	start = flags->startSwitch;
	stop = flags->stopSwitch;
	sideClampRight = flags->sideClampRightSensor;
	sideClampHome = flags->sideClampHomeSwitch;
	filmRoll = flags->filmRollSensor;
	knifeUp = flags->knifeUpSensor;
	/* SOF-3838  no longer used repurpose for second label applier 
	switched24V = flags->switched24V;
	highVoltageInrush = flags->highVoltageInrush; */
	secWandUp = flags->secWandUpSensor;
	secWandDown = flags->secWandDownSensor;
	
	filmSelectorLever = flags->filmSelectorLever;
	
	/* SOF-3838  no longer used repurpose for second label applier 
	printerPlate = flags->printerPlateSwitch; */
	secApplierHand = flags->secApplierHandSensor;

	filmSelectorUp = flags->filmSelectorUpSensor;
	filmSelectorDown = flags->filmSelectorDownSensor;
	wandUp = flags->wandUpSensor;
	wandDown = flags->wandDownSensor;
	knifeDown = flags->knifeDownSensor;
	applierHand = flags->applierHandSensor;
	gripperNarrow = flags->gripperNarrowSensor;
	gripperRight = flags->gripperRightSensor;
	gripperLeft = flags->gripperLeftSensor;
	sideClampLeft = flags->sideClampLeftSensor;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool EvolWrapperGeneralSensors::equals( const EvolWrapperGeneralSensors &other ) const
{
	bool result(
		( primaryHome == other.primaryHome ) &&
		( shiftHome == other.shiftHome ) &&
		( shiftLeftLimit == other.shiftLeftLimit ) &&
		( gripperHome == other.gripperHome ) &&
		( sideFolderHome == other.sideFolderHome ) &&
		( pusherHome == other.pusherHome ) &&
		( frontFolderHome == other.frontFolderHome ) &&
		( rearFolderHome == other.rearFolderHome ) &&
		( elevatorHome == other.elevatorHome ) &&
		( labelRotationHome == other.labelRotationHome ) &&
		( elevatorPkg == other.elevatorPkg ) &&
		( transferPkg == other.transferPkg ) &&
		( start == other.start ) &&
		( stop == other.stop ) &&
		( sideClampRight == other.sideClampRight ) &&
		( sideClampHome == other.sideClampHome ) &&
		( filmRoll == other.filmRoll ) &&
		( knifeUp == other.knifeUp ) &&
		/* SOF-3838  no longer used repurpose for second label applier
		( switched24V == other.switched24V ) &&
		( highVoltageInrush == other.highVoltageInrush ) &&
		*/
		( secWandUp == other.secWandUp ) &&
		( secWandDown == other.secWandDown ) &&

		( filmSelectorLever == other.filmSelectorLever ) &&
		/* SOF-3838  no longer used repurpose for second label applier
		( printerPlate == other.printerPlate ) &&
		*/
		( secApplierHand == other.secApplierHand ) &&

		( filmSelectorUp == other.filmSelectorUp ) &&
		( filmSelectorDown == other.filmSelectorDown ) &&
		( wandUp == other.wandUp ) &&
		( wandDown == other.wandDown ) &&
		( knifeDown == other.knifeDown ) &&
		( applierHand == other.applierHand ) &&
		( gripperNarrow == other.gripperNarrow ) &&
		( gripperRight == other.gripperRight ) &&
		( gripperLeft == other.gripperLeft ) &&
		( sideClampLeft == other.sideClampLeft ));

	Debug::trace( "EvolWrapperGeneralSensors", HobString( "equals = " ) + Bool::toString( result ) );
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EvolWrapperGeneralSensors::operator = ( const EvolWrapperGeneralSensors &rhs )
{
	primaryHome = rhs.primaryHome;
	shiftHome = rhs.shiftHome;
	shiftLeftLimit = rhs.shiftLeftLimit;
	gripperHome = rhs.gripperHome;
	sideFolderHome = rhs.sideFolderHome;
	pusherHome = rhs.pusherHome;
	frontFolderHome = rhs.frontFolderHome;
	rearFolderHome = rhs.rearFolderHome;
	elevatorHome = rhs.elevatorHome;
	labelRotationHome = rhs.labelRotationHome;
	elevatorPkg = rhs.elevatorPkg;
	transferPkg = rhs.transferPkg;
	start = rhs.start;
	stop = rhs.stop;
	sideClampRight = rhs.sideClampRight;
	sideClampHome = rhs.sideClampHome;
	filmRoll = rhs.filmRoll;
	knifeUp = rhs.knifeUp;
	/* SOF-3838  no longer used repurpose for second label applier
	switched24V = rhs.switched24V;
	highVoltageInrush = rhs.highVoltageInrush;
	*/
	secWandUp = rhs.secWandUp;
	secWandDown = rhs.secWandDown;
	
	filmSelectorLever = rhs.filmSelectorLever;
	/* SOF-3838  no longer used repurpose for second label applier
	printerPlate = rhs.printerPlate;
	*/
	secApplierHand  = rhs.secApplierHand;

	filmSelectorUp = rhs.filmSelectorUp;
	filmSelectorDown = rhs.filmSelectorDown;
	wandUp = rhs.wandUp;
	wandDown = rhs.wandDown;
	knifeDown = rhs.knifeDown;
	applierHand = rhs.applierHand;
	gripperNarrow = rhs.gripperNarrow;
	gripperRight = rhs.gripperRight;
	gripperLeft = rhs.gripperLeft;
	sideClampLeft = rhs.sideClampLeft;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EvolWrapperGeneralSensors::initialize()
{
	primaryHome = shiftHome = shiftLeftLimit = gripperHome =
    sideFolderHome = pusherHome = frontFolderHome = rearFolderHome =
	elevatorHome = labelRotationHome = elevatorPkg = transferPkg =
	start = stop = sideClampRight = sideClampHome =
	filmRoll = knifeUp = 
	/* SOF-3838  no longer used repurpose for second label applier
	switched24V = highVoltageInrush =
	*/
	secWandUp = secWandDown = filmSelectorLever = 
	/* SOF-3838  no longer used repurpose for second label applier 
	printerPlate = 
	*/
	secApplierHand =filmSelectorUp = filmSelectorDown =
	wandUp = wandDown = knifeDown = applierHand =
	gripperNarrow = gripperRight = gripperLeft = sideClampLeft = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EvolWrapperGeneralSensors::serializeField( const void *f, size_t fs ) const
{
	const char *p( (const char*) f );

	while ( fs-- )
		serializedData.append( 1, *p++ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void *EvolWrapperGeneralSensors::deserializeField( void *f, size_t fs, const void *buffer )
{
	char *p( (char*) f );
	char *b( (char*) const_cast<void*>( buffer ) );

	while ( fs-- )
		*p++ = *b++;

	return b;
}

/****************************************
*										*
*	  EvolWrapperGeneralSensors_2		*
*										*
*****************************************/

EvolWrapperGeneralSensors_2::EvolWrapperGeneralSensors_2()
{
	initialize();
}

EvolWrapperGeneralSensors_2::EvolWrapperGeneralSensors_2(const char *buffer)
{
	void *b(const_cast<char*>(buffer));

	b = deserializeField(&secLabelRotationHome, sizeof(bool), b);
	b = deserializeField(&secLabelApplierSlideHome, sizeof(bool), b);
}

EvolWrapperGeneralSensors_2::EvolWrapperGeneralSensors_2(const EvolWrapperGeneralSensors_2 &sensors)
{
	(*this) = sensors;
}

EvolWrapperGeneralSensors_2::~EvolWrapperGeneralSensors_2()
{
}

void EvolWrapperGeneralSensors_2::operator=(const EvolWrapperGeneralSensors_2 &rhs)
{
	secLabelRotationHome = rhs.secLabelRotationHome;
	secLabelApplierSlideHome = rhs.secLabelApplierSlideHome;
}

bool EvolWrapperGeneralSensors_2::equals(const EvolWrapperGeneralSensors_2 &other) const
{
	bool result(
		( secLabelRotationHome    == other.secLabelRotationHome ) &&
		(secLabelApplierSlideHome == other.secLabelApplierSlideHome )
	);

	Debug::trace("EvolWrapperGeneralSensors_2", HobString("equals = ") + Bool::toString(result));
	return result;
}

const char *EvolWrapperGeneralSensors_2::serialize() const
{
	serializedData = "";

	serializeField(&secLabelRotationHome, sizeof(bool));
	serializeField(&secLabelApplierSlideHome, sizeof(bool));

	return serializedData.data();
}

void EvolWrapperGeneralSensors_2::show()
{
	HobString msgDebug;

	if (!Debug::isTracing("EvolWrapperGeneralSensors"))
		return;

	msgDebug = "secLabelRotationHome: ";
	msgDebug.append(Bool::toString(secLabelRotationHome));
	msgDebug = "secLabelApplierSlideHome: ";
	msgDebug.append(Bool::toString(secLabelApplierSlideHome));
	Debug::trace("EvolWrapperGeneralSensors", msgDebug);
}

UINT32 EvolWrapperGeneralSensors_2::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void EvolWrapperGeneralSensors_2::setStatus(UINT32 statusBits)
{
	const EvolWrSwitchSensor_2Bits *flags((const EvolWrSwitchSensor_2Bits*)&statusBits);

	secLabelRotationHome = flags->secLabelRotationHome;
	secLabelApplierSlideHome = flags->secLabelApplierSlideHome;
}

void EvolWrapperGeneralSensors_2::initialize()
{
	secLabelRotationHome = false;
	secLabelApplierSlideHome = false;
}

void EvolWrapperGeneralSensors_2::serializeField(const void *f, size_t fs) const
{
	const char *p((const char*)f);

	while (fs--)
		serializedData.append(1, *p++);
}

void *EvolWrapperGeneralSensors_2::deserializeField(void *f, size_t fs, const void *buffer)
{
	char *p((char*)f);
	char *b((char*) const_cast<void*>(buffer));

	while (fs--)
		*p++ = *b++;

	return b;
}
