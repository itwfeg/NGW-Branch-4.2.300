/**
 * @file stringsconfig.h
 * Scale config path strings.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef STRINGSCONFIG_H
#define STRINGSCONFIG_H

///////////////////////////////////////////////////////////////////////////////////////////////////

#define CONF_UUID                               "config/uuid"

#define CONF_REQUEST_TIMEOUT                    "scalableItemPull/timeout"
#define CONF_REQUEST_MAX_ATTEMPTS               "scalableItemPull/retry"

#define CONF_DOWNLOAD_URL                       "heartbeat/downloadUrl"
#define CONF_HEARTBEAT_URL                      "heartbeat/url"
#define CONF_HEARTBEAT_BACKUPURL                "heartbeat/backupUrl"
#define CONF_HEARTBEAT_ENDPOINT                 "heartbeat/endpoint"
#define CONF_HEARTBEAT_INTERVAL_ENABLED         "heartbeat/triggers/onInterval"
#define CONF_HEARTBEAT_STARTUP_ENABLED          "heartbeat/triggers/onStartup"
#define CONF_HEARTBEAT_SHUTDOWN_ENABLED         "heartbeat/triggers/onShutdown"
#define CONF_HEARTBEAT_RECONNECT_ENABLED        "heartbeat/triggers/onReconnect"
#define CONF_HEARTBEAT_RECONNECT_OFFSET         "heartbeat/triggers/reconnectOffset"

#define CONF_HEALTH_CHECK_INTERVAL              "eventController/timerInterval"

#define CONF_UPDATE_CONFIG_ENABLED              "eventController/events/changeConfiguration"
#define CONF_DOWNLOAD_FILE_ENABLED              "eventController/events/downloadFile"
#define CONF_APPLY_SETTINGS_ENABLED             "eventController/events/applyScaleSettings"
#define CONF_FETCH_PLU_ENABLED                  "eventController/events/fetchPlu"
#define CONF_UPGRADE_FIRMWARE_ENABLED           "eventController/events/upgradeFirmware"
#define CONF_INSTALL_FEATURE_ENABLED            "eventController/events/installFeature"
#define CONF_LOAD_LABELS_ENABLED                "eventController/events/loadLabel"

#define CONF_OFFLINE_TIMER_MAX                  "offlineDialogs/firstPrintingMessage/start"
#define CONF_MESSAGE_FADE                       "offlineDialogs/messageFade"

#define CONF_FIRST_OFFLINE_MSG_START            "offlineDialogs/firstOfflineMessage/start"
#define CONF_FIRST_OFFLINE_MSG_STOP             "offlineDialogs/firstOfflineMessage/stop"
#define CONF_FIRST_OFFLINE_MSG_FREQ             "offlineDialogs/firstOfflineMessage/frequency"

#define CONF_SECOND_OFFLINE_MSG_START           "offlineDialogs/secondOfflineMessage/start"
#define CONF_SECOND_OFFLINE_MSG_STOP            "offlineDialogs/secondOfflineMessage/stop"
#define CONF_SECOND_OFFLINE_MSG_FREQ            "offlineDialogs/secondOfflineMessage/frequency"

#define CONF_FIRST_PRINTING_MSG_START           "offlineDialogs/firstPrintingMessage/start"
#define CONF_FIRST_PRINTING_MSG_STOP            "offlineDialogs/firstPrintingMessage/stop"

#define CONF_SECOND_PRINTING_MSG_START          "offlineDialogs/secondPrintingMessage/start"
#define CONF_SECOND_PRINTING_MSG_STOP           "offlineDialogs/secondPrintingMessage/stop"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // STRINGSCONFIG_H
