/**
 * @file stringsjson.h
 * JSON strings.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef STRINGSJSON_H
#define STRINGSJSON_H

// uncomment to use old meta events method for file downloads
//#define USE_META_DOWNLOADS

///////////////////////////////////////////////////////////////////////////////////////////////////

// header info
#define JSON_SCALE                      "scale"
#define JSON_SERIAL_NUMBER              "serialNbr"
#define JSON_STORE_NUMBER               "storeNbr"
#define JSON_COUNTRY_CODE               "countryCode"
#define JSON_SCALE_VENDOR               "scaleVendor"
#define JSON_SCALE_MODEL                "scaleModel"
#define JSON_SCALE_MODEL_TEXT           "scaleModelText"

#define JSON_REQUEST_UUID               "requestUuid"
#define JSON_REQUEST_TIMESTAMP          "requestTimestamp"
#define JSON_RESPONSE_UUID              "responseUuid"
#define JSON_RESPONSE_TIMESTAMP         "responseTimestamp"

#define JSON_MDM_LOCATION_MODEL         "mdmLocationModel"
#define JSON_DIVISION_NBR               "divisionNbr"

// health check
#define JSON_HEALTH_CHECK               "healthCheck"
#define JSON_TRIGGER_TYPE               "triggerType"
#define JSON_CONFIG_UUID                "configUuid"
#define JSON_LAST_REBOOT_STAMP          "lastRebootTimestamp"
#define JSON_LAST_COMM_STAMP            "lastCommunicationTimestamp"
#define JSON_LAST_ITEM_CHANGE_STAMP     "lastItemChangeReceivedTimestamp"

// events
#define JSON_EVENTS                     "events"
#define JSON_EVENT                      "event"
#define JSON_UUID                       "uuid"
#define JSON_URL                        "url"
#define JSON_ALWAYS_ENABLED             "alwaysEnabled"
#define JSON_CALLBACK_URL               "callbackUrl"

#define JSON_METADATA                   "metadata"
#define JSON_PACKAGE_FILENAME           "packageFileName"
#define JSON_FIRMWARE_SIZE              "firmwareSize"
#define JSON_CHECKSUM                   "checkSum"

#define JSON_DOWNLOAD                   "download"

#define JSON_FILES                      "files"
#define JSON_FILENAME                   "fileName"
#define JSON_FILE_SIZE                  "fileSize"
#define JSON_FILE_CHECKSUM              "fileCheckSum"
#define JSON_FILE_SEQ_NO                "fileSequenceNbr"

#define JSON_DOWNLOAD_URL               "downloadUrl"
#define JSON_FEATURE_SIZE_NBR           "featureSizeNbr"
#define JSON_FEATURE_CHECKSUM           "featureChecksum"
#define JSON_FIRMWARE_SIZE_NBR          "firmwareSizeNbr"
#define JSON_FIRMWARE_CHECKSUM          "firmwareChecksum"
#define JSON_LABELS_SIZE_NBR            "labelsSizeNbr"
#define JSON_LABELS_CHECKSUM            "labelsChecksum"
#define JSON_PACKAGE_FILENAME           "packageFileName"

// settings
#define JSON_SETTINGS                   "settings"
#define JSON_SCALE_MODE                 "scaleMode"
#define JSON_SCALE_ITEM_ENTRY_TYPE      "scaleItemEntryType"
#define JSON_LABEL_STOCK                "labelStock"
#define JSON_HAS_LOAD_CELL              "hasLoadCell"
#define JSON_WEIGHER_PRECISION          "weigherPrecision"
#define JSON_WEIGHER_CALIB_STAMP        "weigherCalibratedTimestamp"
#define JSON_WEIGHER_CONF_STAMP         "weigherConfiguredTimestamp"
#define JSON_HAS_CUSTOMER_DISPLAY       "hasCustomerDisplay"
#define JSON_TIME_ZONE                  "timezone"
#define JSON_TIME_KEEPER                "timeKeeper"

#define JSON_STORE                      "store"
#define JSON_STORE_ID                   "storeId"
#define JSON_STORE_NAME                 "storeInformation"
#define JSON_STORE_GRAPHIC              "storeGraphicText"

#define JSON_NTP                        "ntp"
#define JSON_SERVER                     "server"
#define JSON_FREQUENCY                  "frequency"
#define JSON_DAY_OF_WEEK                "dayOfWeek"
#define JSON_TIME_OF_DAY                "timeOfDay"
#define JSON_ANTI_MERIDIAN              "antiMeridian"

#define JSON_NETWORK                    "network"
#define JSON_HOST_NAME                  "hostName"
#define JSON_CONNECTION_TYPE            "connectionType"
#define JSON_WIRED                      "wired"
#define JSON_WIRELESS                   "wireless"
#define JSON_IP_CONFIG                  "ipConfig"
#define JSON_DNS                        "dns"
#define JSON_CONNECTION_SPEED           "connectionSpeed"

#define JSON_MAC_ADDRESS                "macAddress"
#define JSON_IP_ADDRESS4                "ip4Address"
#define JSON_IP_ADDRESS6                "ip6Address"

#define JSON_IP_ADDRESS                 "ipAddress"

#define JSON_CHANNEL                    "channel"
#define JSON_AUTO_LINK_RATE             "autoLinkRate"
#define JSON_LINK_RATE                  "linkRate"
#define JSON_SIGNAL_STRENGTH            "signalStrength"
#define JSON_ADAPTER                    "adapter"
#define JSON_AUTHENTICATION             "authentication"

#define JSON_SSID                       "ssid"
#define JSON_SECURITY_METHOD            "securityMethod"
#define JSON_ENCRYPTION                 "encryption"

#define JSON_DEFAULT_KEY                "defaultKey"
#define JSON_KEY1                       "key1"
#define JSON_KEY2                       "key2"
#define JSON_KEY3                       "key3"
#define JSON_KEY4                       "key4"

#define JSON_USERNAME                   "username"
#define JSON_PASSWORD                   "password"

#define JSON_SERVER_CERT                "serverCert"
#define JSON_CLIENT_CERT                "clientCert"
#define JSON_CHAIN                      "chain"
#define JSON_CSR                        "csr"
#define JSON_CSR_KEY                    "csrKey"
#define JSON_CLIENT_KEY                 "clientKey"
#define JSON_CLIENT_CERT_USERS          "clientCertUsers"
#define JSON_CLIENT_CERT_PASSWORD       "clientCertPassword"

#define JSON_PASSPHRASE                 "passphrase"

#define JSON_IS_STATIC                  "isStatic"
#define JSON_DHCP_SERVER                "dhcpServer"
#define JSON_SUBNET_MASK                "subnetMask"
#define JSON_GATEWAY                    "defaultGateway"

#define JSON_AUTO_DNS                   "autoDns"
#define JSON_DNS_SERVER1                "dnsServer1"
#define JSON_DNS_SERVER2                "dnsServer2"
#define JSON_DNS_SERVER3                "dnsServer3"
#define JSON_DNS_SUFFIX1                "dnsSuffix1"
#define JSON_DNS_SUFFIX2                "dnsSuffix2"
#define JSON_DNS_SUFFIX3                "dnsSuffix3"
#define JSON_DNS_SUFFIX4                "dnsSuffix4"
#define JSON_DNS_SUFFIX5                "dnsSuffix5"
#define JSON_DNS_SUFFIX6                "dnsSuffix6"

#define JSON_SNMP                       "snmp"
#define JSON_IS_ENABLED                 "isEnabled"
#define JSON_VARIABLES_PORT             "variablesPort"
#define JSON_TRAPS_IP                   "trapsIp"
#define JSON_TRAPS_PORT                 "trapsPort"

#define JSON_FIRMWARE                   "firmware"
#define JSON_BUILD_NUMBER               "buildNbr"
#define JSON_APPLICATION                "application"
#define JSON_OPERATING_SYSTEM           "operatingSystem"
#define JSON_SYSTEM_CONTROLLER          "systemController"
#define JSON_LOADER                     "loader"
#define JSON_SM_BACKEND                 "smBackend"
#define JSON_FEATURES                   "features"
#define JSON_LICENSES                   "licenses"

#define JSON_PERIPHERALS                "peripherals"
#define JSON_DESCRIPTION                "description"
#define JSON_TYPE                       "type"
#define JSON_SW_VER                     "softwareVersion"
#define JSON_HW_VER                     "hardwareVersion"

#define JSON_DISK                       "disks"
#define JSON_DEVICE                     "device"
#define JSON_DISPLAY_NAME               "displayName"
#define JSON_FILE_SYSTEM_TYPE           "fileSystemType"
#define JSON_SIZE                       "size"
#define JSON_USED                       "used"
#define JSON_FREE                       "free"

#define JSON_USAGE_TODAY                "usageToday"
#define JSON_CAKE_PRINTS                "cakePrints"
#define JSON_PLU_PRINTS                 "pluPrints"
#define JSON_PLU_NUM                    "pluNbr"
#define JSON_MANUAL_PRINTS              "manualPrints"
#define JSON_REMOTE_PRINTS              "remotePrints"

#define JSON_LABELS                     "labels"
#define JSON_LABEL_NUMBER               "labelNbr"
#define JSON_LABEL_NAME                 "labelName"
#define JSON_VERSION                    "version"

// settings event
#define JSON_CURRENT_SETTINGS           "currentSettings"

#define JSON_REBOOT_SCALE               "rebootScale"
#define JSON_NEW_SETTINGS               "newSettings"

// configuration
#define JSON_CONFIG                     "config"
#define JSON_SCALE_CONFIG               "scaleConfig"
#define JSON_SITE                       "site"

// configuration event
#define JSON_CURRENT_CONFIG             "currentConfiguration"
#define JSON_CONFIG_STAMP               "configTimestamp"

#define JSON_NEW_CONFIGURATION          "newConfiguration"

// fetch plu
#define JSON_PLU                        "plu"
#define JSON_LAST_FETCH_PLU_STAMP       "lastFetchPluOffsetTimestamp"
#define JSON_PLU_RECORD_COUNT           "pluRecordCount"
#define JSON_BLOCKED_PLUS               "blockedPluList"

// fetch plu event
#define JSON_DELETE_ALL_PRODUCTS        "removeProductsOnNotInList"
#define JSON_NEW_FETCH_PLU_STAMP        "newFetchPluOffsetTimestamp"

#define JSON_DELETE_PRODUCTS            "deletePluList"

#define JSON_PRODUCTS                   "products"

#define JSON_COOL                       "cool"
#define JSON_COOL_PRE_TEXT              "coolPretexts"
#define JSON_COOL_COUNTRIES             "coolCountries"
#define JSON_COOL_PROD_STEPS            "coolProductionSteps"
#define JSON_COOL_CATS                  "coolCategories"
#define JSON_COOL_TEXT                  "coolTexts"

// callback
#define JSON_CALLBACK                   "callback"
#define JSON_CALLBACK_UUID              "callbackUuid"
#define JSON_CALLBACK_EVENT_TYPE        "callbackEventType"
#define JSON_IS_EVENT_SUCCESS           "isEventSuccessful"
#define JSON_IS_BACKOUT_SUCCESS         "isBackoutSuccessful"
#define JSON_OUTPUT                     "output"

#define JSON_LOG_TEXT                   "logText"
#define JSON_ERROR_TEXT                 "errorText"

#define JSON_ACKNOWLEDGEMENT            "acknowledgement"

// product
#define JSON_PRODUCT_NUM                "productNbr"
#define JSON_PRODUCT_TYPE               "productType"
#define JSON_PRODUCT_DESC               "productDescription"

#define JSON_PRICING                    "pricing"
#define JSON_PRODUCT_LIFE               "productLife"
#define JSON_LABELING                   "labeling"
#define JSON_TEXTS                      "texts"
#define JSON_NUTRITION_FACTS            "nutritionFacts"
#define JSON_COOL                       "cool"

#define JSON_IS_REMOTE_PRINT_ONLY       "isRemotePrintOnly"

#define JSON_COOL_TEXT_LIST             "coolText"
#define JSON_COOL_TEXT_NUM              "coolTextNbr"
#define JSON_COOL_TEXT_SEQ_NO           "coolTextSequenceNbr"

#define JSON_COOL_TEXT_ID               "coolTextId"
#define JSON_COOL_TEXT_NAME             "coolTextName"

// cool
#define JSON_COOL_CAT_PRE_TEXT          "coolCategoryPretextIdList"
#define JSON_COOL_CAT_COUNTRY           "coolCategoryCountryIdList"
#define JSON_COOL_CAT_PROD_STEP         "coolCategoryProductionStepIdList"

// exception
#define JSON_EXCEPTION                  "exception"

#define JSON_TIMESTAMP                  "timestamp"
#define JSON_STATUS                     "status"
#define JSON_ERROR                      "error"
#define JSON_MESSAGE                    "message"
#define JSON_PATH                       "path"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // STRINGSJSON_H
