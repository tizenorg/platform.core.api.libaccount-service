/*
 *  account
 *
 * Copyright (c) 2012 - 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Wonyoung Lee <wy1115.lee@samsung.com>, Sungchan Kim <sungchan81.kim@samsung.com>
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

#ifndef __ACCOUNT_INTERNAL_H__
#define __ACCOUNT_INTERNAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ACCOUNT_INTERNAL_API
#define ACCOUNT_INTERNAL_API __attribute__ ((visibility("default")))
#endif

/**
 * @file        account_internal.h
 * @brief       This file contains the Account API for account management.
 */

/**
 * @internal
 * @brief  Deletes an account from the account database by package name without permission check.
 *
 * @since_tizen 2.4
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read \n
 *             %http://tizen.org/privilege/account.write
 * @remarks    This API need both privileges
 * @param[in]  package_name  The package name of account(s) to delete
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @pre  This function requires an open connection to an account service by account_connect().
 *
 * @see account_connect()
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
int account_delete_from_db_by_package_name_without_permission(const char *package_name);

/**
 * @internal
 * @brief  Updates the account details to the account database without checking provider's permission.
 *
 * @since_tizen 2.3
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read \n
 *             %http://tizen.org/privilege/account.write
 * @remarks    This API need both privileges
 * @param[in]  account     The account handle
 * @param[in]  account_id  The account ID to update
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   The account to update does not exist
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @pre  This function requires an open connection to an account service by account_connect().
 *
 * @see account_connect()
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_user_name()
 */
int account_update_to_db_by_id_without_permission(account_h account, int account_id);

/**
 * @internal
 * @brief  Sets the app ID.
 *         It should be filled with your application ID. For example, com.tizen.testapp.
 *
 * @since_tizen 2.3
 * @remarks     @a app_id is a mandatory field and does not allow duplicate app ID in the account provider database.
 *
 * @param[in]   account_type  The account provider handle \n
 *                            It should be assigned by account_type_create().
 * @param[in]   app_id        The application ID
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_get_app_id()
 */
int account_type_set_app_id(account_type_h account_type, const char *app_id);

/**
 * @internal
 * @brief  Sets the service provider ID.
 *         It should be filled with your service provider ID.
 *
 * @since_tizen 2.3
 * @remarks    @a service_provider_id is a mandatory field.
 *
 * @param[in]  account_type         The account provider handle \n
 *                                  It should be assigned by account_type_create().
 * @param[in]  service_provider_id  The service provider ID
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_get_service_provider_id()
 */
int account_type_set_service_provider_id(account_type_h account_type, const char *service_provider_id);

/**
 * @internal
 * @brief  Sets icon path.
 *         It represents your service provider or an application.
 *
 * @since_tizen 2.3
 * @remarks     @a icon_path is not a mandatory field. But when it is set, you can display this icon in the Add Account screen.
 *
 * @param[in]  account_type  The account provider handle\n
 *                           It should be assigned by account_type_create().
 * @param[in]  icon_path     The icon path of the application
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_get_icon_path()
 */
int account_type_set_icon_path(account_type_h account_type, const char *icon_path);

/**
 * @internal
 * @brief  Sets small icon path.
 *         It also represents your service provider or an application.
 *
 * @since_tizen 2.3
 * @remarks     @a small_icon_path is not a mandatory field.
 *
 * @param[in]   account_type     The account provider handle \n
 *                               It should be assigned by account_type_create().
 * @param[in]   small_icon_path  The scaled icon of your application
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_get_small_icon_path()
 */
int account_type_set_small_icon_path(account_type_h account_type, const char *small_icon_path);

/**
 * @internal
 * @brief  Sets support for multiple accounts.
 *         It represents whether your application supports multiple accounts.
 *
 * @since_tizen 2.3
 * @remarks     The default value of multiple account support is @c FALSE.
 *
 * @param[in]   account_type              The account provider handle \n
 *                                        It should be assigned by account_type_create().
 * @param[in]   multiple_account_support  Set @c TRUE if your application can support two or more accounts, \n
 *                                        otherwise @c FALSE if your application can support only one account
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_get_multiple_account_support()
 */
int account_type_set_multiple_account_support(account_type_h account_type, const bool multiple_account_support);

