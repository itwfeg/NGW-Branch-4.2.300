/**
 * @file WizardsTable.h
 * @brief Wizards table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOWIZARDSTABLE_H
#define DBOWIZARDSTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_WIZARDS "wizards"

// columns
#define COLUMN_DEVICE "device"
#define COLUMN_DEVICE_EXT "deviceExt"
#define COLUMN_ERROR_CODE "errorCode"
#define COLUMN_SERVICE_CAT "serviceCategory"
#define COLUMN_SEVERITY "severity"
#define COLUMN_WIZARD "wizard"

// value types

#define EVENT_SEVERITY_UNKNOWN 0
#define EVENT_SEVERITY_INFO 1
#define EVENT_SEVERITY_WARNING 2
#define EVENT_SEVERITY_ERROR 3
#define EVENT_SEVERITY_CRITICAL 4

//
// create table sql
//

#define CREATE_TABLE_WIZARDS "create table " \
    TABLE_WIZARDS " ( " \
        COLUMN_DEVICE               " integer not null, " \
        COLUMN_DEVICE_EXT           " integer not null, " \
        COLUMN_ERROR_CODE           " integer not null, " \
        COLUMN_SEVERITY             " integer, " \
		COLUMN_SERVICE_CAT			" integer, " \
        COLUMN_WIZARD               " integer, " \
        "primary key (" COLUMN_DEVICE ", " COLUMN_DEVICE_EXT ", " COLUMN_ERROR_CODE "));"

//
// populate table sql
//

#define POPULATE_TABLE_WIZARDS "insert into " TABLE_WIZARDS " " \
    "(" COLUMN_DEVICE "," COLUMN_DEVICE_EXT "," COLUMN_ERROR_CODE "," COLUMN_SEVERITY "," COLUMN_SERVICE_CAT "," COLUMN_WIZARD ") values " \
	\
    /* wrapper */ \
    "( 9, 0, 501, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapperCommLost */ \
    "( 9, 0, 502, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* PrimaryConveyorStillHome */ \
    "( 9, 0, 503, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* PrimaryMotorLowCurrent */ \
    "( 9, 0, 504, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* PrimaryMotorMoveError */ \
    "( 9, 0, 505, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SecondaryMotorLowCurrent */ \
    "( 9, 0, 506, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* CameraHeightCalError */ \
    "( 9, 0, 507, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* RearFolderStillHome */ \
    "( 9, 0, 508, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* RearFolderMotorLowCurrent */ \
    "( 9, 0, 509, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* RearFolderMotorMoveError */ \
	\
    "( 9, 0, 510, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* FrontFolderStillHome */ \
    "( 9, 0, 511, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* FrontFolderMotorLowCurrent */ \
    "( 9, 0, 512, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SealerBeltMotorLowCurrent */ \
    "( 9, 0, 513, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* PusherStillHome */ \
    "( 9, 0, 514, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* PusherMotorLowCurrent */ \
    "( 9, 0, 515, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*EPrimaryConveyorNotAtHome */ \
    "( 9, 0, 516, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 3 ), "		/* PrimaryConveyorBlocked */ \
    "( 9, 0, 517, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* PrimaryConveyorJammed */ \
    "( 9, 0, 518, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* PrimaryConveyorDidNotFindHome */ \
    "( 9, 0, 519, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 3 ), "		/* PackageBlockingIntake */ \
	\
	"( 9, 0, 520, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SecondaryElevatorSyncError */ \
	"( 9, 0, 521, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 3 ), "		/* PackageDimensionError */ \
	"( 9, 0, 522, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* PackageOnElevatorSensorNotTripped */ \
	"( 9, 0, 523, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* SecondaryConveyorJammed */ \
	"( 9, 0, 524, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*ESecondaryShiftNotAtHome */ \
	"( 9, 0, 525, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* CameraAlignmentNeeded */ \
	"( 9, 0, 526, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* SecondaryShiftLeftLimitError */ \
	"( 9, 0, 527, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* SecondaryShiftRightLimitError */ \
	"( 9, 0, 528, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SecondaryShiftDidNotFindHome */ \
	"( 9, 0, 529, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SecondaryMotorMoveError */ \
	\
	"( 9, 0, 530, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*EElevatorNotAtHome */ \
	"( 9, 0, 531, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* ElevatorJammed */ \
	"( 9, 0, 532, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* ElevatorMotorLowCurrent */ \
	"( 9, 0, 533, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* ElevatorDidNotFindHome */ \
	"( 9, 0, 534, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* ElevatorStillHome */ \
	"( 9, 0, 535, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* ElevatorSensorNotWorking */ \
	"( 9, 0, 536, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* ElevatorMotorMoveError */ \
	"( 9, 0, 537, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* FilmTooSmallForPackage */ \
	"( 9, 0, 538, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* FilmNotPulled */ \
	"( 9, 0, 539, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*EGripperNotAtHome */ \
	\
	"( 9, 0, 540, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* KnifeSensorNoTrip */ \
	"( 9, 0, 541, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* GripperJammed */ \
	"( 9, 0, 542, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* GripperDidNotFindHome */ \
	"( 9, 0, 543, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* GripperStillHome */ \
	"( 9, 0, 544, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* GripperMotorLowCurrent */ \
	"( 9, 0, 545, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* KnifeFireTimeExceeded */ \
	"( 9, 0, 546, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*ESideClampsNotAtHome */ \
	"( 9, 0, 547, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* SideClampsJammed */ \
	"( 9, 0, 548, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*ESideFoldersNotAtHome */ \
	"( 9, 0, 549, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* SideFoldersJammed */ \
	\
	"( 9, 0, 550, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*ERearFolderNotAtHome */ \
	"( 9, 0, 551, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* RearFolderJammed */ \
	"( 9, 0, 552, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*EFrontFolderNotAtHome */ \
	"( 9, 0, 553, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* FrontFolderJammed */ \
	"( 9, 0, 554, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/*EPusherNotAtHome */ \
	"( 9, 0, 555, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* PusherJammed */ \
	"( 9, 0, 556, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* SealerBeltJammed */ \
	"( 9, 0, 557, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* KnifeSensorDidntClear */ \
	"( 9, 0, 558, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), "		/* LabelApplierDidntHome */ \
	"( 9, 0, 559, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* TemperatureError */ \
	\
	"( 9, 0, 560, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* HeaterError */ \
	"( 9, 0, 561, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* GripperMotorMoveError */ \
	"( 9, 0, 562, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* SideClampsStillHome */ \
	"( 9, 0, 563, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SideClampsMotorLowCurrent */ \
	"( 9, 0, 564, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SideClampsMotorMoveError */ \
	"( 9, 0, 565, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* NoCanCommAllBoards */ \
	"( 9, 0, 566, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* FrontFolderMotorMoveError */ \
	"( 9, 0, 567, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* RightCanopyInterlock */ \
	"( 9, 0, 568, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* RightCoverInterlock */ \
	"( 9, 0, 569, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* LeftCoverInterlock */ \
	\
	"( 9, 0, 570, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* LeftCanopyInterlock */ \
	"( 9, 0, 571, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* KnifeAccessInterlock */ \
	"( 9, 0, 572, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* FilmDoorInterlock */ \
	"( 9, 0, 573, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* SideFolderStillHome */ \
	"( 9, 0, 574, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SideFolderMotorLowCurrent */ \
	"( 9, 0, 575, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SideClampsDidNotFindHome */ \
	"( 9, 0, 576, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SideFoldersDidNotFindHome */ \
	"( 9, 0, 577, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* RearFolderDidNotFindHome */ \
	"( 9, 0, 578, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* FrontFolderDidNotFindHome */ \
	"( 9, 0, 579, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* PusherDidNotFindHome */ \
	\
	"( 9, 0, 580, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* Reserved3 */ \
	"( 9, 0, 581, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* PackageStoppedOnSecondaryConveyor */ \
	"( 9, 0, 582, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SideFolderMotorMoveError */ \
	"( 9, 0, 583, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* GhostPackageDetected */ \
	"( 9, 0, 584, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* PackageTooTall */ \
	"( 9, 0, 585, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* Node1_MotorFuseBlown */ \
	"( 9, 0, 586, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* Node2_MotorFuseBlown */ \
	"( 9, 0, 587, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* Node3_MotorFuseBlown */ \
	"( 9, 0, 588, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* Node4_MotorFuseBlown */ \
	"( 9, 0, 589, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* Node5_MotorFuseBlown */ \
	\
	"( 9, 0, 590, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* TopRollFilmLowWarn */ \
	"( 9, 0, 591, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* BottomRollFilmLowWarn */ \
	"( 9, 0, 592, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxTimingsNullNode1 */ \
	"( 9, 0, 593, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxTimingsNullNode2 */ \
	"( 9, 0, 594, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxTimingsNullNode3 */ \
	"( 9, 0, 595, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxTimingsNullNode4 */ \
	"( 9, 0, 596, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxTimingsNullNode5 */ \
	"( 9, 0, 597, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorruptNode1 */ \
	"( 9, 0, 598, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorruptNode2 */ \
	"( 9, 0, 599, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorruptNode3 */ \
	\
	"( 9, 0, 600, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorruptNode4 */ \
	"( 9, 0, 601, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorruptNode5 */ \
	"( 9, 0, 602, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingHBNode1 */ \
	"( 9, 0, 603, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingHBNode2 */ \
	"( 9, 0, 604, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingHBNode3 */ \
	"( 9, 0, 605, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingHBNode4 */ \
	"( 9, 0, 606, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingHBNode5 */ \
	"( 9, 0, 607, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsTooManyNode1 */ \
	"( 9, 0, 608, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsTooManyNode2 */ \
	"( 9, 0, 609, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsTooManyNode3 */ \
	\
	"( 9, 0, 610, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsTooManyNode4 */ \
	"( 9, 0, 611, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsTooManyNode5 */ \
	"( 9, 0, 612, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingWBNode1 */ \
	"( 9, 0, 613, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingWBNode2 */ \
	"( 9, 0, 614, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingWBNode3 */ \
	"( 9, 0, 615, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingWBNode4 */ \
	"( 9, 0, 616, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsMissingWBNode5 */ \
	"( 9, 0, 617, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorrupt_CONFIG_Node1 */ \
	"( 9, 0, 618, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorrupt_CONFIG_Node2 */ \
	"( 9, 0, 619, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorrupt_CONFIG_Node3 */ \
	\
	"( 9, 0, 620, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorrupt_CONFIG_Node4 */ \
	"( 9, 0, 621, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsCorrupt_CONFIG_Node5 */ \
	"( 9, 0, 622, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsConfigTooManyNode1 */ \
	"( 9, 0, 623, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsConfigTooManyNode2 */ \
	"( 9, 0, 624, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsConfigTooManyNode3 */ \
	"( 9, 0, 625, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsConfigTooManyNode4 */ \
	"( 9, 0, 626, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 1 ), "			/* WrapBoxTimingsConfigTooManyNode5 */ \
	"( 9, 0, 627, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxReadBufferCorruption */ \
	"( 9, 0, 628, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxWriteBufferCorruption */ \
	"( 9, 0, 629, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxTimingsReadBufferTooMany */ \
	\
	"( 9, 0, 630, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxTimingsWriteBufferTooMany */ \
	"( 9, 0, 631, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 3 ), "		/* PkgSensorAcrossElevatorBlocked */ \
	"( 9, 0, 632, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* DataFlashUpdateTimeout */ \
	"( 9, 0, 633, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* DataFlashUpdateCallocError */ \
	"( 9, 0, 634, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* Node1CanCommFailure */ \
	"( 9, 0, 635, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* Node3CanCommFailure */ \
	"( 9, 0, 636, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* Node4CanCommFailure */ \
	"( 9, 0, 637, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* Node5CanCommFailure */ \
	"( 9, 0, 638, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* PusherMotorMoveError */ \
	"( 9, 0, 639, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* WrapBoxTimingsCorrupt */ \
	\
	"( 9, 0, 640, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* PrimaryIntakeStalled */ \
	"( 9, 0, 641, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* SecondaryIntakeStalled */ \
	"( 9, 0, 642, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* ElevatorStalled */ \
	"( 9, 0, 643, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* RearFolderStalled */ \
	"( 9, 0, 644, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* FrontFolderStalled */ \
	"( 9, 0, 645, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* SideFolderStalled */ \
	"( 9, 0, 646, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* SideClampStalled */ \
	"( 9, 0, 647, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* SealerBeltStalled */ \
	"( 9, 0, 648, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 3 ), "		/* SecondaryShiftJammed */ \
	"( 9, 0, 649, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* GripperStalled */ \
	\
	"( 9, 0, 650, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), "			/* PusherStalled */ \
	"( 9, 0, 651, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), "		/* SealerBeltMotorMoveError */ \
	"( 9, 0, 652, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/* PrimaryLabelTakenError */ \
	"( 9, 0, 653, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), "		/* SecondaryLabelTakenError */ \
	"( 9, 0, 654, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), "			/* IncompatiblePkgTimingData */ \
    "( 9, 0, 655, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ENarrowGripperFailedToClose */ \
    "( 9, 0, 656, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ENarrowGripperFailedToOpen */ \
    "( 9, 0, 657, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ELeftGripperFailedToClose */ \
    "( 9, 0, 658, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ELeftGripperFailedToOpen */ \
    "( 9, 0, 659, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ERightGripperFailedToClose */ \
	\
    "( 9, 0, 660, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ERightGripperFailedToOpen */ \
	"( 9, 0, 661, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ELeftSideClampFailedToClose */ \
	"( 9, 0, 662, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ELeftSideClampFailedToOpen */ \
	"( 9, 0, 663, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ERightSideClampFailedToClose */ \
	"( 9, 0, 664, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/*ERightSideClampFailedToOpen */ \
	"( 9, 0, 665, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* UpperFilmSelectorFailedToSwitch */ \
	"( 9, 0, 666, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* LowerFilmSelectorFailedToSwitch */ \
	"( 9, 0, 667, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 0 ), " 		/* FilmSelectorUnlatched */ \
	"( 9, 0, 668, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* ApplierWandUpFailure */ \
	"( 9, 0, 669, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* ApplierWandDownFailure */ \
	\
	"( 9, 0, 670, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* ApplierHandHomeFailure */ \
	"( 9, 0, 671, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* ApplierWandVacuumFailure */ \
	"( 9, 0, 672, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), " 	/* AirCompressorFailure */ \
	"( 9, 0, 673, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), " 	/* RegulatedAirPressureFailure */ \
	"( 9, 0, 674, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), " 		/* ActuatorAirPressureFailure */ \
	"( 9, 0, 675, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), " 		/* BadUpperFilmPull */ \
	"( 9, 0, 676, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), " 		/* BadLowerFilmPull */ \
	"( 9, 0, 677, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), " 	/* PowerSupplyFailure24V */ \
	"( 9, 0, 678, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), " 	/* InrushFailure170V */ \
	"( 9, 0, 679, " STRINGIFY(EVENT_SEVERITY_CRITICAL) ", 0, 0 ), " 	/* PowerSupplyFailure170V */ \
	\
	"( 9, 0, 680, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), " 		/* UpperFilmBrakeFailure */ \
	"( 9, 0, 681, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 0 ), " 		/* LowerFilmBrakeFailure */ \
	"( 9, 0, 682, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 3 ), " 		/* UpperFilmRollLooseArbor */ \
	"( 9, 0, 683, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 3 ), " 		/* LowerFilmRollLooseArbor */ \
	"( 9, 0, 684, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 3 ), " 		/* PrinterDockFailure */ \
	"( 9, 0, 685, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 3 ), " 		/* BlockedPackageSensor */ \
    "( 9, 0, 686, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 3 ), " 		/* PressStartButton */ \
    "( 9, 0, 687, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 3 ), " 		/* IntakeSensorBlocked */ \
    "( 9, 0, 688, " STRINGIFY(EVENT_SEVERITY_WARNING) ", 0, 3 ), " 		/* RegulatedPressureTooHigh */ \
	\
	"( 9, 0, 689, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* SecApplierWandUpFailure */ \
	"( 9, 0, 690, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* SecApplierWandDownFailure */ \
	\
	"( 9, 0, 691, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* SecApplierHandHomeFailure */ \
	"( 9, 0, 692, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* SecApplierWandVacuumFailure */ \
	\
	"( 9, 0, 693, " STRINGIFY(EVENT_SEVERITY_ERROR) ", 0, 0 ), " 		/* ApplerSlideAdjustmentTimeout */ \
    /* wrapper system */ \
    "( 10, 0, 17, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ), " \
    "( 10, 0, 18, " STRINGIFY(EVENT_SEVERITY_INFO) ", 0, 3 ); "

//
// insert sql
//

#define INSERT_WIZARDS "insert into " \
	TABLE_WIZARDS " (" \
		COLUMN_DEVICE ", " \
		COLUMN_DEVICE_EXT ", " \
		COLUMN_ERROR_CODE ", " \
		COLUMN_SEVERITY ", " \
		COLUMN_SERVICE_CAT ", " \
		COLUMN_WIZARD ") " \
		"values (?1, ?2, ?3, ?4, ?5, ?6);"

//
// update sql
//

#define UPDATE_WIZARDS "update " \
	TABLE_WIZARDS " set " \
		COLUMN_SEVERITY "=?4, " \
		COLUMN_SERVICE_CAT "=?5, " \
		COLUMN_WIZARD "=?6 " \
		"where (" COLUMN_DEVICE "=?1 and " COLUMN_DEVICE_EXT "=?2 and " COLUMN_ERROR_CODE "=?3);"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOWIZARDSTABLE_H
