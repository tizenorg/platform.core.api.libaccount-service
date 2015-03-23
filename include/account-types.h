/*
 *
 * Copyright (c) 2012 - 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __ACCOUNT_TYPES_H__
#define __ACCOUNT_TYPES_H__

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @addtogroup CAPI_ACCOUNT_MANAGER_MODULE
 * @{
 */

/**
 * @file     account-types.h
 * @ingroup  CAPI_ACCOUNT
 * @brief    This file defines common types and enums of ACCOUNT.
 */

/**
 * @brief  Enumeration for the state of capability.
 * @since_tizen  2.3
 */
typedef enum
{
    ACCOUNT_CAPABILITY_STATE_INVALID = 0, /**< Account capability is invalid */
    ACCOUNT_CAPABILITY_DISABLED, /**< Account capability is disabled */
    ACCOUNT_CAPABILITY_ENABLED, /**< Account capability is enabled */
    ACCOUNT_CAPABILITY_STATE_MAX
}
account_capability_state_e;

/**
 * @brief  Enumeration for the state of account secrecy.
 * @since_tizen  2.3
 */
typedef enum
{
    ACCOUNT_SECRECY_INVALID = 0, /**< Account secrecy is invalid */
    ACCOUNT_SECRECY_INVISIBLE, /**< Account is not visible */
    ACCOUNT_SECRECY_VISIBLE, /**< Account is visible */
    ACCOUNT_SECRECY_MAX
}
account_secrecy_state_e;

/**
 * @brief  Enumeration for the account sync status.
 * @since_tizen  2.3
 */
typedef enum
{
    ACCOUNT_SYNC_INVALID = 0, /**< Account sync is invalid */
    ACCOUNT_SYNC_NOT_SUPPORT,  /**< Account sync not supported */
    ACCOUNT_SYNC_STATUS_OFF, /**< Account sync supported but all synchronization functionalities are off */
    ACCOUNT_SYNC_STATUS_IDLE, /**< Account sync support and sync status is idle */
    ACCOUNT_SYNC_STATUS_RUNNING, /**< Account sync support and sync status is running */
    ACCOUNT_SUPPORTS_SYNC, /**<  NOT USED, WILL BE REMOVED TO PREVENT BUILD ERROR */
    ACCOUNT_NOT_SUPPORTS_SYNC, /**<  NOT USED, WILL BE REMOVED TO PREVENT BUILD ERROR */
    ACCOUNT_SYNC_MAX
}
account_sync_state_e;

/**
 * @brief  Enumeration for the account auth type.
 * @since_tizen  2.3
 */
typedef enum
{
    ACCOUNT_AUTH_TYPE_INVALID = 0, /**< Auth type is invalid */
    ACCOUNT_AUTH_TYPE_XAUTH, /**< XAuth type */
    ACCOUNT_AUTH_TYPE_OAUTH, /**< OAuth type */
    ACCOUNT_AUTH_TYPE_CLIENT_LOGIN, /**< Client-Login type */
    ACCOUNT_AUTH_TYPE_MAX
}account_auth_type_e;

/**
 * @brief  Enumeration for the account auth data type.
 * @since_tizen  2.3
 */
typedef enum
{
    ACCOUNT_AUTH_DATA_TYPE_BOOLEAN = 0, /**< Auth Data type is boolean, primitive type 'bool' */
    ACCOUNT_AUTH_DATA_TYPE_BYTE, /**< Auth Data type is byte, primitive type 'unsigned char' */
    ACCOUNT_AUTH_DATA_TYPE_SHORT, /**< Auth Data type is 16 bit integer, primitive type 'short int' */
    ACCOUNT_AUTH_DATA_TYPE_USHORT, /**< Auth Data type is 16 bit unsigned integer, primitive type 'unsigned short int' */
    ACCOUNT_AUTH_DATA_TYPE_LONG, /**< Auth Data type is 32 bit integer, primitive type 'long int' */
    ACCOUNT_AUTH_DATA_TYPE_ULONG, /**< Auth Data type is 32 bit unsigned integer, primitive type 'unsigned long int' */
    ACCOUNT_AUTH_DATA_TYPE_LONGLONG, /**< Auth Data type is 64 bit integer, primitive type 'long long int' */
    ACCOUNT_AUTH_DATA_TYPE_ULONGLONG, /**< Auth Data type is 64 bit unsigned integer, primitive type 'unsigned long long int' */
    ACCOUNT_AUTH_DATA_TYPE_DOUBLE, /**< Auth Data type is double, primitive type 'double' */
    ACCOUNT_AUTH_DATA_TYPE_STRING /**< Auth Data type is string, primitive type 'char' */
}account_auth_data_type_e;


/**
 * @brief  The structure type for the Account handle.
 * @since_tizen  2.3
 */
typedef struct account_s* account_h;

/**
 * @brief  The structure type for the Account provider handle.
 * @since_tizen  2.3
 */
typedef struct account_type_s* account_type_h;

/**
 * @brief  The structure type for the Account handle.
 * @since_tizen  2.3
 */
typedef struct label_s* label_h;

/**
 * @brief  The structure type for the Account subscribe handle.
 * @since_tizen  2.3
 */
typedef struct account_subscribe_s* account_subscribe_h;

/**
 * @brief        The insert notification type.
 * @since_tizen  2.3
 * @remarks      When the account database is changed, You can distinguish one event type from the other which are set for subscribing notification.
 *
 * @see account_subscribe_notification()
 */
#define ACCOUNT_NOTI_NAME_INSERT        "insert"

/**
 * @brief        The update notification type.
 * @since_tizen  2.3
 * @remarks      When the account database is changed, You can distinguish one event type from the other which are set for subscribing notification.
 *
 * @see account_subscribe_notification()
 */
#define ACCOUNT_NOTI_NAME_UPDATE        "update"

/**
 * @brief        The delete notification type.
 * @since_tizen  2.3
 * @remarks      When the account database is changed, You can distinguish one event type from the other which are set for subscribing notification.
 *
 * @see account_subscribe_notification()
 */
#define ACCOUNT_NOTI_NAME_DELETE        "delete"

/**
 * @brief        The sync_update notification type.
 * @since_tizen  2.3
 * @remarks      When the account database is changed, You can distinguish one event type from the other which are set for subscribing notification.
 *
 * @see account_subscribe_notification()
 */
#define ACCOUNT_NOTI_NAME_SYNC_UPDATE   "sync_update"


/**
 * @brief        This is a key of app_control_h’s extra data. The value of the key is a user account name.
 * @since_tizen  2.3
 * @remarks      You can retrieve the value like this, app_control_get_extra_data(app_control, ACCOUNT_DATA_USERNAME, &username).\n
                 This value will be sent with ACCOUNT_OPERATION_VIEW.
 */
#define ACCOUNT_DATA_USERNAME                             "http://tizen.org/account/data/username"

/**
 * @brief        This is a key of app_control_h’s extra data. The value of the key is account DB ID of the user’s account information.
 * @since_tizen  2.3
 * @remarks      You can retrieve the value like this, app_control_get_extra_data(app_control, ACCOUNT_DATA_ID, &id).\n
                 This value will be sent with ACCOUNT_OPERATION_VIEW.
 */
#define ACCOUNT_DATA_ID                                   "http://tizen.org/appcontrol/data/account/id"

/**
 * @brief        This is an application launch operation name.
 * @since_tizen  2.3
 * @remarks      This operation will be sent when a user selects your account in add account view.
 */
#define ACCOUNT_OPERATION_SIGNIN                          "http://tizen.org/appcontrol/operation/account/add"

/**
 * @brief        This is an application launch operation name.
 * @since_tizen  2.3
 * @remarks      This operation will be sent when a user selects your account in account list view.
 */
#define ACCOUNT_OPERATION_VIEW                            "http://tizen.org/appcontrol/operation/account/configure"

/**
 * @brief        This is contact capability string.
 * @since_tizen  2.3
 * @remarks      If your provider service has contact information and contact information is provided another application, register this capability through manifest file.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_CONTACT               "http://tizen.org/account/capability/contact"

/**
 * @brief        This is calendar capability string.
 * @since_tizen  2.3
 * @remarks      If your provider service has calendar information and calendar information is provided another application, register this capability through manifest file.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_CALENDAR              "http://tizen.org/account/capability/calendar"

/**
 * @brief        This is email capability string.
 * @since_tizen  2.3
 * @remarks      If your provider service have email service and email is provided another application, register this capability through manifest file.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_EMAIL                 "http://tizen.org/account/capability/email"

/**
 * @brief        This is photo capability string.
 * @since_tizen  2.3
 * @remarks      If your provider service has photo and photo is provided another application, register this capability through manifest file.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_PHOTO                 "http://tizen.org/account/capability/photo"

/**
 * @brief        This is video capability string.
 * @since_tizen  2.3
 * @remarks      If your provider service has video and video is provided another application, register this capability through manifest file.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_VIDEO                 "http://tizen.org/account/capability/video"

/**
 * @brief        This is music capability string.
 * @since_tizen  2.3
 * @remarks      If your provider service has music and music is provided another application, register this capability through manifest file.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_MUSIC                 "http://tizen.org/account/capability/music"

/**
 * @brief        This is document capability string.
 * @since_tizen  2.3
 * @remarks      If your provider service has document and document is provided another application, register this capability through manifest file.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_DOCUMENT                 "http://tizen.org/account/capability/document"

/**
 * @brief        This is message capability string.
 * @since_tizen  2.3
 * @remarks      This capability will be exploited to use SMS, MMS.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_MESSAGE               "http://tizen.org/account/capability/message"

/**
 * @brief        This is game capability string.
 * @since_tizen  2.3
 * @remarks      This capability will be exploited to use game data.
 */
#define ACCOUNT_SUPPORTS_CAPABILITY_GAME               "http://tizen.org/account/capability/game"

/**
 * @brief Authenticator app control must handle this operation ID if it wants to support SSO
 * @since 2.3
 * @see account_request_auth_token()
 */
#define ACCOUNT_OPERATION_GET_AUTH_TOKEN                  "http://tizen.org/appcontrol/operation/account/get_auth_token"

/**
 * @brief account_auth_data predefined key. Used by the account provider in account_auth_data_h.
 * @since 2.3
 */
#define ACCOUNT_AUTHENTICATION_RESPONSE_USER_NAME "user_name"

/**
 * @brief account_auth_data predefined key. Used by the account provider in account_auth_data_h.
 * @since 2.3
 */
#define ACCOUNT_AUTHENTICATION_RESPONSE_SECRET  "secret"

/**
 * @brief account_auth_data predefined key. Used by the consumer application in account_auth_data_h, to specify which mechanism to use for authentication.
 * @since 2.3
 */
#define ACCOUNT_AUTH_DATA_AUTH_MECHANISM "mechanism"

/**
 * @brief Predefined app_control_h key for ACCOUNT_OPERATION_GET_AUTH_TOKEN response. Used by the account provider to propagate service provider\n
 * specific error codes back to the consumer application. For success cases, app_control_h must not have this key
 * @since 2.3
 */
#define ACCOUNT_AUTHENTICATION_ERROR_CODE "error_code"

/**
 * @brief Predefined app_control_h key for ACCOUNT_OPERATION_GET_AUTH_TOKEN response. Used by the account provider to propagate service provider\n
 * specific error message back to the consumer application. For success cases, app_control_h must not have this key
 * @since 2.3
 */
#define ACCOUNT_AUTHENTICATION_ERROR_MSG "error_msg"
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_TYPES_H__*/

