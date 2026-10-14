/**
 * @file ServicePartTable.h
 * @brief Service part table and column definitions.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOSERVICEPARTTABLE_H
#define DBOSERVICEPARTTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_SERVICEPART "servicepart"

// columns
#define COLUMN_DESCRIPTION "description"
#define COLUMN_ID "id"
#define COLUMN_NOTES_REQUIRED "notesRequired"


//
// create table sql
//

#define CREATE_TABLE_SERVICEPART "create table " \
    TABLE_SERVICEPART " ( " \
        COLUMN_ID                   " integer primary key not null, " \
		COLUMN_DESCRIPTION          " string, " \
        COLUMN_NOTES_REQUIRED       " integer  );"

//
// populate table sql
//

#define POPULATE_TABLE_SERVICEPART "insert into " TABLE_SERVICEPART " " \
    "(" COLUMN_ID "," COLUMN_DESCRIPTION "," COLUMN_NOTES_REQUIRED ") values " \
    "(   1, 'Actuator', 0 ), " \
    "(   2, 'Applier', 0 ), " \
    "(   3, 'Arbor / Arbor Shaft', 0 ), " \
    "(   4, 'Arm', 0 ), " \
    "(   5, 'Bar / Support', 0 ), " \
    "(   6, 'Bearing / Bushing', 0 ), " \
    "(   7, 'Belt', 0 ), " \
    "(   8, 'Block', 0 ), " \
    "(   9, 'Bracket', 0 ), " \
    "(  10, 'Brake', 0 ), " \
    "(  11, 'Bumper', 0 ), " \
    "(  12, 'Cable', 0 ), " \
    "(  13, 'Camera', 0 ), " \
    "(  14, 'Channel', 0 ), " \
    "(  15, 'Circuit Board', 0 ), " \
    "(  16, 'Communication', 0 ), " \
    "(  17, 'Compact Flash', 0 ), " \
    "(  18, 'Compressor', 0 ), " \
    "(  19, 'Configuration', 0 ), " \
    "(  20, 'Connector / Terminal', 0 ), " \
    "(  21, 'Conveyor', 0 ), " \
    "(  22, 'Cover', 0 ), " \
    "(  23, 'Cross Brace', 0 ), " \
    "(  24, 'Cylinder', 0 ), " \
    "(  25, 'Data Base / Label Formats', 0 ), " \
    "(  26, 'Drive Roller', 0 ), " \
    "(  27, 'Dryer', 0 ), " \
    "(  28, 'Elevator', 0 ), " \
    "(  29, 'Fan', 0 ), " \
    "(  30, 'Film Chamber/Supply', 0 ), " \
    "(  31, 'Filter / Screen', 0 ), " \
    "(  32, 'Firmware', 0 ), " \
    "(  33, 'Fitting', 0 ), " \
    "(  34, 'Frame', 0 ), " \
    "(  35, 'Fuse / Circuit Breaker', 0 ), " \
    "(  36, 'Gear', 0 ), " \
    "(  37, 'Gripper', 0 ), " \
    "(  38, 'Heat Element', 0 ), " \
    "(  39, 'Heater', 0 ), " \
    "(  40, 'Hold Down', 0 ), " \
    "(  41, 'Hose', 0 ), " \
    "(  42, 'Intake', 0 ), " \
    "(  43, 'Interlock', 0 ), " \
    "(  44, 'Key', 0 ), " \
    "(  45, 'Knife', 0 ), " \
    "(  46, 'Latch', 0 ), " \
    "(  47, 'Lens', 0 ), " \
    "(  48, 'Lever', 0 ), " \
    "(  49, 'Light Tube', 0 ), " \
    "(  50, 'Load Cell', 0 ), " \
    "(  51, 'Misc Hardware', 0 ), " \
    "(  52, 'Motor', 0 ), " \
    "(  53, 'Operator', 0 ), " \
    "(  54, 'O-Ring / Gasket', 0 ), " \
    "(  55, 'Pan', 0 ), " \
    "(  56, 'Part - See Text', 1 ), " \
    "(  57, 'Plate', 0 ), " \
    "(  58, 'Posts', 0 ), " \
    "(  59, 'Power Supply', 0 ), " \
    "(  60, 'Power/Voltage', 0 ), " \
    "(  61, 'Pressure', 0 ), " \
    "(  62, 'Pressure Switch', 0 ), " \
    "(  63, 'Pulley', 0 ), " \
    "(  64, 'Pump', 0 ), " \
    "(  65, 'Pusher', 0 ), " \
    "(  66, 'Rail', 0 ), " \
    "(  67, 'Relay / SSR', 0 ), " \
    "(  68, 'Roller', 0 ), " \
    "(  69, 'Sealer Belt', 0 ), " \
    "(  70, 'Selector', 0 ), " \
    "(  71, 'Sensor', 0 ), " \
    "(  72, 'Shaft', 0 ), " \
    "(  73, 'Side Clamp', 0 ), " \
    "(  74, 'Slats', 0 ), " \
    "(  75, 'Slide', 0 ), " \
    "(  76, 'Slide Blocks', 0 ), " \
    "(  77, 'Slide Washers', 0 ), " \
    "(  78, 'Software', 0 ), " \
    "(  79, 'Solenoid', 0 ), " \
    "(  80, 'Speed Reducer', 0 ), " \
    "(  81, 'Spring / Tensioner', 0 ), " \
    "(  82, 'Start/Stop Actuator', 0 ), " \
    "(  83, 'Stops / Bumper', 0 ), " \
    "(  84, 'Strut', 0 ), " \
    "(  85, 'Switch', 0 ), " \
    "(  86, 'System', 0 ), " \
    "(  87, 'Table', 0 ), " \
    "(  88, 'Take-up', 0 ), " \
    "(  89, 'Tank', 0 ), " \
    "(  90, 'Thermostat', 0 ), " \
    "(  91, 'Timings', 0 ), " \
    "(  92, 'Transformer', 0 ), " \
    "(  93, 'Underfolder', 0 ), " \
    "(  94, 'Vacuum', 0 ), " \
    "(  95, 'Valve', 0 ), " \
    "(  96, 'Wand / Cup', 0 ), " \
    "(  97, 'Weigher', 0 ), " \
    "(  98, 'Wiring', 0 ), " \
    "(  99, 'See Text', 1 ); "

//
// insert sql
//

#define INSERT_SERVICEPART "insert into " \
	TABLE_SERVICEPART " (" \
		COLUMN_ID "," \
		COLUMN_DESCRIPTION ", " \
		COLUMN_NOTES_REQUIRED ") " \
		"values ( ?1, ?2, ?3 );"

//
// update sql
//

#define UPDATE_SERVICEPART "update " \
	TABLE_SERVICEPART " set " \
		COLUMN_DESCRIPTION "=?2, " \
		COLUMN_NOTES_REQUIRED "=?3 " \
		"where " COLUMN_ID "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOSERVICEPARTTABLE_H
