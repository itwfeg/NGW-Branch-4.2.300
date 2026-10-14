/**
 * @file sessioneventtype.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "sessioneventtype.h"

const QVector<uint> SIMPLE_EVENT_TYPE;

// ============================================================================
// Session Manager
// ============================================================================

const QVector<uint> SESSION_MAN_CREATE_SESSION = {4};
const QVector<uint> SESSION_MAN_CREATE_NAMED_SESSION = {4, 0};
const QVector<uint> SESSION_MAN_DESTROY_SESSION = {0};
const QVector<uint> SESSION_MAN_RESUME_NAMED_SESSION = {0};

const QVector<uint> SESSION_MAN_REG_SERVICE_ALIAS = {0};
const QVector<uint> SESSION_MAN_UNREG_SERVICE_ALIAS = {0};

const QVector<uint> SESSION_MAN_SESSION_CREATED = {0};
const QVector<uint> SESSION_MAN_SESSION_RESUMED = {0};

// ============================================================================
// Session
// ============================================================================

const QVector<uint> SESSION_REGISTER_EVENT = {4, 4};
const QVector<uint> SESSION_UNREGISTER_EVENT = {4, 4};
