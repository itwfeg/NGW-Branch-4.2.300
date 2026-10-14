#ifndef EVOLWRAPPERGENERALSENSORS_H
#define EVOLWRAPPERGENERALSENSORS_H

#include "Wrapper.h"
#include "HobString.h"

class EvolWrapperGeneralSensors
{
public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	EvolWrapperGeneralSensors();
	EvolWrapperGeneralSensors( const char *buffer );
	EvolWrapperGeneralSensors( const EvolWrapperGeneralSensors &sensors );

	~EvolWrapperGeneralSensors();

	// ========================================================================
	// Operators
	// ========================================================================

	void operator = ( const EvolWrapperGeneralSensors& rhs );

	bool operator == ( const EvolWrapperGeneralSensors& rhs ) const {return equals(rhs);}

	// ========================================================================
	// Methods
	// ========================================================================

	bool equals( const EvolWrapperGeneralSensors &other ) const;

	const char *serialize() const;

	void show();

	// ========================================================================
	// Properties
	// ========================================================================

	UINT32 getSerializedLength() const;

	bool isPrimaryHome() const {return primaryHome;}
	void setPrimaryHome( bool value ) {primaryHome = value;}

	bool isShiftHome() const {return shiftHome;}
	void setShiftHome( bool value ) {shiftHome = value;}

	bool isShiftLeftLimit() const {return shiftLeftLimit;}
	void setShiftLeftLimit( bool value ) {shiftLeftLimit = value;}

	bool isGripperHome() const {return gripperHome;}
	void setGripperHome( bool value ) {gripperHome = value;}

	bool isSideFolderHome() const {return sideFolderHome;}
	void setSideFolderHome( bool value ) {sideFolderHome = value;}

	bool isPusherHome() const {return pusherHome;}
	void setPusherHome( bool value ) {pusherHome = value;}

	bool isFrontFolderHome() const {return frontFolderHome;}
	void setFrontFolderHome( bool value ) {frontFolderHome = value;}

	bool isRearFolderHome() const {return rearFolderHome;}
	void setRearFolderHome( bool value ) {rearFolderHome = value;}

	bool isElevatorHome() const {return elevatorHome;}
	void setElevatorHome( bool value ) {elevatorHome = value;}

	bool isLabelRotationHome() const {return labelRotationHome;}
	void setLabelRotationHome( bool value ) {labelRotationHome = value;}

	bool isElevatorPkg() const {return elevatorPkg;}
	void setElevatorPkg( bool value ) {elevatorPkg = value;}

	bool isTransferPkg() const {return transferPkg;}
	void setTransferPkg( bool value ) {transferPkg = value;}

	bool isStart() const {return start;}
	void setStart( bool value ) {start = value;}

	bool isStop() const {return stop;}
	void setStop( bool value ) {stop = value;}

	bool getSideClampRight() const {return sideClampRight;}
	void setSideClampRight( bool value ) {sideClampRight = value;}

	bool isSideClampHome() const {return sideClampHome;}
	void setSideClampHome( bool value ) {sideClampHome = value;}

	bool getFilmRoll() const {return filmRoll;}
	void setFilmRoll( bool value ) {filmRoll = value;}

	bool getKnifeUp() const {return knifeUp;}
	void setKnifeUp( bool value ) {knifeUp = value;}

	/* SOF-3838  no longer used repurpose for second label applier 
	bool getSwitched24V() const {return switched24V;}
	void setSwitched24V( bool value ) {switched24V = value;}

	bool getHighVoltageInrush() const {return highVoltageInrush;}
	void setHighVoltageInrush( bool value ) {highVoltageInrush = value;}
	*/
	bool getSecWandUp() const {return secWandUp;}
	void setSecWandUp( bool value ) {secWandUp = value;}

	bool getSecWandDown() const {return secWandDown;}
	void setSecWandDown( bool value ) {secWandDown = value;}

	bool getFilmSelectorLever() const {return filmSelectorLever;}
	void setFilmSelectorLever( bool value ) {filmSelectorLever = value;}

	/* SOF-3838  no longer used repurpose for second label applier 
	bool getPrinterPlate() const {return printerPlate;}
	void setPrinterPlate( bool value ) {printerPlate = value;}
	*/
	
	bool getSecApplierHand() const {return secApplierHand;}
	void setSecApplierHand( bool value ) {secApplierHand = value;}