/**
 * @internal
 * @brief  Sets label and locale.
 *         Label represents the name of an account provider.
 *
 * @since_tizen 2.3
 * @param[in]   account_type  The account provider handle \n
 *                            It should be assigned by account_type_create().
 * @param[in]   label         The name of account depends on the specified locale
 * @param[in]   locale        The locale is specified as an ISO 3166 alpha-2 two letter country-code followed by ISO 639-1 for the two-letter language code.\n
 *                            For example, "ko_KR" for Korean, "en_US" for American English.
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_get_label()
 */
int account_type_set_label(account_type_h account_type, const char* label, const char* locale);

/**
 * @internal
 * @brief  Sets the capability.
 *
 * @since_tizen 2.3
 * @param[in]   account_type      The account provider handle
 * @param[in]   provider_feature  Th capability key of the account
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_set_capability()
 */
int account_type_set_provider_feature(account_type_h account_type, const char* provider_feature);

/**
 * @internal
 * @brief  Inserts the account provider details to the database.
 *
 * @since_tizen 2.3
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read \n
 *              %http://tizen.org/privilege/account.write
 * @remarks     this API need both privileges
 * @param[in]   account_type     The account handle which is created by account_type_create() \n
 * @param[out]  account_type_id  The account provider ID to be assigned after inserting the account provider handle
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @pre   This function requires an open connection to an account service by account_connect().
 * @post  account_disconnect() is recommended after insertion.
 *
 * @see account_connect()
 * @see account_disconnect()
 */
int account_type_insert_to_db(account_type_h account_type, int* account_type_id);

/**
 * @internal
 * @brief  Updates the account details to the account database.
 *
 * @since_tizen 2.3
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read \n
 *              %http://tizen.org/privilege/account.write
 * @remarks     this API need both privileges
 * @param[in]   account_type  The account handle which is created by account_type_create() \n
 * @param[in]   app_id        The application ID of the account provider
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 *
 * @pre   This function requires an open connection to an account service by account_connect().
 * @post  account_disconnect() is recommended after update.
 *
 * @see account_connect()
 * @see account_disconnect()
 */
int account_type_update_to_db_by_app_id(const account_type_h account_type, const char* app_id);

/**
 * @internal
 * @brief  Deletes the account provider from the account database by application ID.
 *
 * @since_tizen 2.3
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read \n
 *              %http://tizen.org/privilege/account.write
 * @remarks     this API need both privileges
 * @param[in]   app_id  The application ID of account provider to be deleted
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @pre  This function requires an open connection to an account service by account_connect().
 *
 * @see account_connect()
 * @see account_disconnect()
 */
int account_type_delete_by_app_id(const char* app_id);

/**
 * @internal
 * @brief       Start to subscribe account event through the given callback function
 *
 * @param[in]   account_subscribe       The account subscription handle
 * @param[in]   cb_func When an account is removed from account database. It will be called with event message and account id.
 * @param[in]   user_data user_data will be delivered to cb_func
 *
 * @return      0 on success, otherwise a negative error value.
 * @retval      #ACCOUNT_ERROR_NONE Successful
 * @retval      #ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL Subscription fail
 * @retval      #ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_unsubscribe_notification()
 * @see account_subscribe_notification()
 */
ACCOUNT_API int account_subscribe_notification_ex(account_subscribe_h account_subscribe, account_event_cb cb_func, void* user_data);

/**
 * @internal
 * @brief       Start to subscribe account event through the given callback function
 *
 * @param[in]   account_subscribe       The account subscription handle
 * @param[in]   cb_func When an account is removed from account database. It will be called with event message and account id.
 * @param[in]   user_data user_data will be delivered to cb_func
 *
 * @return      0 on success, otherwise a negative error value.
 * @retval      #ACCOUNT_ERROR_NONE Successful
 * @retval      #ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL Subscription fail
 * @retval      #ACCOUNT_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see account_unsubscribe_notification()
 * @see account_subscribe_notification()
 */
ACCOUNT_API int account_unsubscribe_notification_ex(account_subscribe_h account_subscribe);


/*offline apis*/
int account_type_insert_to_db_offline(account_type_h account_type, int* account_type_id);

int account_type_delete_by_app_id_offline(const char* app_id);

/**
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_INTERNAL_H__ */
