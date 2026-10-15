#ifndef COMMAND_OPERATIONS_H
#define COMMAND_OPERATIONS_H

#include "Printer.h"

#define MaxTeachTableEntries 30

typedef enum CommandIdentifier
{
	Command0,
	Command1,
	Command2,
	Command3,
	Command4,
	Command5,
	Command6,
	Command7,
	Command8
} CommandIdentifier_t;

/* Command Directives */
typedef enum CommandDirective
{
	IdleDirective,
	PrintDirective,
	StepDirective,
	StepUntilDirective,
	WaitDirective,
	WaitUntilDirective,
	TestDirective,
	StatusDirective,
	MaskDirective,
	CounterDirective,
	DisableDirective,
    CutDirective = 14,
} CommandDirective_t;

/* Command Data Types */
typedef enum CommandDataType
{
	DirectCommandData,
	IndirectCommandData
} CommandDataType_t;

/* Test Operators */
typedef enum BitComparator
{
	BitsEqual,
	BitsNotEqual
} BitComparator_t;

/* Test Actions */
typedef enum TestAction
{
	ContinueExecution,
	AbortExecution
} TestAction_t;

/* Bit Operators */
typedef enum BitOperator
{
	SetBits,
	ClearBits
} BitOperator_t;

/* Counter Operators */
typedef enum CounterOperator
{
	ResetCounter,
	EnableCounter,
	DisableCounter
} CounterOperator_t;

/* Label Orientation Identifiers */
typedef enum LabelFeedDirection
{
	HeelFirst,
	HeadFirst
} LabelFeedDirection_t;

typedef struct
{
	UINT8 commandDirective;
	UINT8 pad[7];
} GenericOperation, IdleOperation;

typedef struct
{
	UINT8	commandDirective;
    UINT8	type; 
    UINT16	data; 
    UINT8	labelFeedDirection;
    UINT8	pad[3];
} PrintOperation;

typedef struct
{
    UINT8	commandDirective;
    UINT8	type; 
    UINT16	data; 
    UINT8	pad[4];
} StepOperation, WaitOperation;

typedef struct
{
    UINT8	commandDirective;
    UINT8	type; 
    UINT16	data;
    UINT8	bitOperation; 
    UINT8	bits;
    UINT8	result;
    UINT8	pad;
} StepUntilOperation;

typedef struct
{
    UINT8	commandDirective;
    UINT8	bitOperation; 
    UINT8	bits;
    UINT8	result;
    UINT8	pad[4];
} WaitUntilOperation;

typedef struct
{
    UINT8	commandDirective;
    UINT8	bitOperation; 
    UINT8	bits;
    UINT8	result;
    UINT8	trueBits;
    UINT8	falseBits;
    UINT8	trueAction;
    UINT8	falseAction;
} TestOperation;

typedef struct
{
    UINT8	commandDirective;
    UINT8	bitOperator;
    UINT8	bits;
    UINT8	pad[5];
} StatusOperation;

typedef struct
{
    UINT8	commandDirective;
    UINT8	counterOperator;
    UINT8	pad[6];
} CounterOperation;

typedef union
{
    GenericOperation    generic;
    IdleOperation       idle;
    PrintOperation      print;
    StepOperation       step;
    StepUntilOperation  stepUntil;
    WaitOperation       wait;
    WaitUntilOperation  waitUntil;
    TestOperation       test;
    StatusOperation     status;
    CounterOperation    counter;
} CommandOperationMsg;

#endif // COMMAND_OPERATIONS_H