	bool getFilmSelectorUp() const {return filmSelectorUp;}
	void setFilmSelectorUp( bool value ) {filmSelectorUp = value;}

	bool getFilmSelectorDown() const {return filmSelectorDown;}
	void setFilmSelectorDown( bool value ) {filmSelectorDown = value;}

	bool getWandUp() const {return wandUp;}
	void setWandUp( bool value ) {wandUp = value;}

	bool getWandDown() const {return wandDown;}
	void setWandDown( bool value ) {wandDown = value;}

	bool getKnifeDown() const {return knifeDown;}
	void setKnifeDown( bool value ) {knifeDown = value;}

	bool getApplierHand() const {return applierHand;}
	void setApplierHand( bool value ) {applierHand = value;}

	bool getGripperNarrow() const {return gripperNarrow;}
	void setGripperNarrow( bool value ) {gripperNarrow = value;}

	bool getGripperRight() const {return gripperRight;}
	void setGripperRight( bool value ) {gripperRight = value;}

	bool getGripperLeft() const {return gripperLeft;}
	void setGripperLeft( bool value ) {gripperLeft = value;}

	bool getSideClampLeft() const {return sideClampLeft;}
	void setSideClampLeft( bool value ) {sideClampLeft = value;}

	void setStatus( UINT32 statusBits );

private:

	bool primaryHome;
	bool shiftHome;
	bool shiftLeftLimit;
	bool gripperHome;
	bool sideFolderHome;
	bool pusherHome;
	bool frontFolderHome;
	bool rearFolderHome;
	bool elevatorHome;
	bool labelRotationHome;
	bool elevatorPkg;
	bool transferPkg;
	bool start;
	bool stop;
	bool sideClampRight;
	bool sideClampHome;
	bool filmRoll;
	bool knifeUp;
	/* SOF-3838  no longer used repurpose for second label applier 
	bool switched24V;
	bool highVoltageInrush;
	*/
	bool secWandUp; 
	bool secWandDown;

	bool filmSelectorLever;
	/* SOF-3838  no longer used repurpose for second label applier 
	bool printerPlate;
	*/
	bool secApplierHand;

	bool filmSelectorUp;
	bool filmSelectorDown;
	bool wandUp;
	bool wandDown;
	bool knifeDown;
	bool applierHand;
	bool gripperNarrow;
	bool gripperRight;
	bool gripperLeft;
	bool sideClampLeft;

	mutable std::string serializedData;

	void initialize();

	void serializeField( const void *f, size_t fs ) const;
	void *deserializeField( void *f, size_t fs, const void *buffer );

#ifdef USE_ACCOUNTANT
	static int _dataType_;
	static std::string _dataTypeName_;
#endif

};

class EvolWrapperGeneralSensors_2
{
public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	EvolWrapperGeneralSensors_2();
	EvolWrapperGeneralSensors_2(const char *buffer);
	EvolWrapperGeneralSensors_2(const EvolWrapperGeneralSensors_2 &sensors);

	~EvolWrapperGeneralSensors_2();

	// ========================================================================
	// Operators
	// ========================================================================

	void operator = (const EvolWrapperGeneralSensors_2& rhs);
	bool operator == (const EvolWrapperGeneralSensors_2& rhs) const { return equals(rhs); }

	// ========================================================================
	// Methods
	// ========================================================================

	bool equals(const EvolWrapperGeneralSensors_2 &other) const;
	const char *serialize() const;
	void show();

	// ========================================================================
	// Properties
	// ========================================================================

	UINT32 getSerializedLength() const;
	void setStatus(UINT32 statusBits);

	bool isSecLabelRotationHome() const { return secLabelRotationHome; }
	void setSecLabelRotationHome(bool value) { secLabelRotationHome = value; }

	bool isSecLabelApplierSlideHome() const { return secLabelApplierSlideHome; }
	void setSecLabelApplierSlideHome(bool value) { secLabelApplierSlideHome = value; }


private:

	bool secLabelRotationHome;
	bool secLabelApplierSlideHome;

	mutable std::string serializedData;

	void initialize();

	void serializeField(const void * f, size_t fs) const;
	void *deserializeField(void * f, size_t fs, const void * buffer);
};

#endif // EVOLWRAPPERGENERALSENSORS_H
