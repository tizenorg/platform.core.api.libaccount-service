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

#ifndef __ACCOUNT_H__
#define __ACCOUNT_H__

#include <account-types.h>
#include <account-error.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ACCOUNT_API
#define ACCOUNT_API __attribute__ ((visibility("default")))
#endif

/**
 * @file        account.h
 * @brief       This file contains the Account API for account management.
 */

/**
 * @addtogroup CAPI_ACCOUNT_MANAGER_MODULE
 * @{
 */

/**
 * @brief   Called once for each account from the database.
 *
 * @since_tizen 3.0
 * @param[in]  account    The account handle
 * @param[in]  user_data  The user data passed from the foreach function
 *
 * @return     @c true to continue with the next iteration of the loop, \n
 *             otherwise @c false to break out of the loop
 *
 * @pre account_foreach_account_from_db(), account_query_account_by_account_id(), account_query_account_by_user_name() or account_query_account_by_package_name() must be called.
 *
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 */
typedef bool (*account_cb)(account_h account, void *user_data);

/**
 * @brief  Called once for each capability of an account in the database.
 *
 * @since_tizen 3.0
 * @param[in]   capability_type  The capability type
 * @param[in]   capability_state The capability state
 * @param[in]   user_data        The user data passed from the foreach function
 *
 * @return      @c true to continue with the next iteration of the loop, \n
 *              otherwise @c false to break out of the loop
 *
 * @pre account_query_capability_by_account_id() must be called.
 *
 * @see account_query_capability_by_account_id()
 */
typedef bool (*capability_cb)(const char *capability_type, account_capability_state_e capability_state, void *user_data);

/**
 * @brief  Called once for each custom data of an account in the database.
 *
 * @since_tizen 3.0
 * @param[in]  key        The user custom key
 * @param[in]  value      The user custom value of the specific key
 * @param[in]  user_data  The user data passed
 *
 * @return     @c true to continue with the next iteration of the loop, \n
 *             otherwise @c false to break out of the loop
 *
 * @pre account_query_capability_by_account_id() must be called.
 *
 * @see account_query_capability_by_account_id()
 */
typedef bool (*account_custom_cb)(char *key, char *value, void *user_data);


/**
 * @brief  Called once for each account provider in the database.
 *
 * @since_tizen 3.0
 * @param[in]  account_type  The account provider handle
 * @param[in]  user_data     The user data passed
 *
 * @return     @c true to continue with the next iteration of the loop, \n
 *             otherwise @c false to break out of the loop
 *
 * @pre account_type_foreach_account_type_from_db(), account_type_query_by_provider_feature() must be called.
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_provider_feature()
 */
typedef bool (*account_type_cb)(account_type_h account_type, void *user_data);


/**
 * @brief  Called once for each account label.
 *
 * @since_tizen 3.0
 * @param[in]  app_id     The application ID
 * @param[in]  label      The name of the account depends on the specified locale
 * @param[in]  locale     The locale is specified as an ISO 3166 alpha-2 two letter country-code followed by ISO 639-1 for the two-letter language code.\n
 *                        For example, "ko_KR" for Korean, "en_US" for American English.
 * @param[in]  user_data  The user data passed
 *
 * @return     @c true to continue with the next iteration of the loop, \n
 *             otherwise @c false to break out of the loop
 *
 * @pre account_type_get_label(), account_type_query_label_by_app_id() must be called.
 *
 * @see account_type_get_label()
 * @see account_type_query_label_by_app_id()
 */
typedef bool (*account_label_cb)(char *app_id, char *label, char *locale, void *user_data);


/**
 * @brief  Called once for each capability of an account provider in the database.
 *
 * @since_tizen 3.0
 * @param[in]  app_id     The application ID
 * @param[in]  key        The user custom key
 * @param[in]  user_data  The user data passed
 *
 * @return     @c true to continue with the next iteration of the loop, \n
 *             otherwise @c false to break out of the loop
 *
 * @pre account_type_query_provider_feature_by_app_id(), account_type_get_provider_feature_all() must be called.
 *
 * @see account_type_query_provider_feature_by_app_id()
 * @see account_type_get_provider_feature_all()
 */
typedef bool (*provider_feature_cb)(char *app_id, char *key, void *user_data);


/**
 * @brief Called once when an event occurs.
 *
 * @since_tizen 3.0
 * @param[in]  event_type  The account event type
 * @param[in]  account_id  The account ID to update
 * @param[in]  user_data   The user data passed
 *
 * @return     @c true to continue with the next iteration of the loop, \n
 *             otherwise @c false to break out of the loop
 *
 * @pre account_subscribe_notification() must be called.
 *
 * @see account_subscribe_notification()
 */
typedef bool (*account_event_cb)(const char *event_type, int account_id, void *user_data);

/**
 * @brief  Creates a handle to the account.
 *
 * @since_tizen 3.0
 * @remarks    Release @a account using account_destroy().
 * @remarks    The created handle is not added to the account database until account_insert_to_db() is called.
 *
 * @param[in]  account  The account handle
 *
 * @return     @c 0 on success,
 *             otherwise a negative error value
 * @retval     #ACCOUNT_ERROR_NONE               Successful
 * @retval     #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval     #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_destroy()
 */
int account_create(account_h *account);


/**
 * @brief  Destroys the account handle and releases all its resources.
 *
 * @since_tizen 3.0
 * @param[in]  account  The account handle
 *
 * @return     @c 0 on success,
 *             otherwise a negative error value
 * @retval     #ACCOUNT_ERROR_NONE               Successful
 * @retval     #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_create()
 */
int account_destroy(account_h account);


/**
 * @brief  Inserts the account details to the account database.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read \n
 *              %http://tizen.org/privilege/account.write
 * @remarks     This API need both privileges
 * @param[in]   account        The account handle
 * @param[out]  account_db_id  The account ID to be assigned to an account
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER        Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY            Out of Memory
 * @retval  #ACCOUNT_ERROR_DB_FAILED                Database operation failed
 * @retval  #ACCOUNT_ERROR_DUPLICATED               Same user name exists in your application
 * @retval  #ACCOUNT_ERROR_NOT_ALLOW_MULTIPLE       Tried to add an account in spite of multiple false accounts
 * @retval  #ACCOUNT_ERROR_NOT_REGISTERED_PROVIDER  Tried to add an account though you did not register the account type in manifest
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED        DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY            SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED            Account database did not opened
 *
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
int account_insert_to_db(account_h account, int *account_db_id);


/**
 * @brief  Deletes an account from the account database by account DB ID.
 *
 * @since_tizen 3.0
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read \n
 *             %http://tizen.org/privilege/account.write
 * @remarks    This API need both privileges \n
 *             Only can delete an account which was added by same package applications
 * @param[in]  account_db_id  The account ID to delete
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  The account owner is different from the caller or DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
int account_delete_from_db_by_id(int account_db_id);


/**
 * @brief  Deletes an account from the account database by user name.
 *
 * @since_tizen 3.0
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read \n
 *             %http://tizen.org/privilege/account.write
 * @remarks    This API need both privileges \n
 *             Only can delete accounts which were added by same package applications
 * @param[in]  user_name     The user name of the account to delete
 * @param[in]  package_name  The package name of the account to delete
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
int account_delete_from_db_by_user_name(char *user_name, char *package_name);


/**
 * @brief  Deletes an account from the account database by package name.
 *
 * @since_tizen 3.0
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read \n
 *             %http://tizen.org/privilege/account.write
 * @remarks    This API need both privileges \n
 *             Only can delete accounts which was added by same package applications
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
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_update_to_db_by_id()
 * @see account_update_to_db_by_user_name()
 */
int account_delete_from_db_by_package_name(const char *package_name);

/**
 * @brief  Updates the account details to the account database.
 *
 * @since_tizen 3.0
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read \n
 *             %http://tizen.org/privilege/account.write
 * @remarks    This API need both privileges \n
 *             Only can update an account which was added by same package applications
 * @param[in]  account     The account handle
 * @param[in]  account_id  The account ID to update
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   The account to update does not exist
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_user_name()
 */
int account_update_to_db_by_id(account_h account, int account_id);

/**
 * @brief      Updates the account details to the account database.
 *             The provider permission check has been added since tizen 3.0.
 *
 * @since_tizen 3.0
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read \n
 *             %http://tizen.org/privilege/account.write
 * @remarks    This API need both privileges \n
 *             Only can update an account which was added by same package applications
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
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_user_name()
 */
int account_update_to_db_by_id_ex(account_h account, int account_id);

/**
 * @brief  Updates the account details to the account database.
 *
 * @since_tizen 3.0
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read \n
 *             %http://tizen.org/privilege/account.write
 * @remarks    This API need both privileges \n
 *             Only can update accounts which were added by same package applications
 * @param[in]  account       The account handle
 * @param[in]  user_name     The user name of the account to update
 * @param[in]  package_name  The package name for the user name
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   The account to update does not exist
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @see account_insert_to_db()
 * @see account_delete_from_db_by_id()
 * @see account_delete_from_db_by_user_name()
 * @see account_delete_from_db_by_package_name()
 * @see account_update_to_db_by_id()
 *
 */
int account_update_to_db_by_user_name(account_h account, const char *user_name, const char *package_name);

/**
 * @brief  Gets the ID of an account.
 *
 * @since_tizen 3.0
 * @param[in]   account     The account handle
 * @param[out]  account_id  The account ID
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 */
int account_get_account_id(account_h account, int *account_id);

/**
 * @brief  Gets the user name of an account.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a user_name using free().
 *
 * @param[in]   account    The account handle
 * @param[out]  user_name  The user name of the account
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_set_user_name()
 */
int account_get_user_name(account_h account, char **user_name);


/**
 * @brief  Sets the user name of an account.
 *
 * @since_tizen 3.0
 * @param[in]  account    The account handle
 * @param[in]  user_name  The string to set as user name
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_user_name()
 */
int account_set_user_name(account_h account, const char *user_name);


/**
 * @brief  Gets the display name of an account.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a display_name using free().
 *
 * @param[in]   account       The account handle
 * @param[out]  display_name  The display name of the account
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_get_display_name()
 */
int account_get_display_name(account_h account, char **display_name);


/**
 * @brief  Sets the display name of an account.
 *
 * @since_tizen 3.0
 * @param[in]  account      The account handle
 * @param[in]  display_name The text string to set as the display name
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_display_name()
 */
int account_set_display_name(account_h account, const char *display_name);

/**
 * @brief  Gets the capability detail of an account.
 *
 * @since_tizen 3.0
 * @param[in]   account           The account handle
 * @param[in]   capability_type   The capability type to get the capability value
 * @param[out]  capability_value  The capability value (on/off) of the specified capability_type
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   There is no given capability_type in the account
 *
 * @see account_set_capability()
 */
int account_get_capability(account_h account, const char *capability_type, account_capability_state_e *capability_value);

/**
 * @brief  Gets all the capabilities of an account.
 *
 * @since_tizen 3.0
 * @param[in]  account    The account handle
 * @param[in]  callback   The callback function
 * @param[in]  user_data  The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_set_capability()
 */
int account_get_capability_all(account_h account, capability_cb callback, void *user_data);

/**
 * @brief  Sets the capability.
 *
 * @since_tizen 3.0
 * @param[in]  account           The account handle
 * @param[in]  capability_type   The capability type
 * @param[in]  capability_state  The capability state
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_capability()
 */
int account_set_capability(account_h account, const char *capability_type, account_capability_state_e capability_state);


/**
 * @brief  Gets the icon path.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a icon_path using free().
 *
 * @param[in]   account    The account handle
 * @param[out]  icon_path  The icon path
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_set_icon_path()
 */
int account_get_icon_path(account_h account, char **icon_path);


/**
 * @brief  Sets the icon path.
 *
 * @since_tizen 3.0
 * @param[in]  account    The account handle
 * @param[in]  icon_path  The text string to set as the icon path
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_icon_path()
 */
int account_set_icon_path(account_h account, const char *icon_path);


/**
 * @brief  Gets the domain name.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a domain_name using free().
 *
 * @param[in]   account      The account handle
 * @param[out]  domain_name  The domain name
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_set_domain_name()
 */
int account_get_domain_name(account_h account, char **domain_name);


/**
 * @brief  Sets the domain name.
 *
 * @since_tizen 3.0
 * @param[in]  account      The account handle
 * @param[in]  domain_name  The text string to set as the domain name
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_domain_name()
 */
int account_set_domain_name(account_h account, const char *domain_name);


/**
 * @brief  Gets the email address.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a email_address using free().
 *
 * @param[in]   account        The account handle
 * @param[out]  email_address  The email address
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_set_email_address()
 */
int account_get_email_address(account_h account, char **email_address);


/**
 * @brief  Sets the email address.
 *
 * @since_tizen 3.0
 * @param[in]  account        The account handle
 * @param[in]  email_address  The text string to set as the email address
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_email_address()
 */
int account_set_email_address(account_h account, const char *email_address);


/**
 * @brief  Gets the package name.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a package_name using free().
 *
 * @param[in]   account       The account handle
 * @param[out]  package_name  The package name
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_set_package_name()
 */
int account_get_package_name(account_h account, char **package_name);


/**
 * @brief  Sets the package name.
 *
 * @since_tizen 3.0
 * @param[in]  account       The account handle
 * @param[in]  package_name  The text string to set as the package name
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_email_address()
 */
int account_set_package_name(account_h account, const char *package_name);


/**
 * @brief  Gets the access token. Access token field is used to store account secrets (such as password or master token).
 *
 * @since_tizen 3.0
 * @remarks    You must release @a access_token using free().
 *
 * @param[in]   account       The account handle
 * @param[out]  access_token  The access token
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @remarks Access token field is used for storing account secret (password / master token etc)
 * Only account owner application can retrieve account password / access_token. For others this field will be null.
 * @see account_set_access_token()
 */
int account_get_access_token(account_h account, char **access_token);


/**
 * @brief  Sets the access token. Access token field is used to store account secrets (such as password or master token).
 *
 * @since_tizen 3.0
 * @param[in]  account       The account handle
 * @param[in]  access_token  The text string to set as the access token
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @remarks Only account owner application can retrieve account password / access_token. For others this field will be null.
 * @see account_get_access_token()
 */
int account_set_access_token(account_h account, const char *access_token);


/**
 * @brief  Gets the user text.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a user_text using free().
 *
 * @param[in]   account          The account handle
 * @param[in]   user_text_index  The index of the user text (range: 0 ~ 4)
 * @param[out]  user_text        The user text
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_set_user_text()
 */
int account_get_user_text(account_h account, int user_text_index, char **user_text);


/**
 * @brief  Sets the user text.
 *
 * @since_tizen 3.0
 * @param[in]  account          The account handle
 * @param[in]  user_text_index  The index of the user text (must be in range from @c 0 to @c 4)
 * @param[in]  user_text        The text string to set as the user text
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_user_text()
 */
int account_set_user_text(account_h account, int user_text_index, const char *user_text);


/**
 * @brief  Gets the user integer.
 *
 * @since_tizen 3.0
 * @param[in]   account         The account handle
 * @param[in]   user_int_index  The index of the user integer (must be in range from @c 0 to @c 4)
 * @param[out]  user_integer    The user integer
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_set_user_int()
 */
int account_get_user_int(account_h account, int user_int_index, int *user_integer);


/**
 * @brief  Sets the user integer.
 *
 * @since_tizen 3.0
 * @param[in]  account         The account handle
 * @param[in]  user_int_index  The index of the user integer (must be in range from @c 0 to @c 4)
 * @param[in]  user_integer    The integer to set as the user integer
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_user_int()
 */
int account_set_user_int(account_h account, int user_int_index, int user_integer);


/**
 * @brief  Gets the auth type.
 *
 * @since_tizen 3.0
 * @param[in]   account    The account handle
 * @param[out]  auth_type  The auth type
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_set_auth_type()
 */
int account_get_auth_type(account_h account, account_auth_type_e *auth_type);


/**
 * @brief  Sets the auth type.
 *
 * @since_tizen 3.0
 * @param[in]  account    The account handle
 * @param[in]  auth_type  The integer to be set as the auth type
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_auth_type()
 */
int account_set_auth_type(account_h account, const account_auth_type_e auth_type);


/**
 * @brief  Gets the secret.
 *
 * @since_tizen 3.0
 * @param[in]   account  The account handle
 * @param[out]  secret   The secret
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_set_secret()
 */
int account_get_secret(account_h account, account_secrecy_state_e *secret);


/**
 * @brief  Sets the secret.
 *
 * @since_tizen 3.0
 * @param[in]  account  The account handle
 * @param[in]  secret   The secrecy to be set
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_secret()
 */
int account_set_secret(account_h account, const account_secrecy_state_e secret);

/**
 * @brief  Gets the sync support.
 *
 * @since_tizen 3.0
 * @param[in]   account       The account handle
 * @param[out]  sync_support  The sync support
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_set_sync_support()
 */
int account_get_sync_support(account_h account, account_sync_state_e *sync_support);


/**
 * @brief  Sets the sync support.
 *
 * @since_tizen 3.0
 * @param[in]  account       The account handle
 * @param[in]  sync_support  The sync state to be set
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_sync_support()
 */
int account_set_sync_support(account_h account, const account_sync_state_e sync_support);


/**
 * @brief  Gets the source.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a user_text using free().
 *
 * @param[in]   account  The account handle
 * @param[out]  source   The source
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_set_source()
 */
int account_get_source(account_h account, char **source);


/**
 * @brief  Sets the source.
 *
 * @since_tizen 3.0
 * @param[in]  account  The account handle
 * @param[in]  source   The text string to set as the source
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_source()
 */
int account_set_source(account_h account, const char *source);

/**
 * @brief  Sets the custom.
 *
 * @since_tizen 3.0
 * @param[in]  account  The account handle
 * @param[in]  key      The user custom key for the specific value
 * @param[in]  value    The user custom value about the given key
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_get_custom()
 */
int account_set_custom(account_h account, const char *key, const char *value);

/**
 * @brief  Gets the user specific custom text of an account key.
 *
 * @since_tizen 3.0
 * @param[in]   account   The account handle
 * @param[in]   key       The key to retrieve custom text
 * @param[out]  value     The text of the given key
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   There is no given capability type in the account
 *
 * @see account_set_custom()
 */
int account_get_custom(account_h account, const char *key, char **value);


/**
 * @brief  Gets all the user custom texts of an account.
 *
 * @since_tizen 3.0
 * @param[in]  account    The account handle
 * @param[in]  callback   The callback function to retrieve all custom text \n
 *                        The callback function gives the key and value.
 * @param[in]  user_data  The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_set_custom()
 */
int account_get_custom_all(account_h account, account_custom_cb callback, void *user_data);

/**
 * @brief  Retrieves all accounts details by invoking the given callback function iteratively.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback   The callback function to invoke
 * @param[in]   user_data  The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   Related record does not exist
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @post  This function invokes account_cb().
 *
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 * @see account_query_account_by_capability()
 */
int account_foreach_account_from_db(account_cb callback, void *user_data);


/**
 * @brief  Retrieve an account with the account ID.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   account_db_id  The account database ID to search
 * @param[out]  account        The account handle \n
 *                             Must be allocated by account_create() and freed after using by account_destroy().
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 * @see account_query_account_by_capability()
 */
int account_query_account_by_account_id(int account_db_id, account_h *account);

/**
 * @brief  Retrieves all accounts with the user name.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback   The callback function to invoke
 * @param[in]   user_name  The user name to search
 * @param[in]   user_data  The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @post  This function invokes account_cb().
 *
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_package_name()
 * @see account_query_account_by_capability()
 *
 */
int account_query_account_by_user_name(account_cb callback, const char *user_name, void *user_data);

/**
 * @brief  Retrieves all accounts with the package name.
 *
 * @since_tizen 3.0
 * @privlevel  public
 * @privilege  %http://tizen.org/privilege/account.read
 * @param[in]  callback      The callback function to invoke
 * @param[in]  package_name  The package name to search
 * @param[in]  user_data     The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @post  This function invokes account_cb().
 *
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_capability()
 */
int account_query_account_by_package_name(account_cb callback, const char *package_name, void *user_data);

/**
 * @brief  Retrieves all accounts with the capability type and capability value.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback          The callback function to invoke
 * @param[in]   capability_type   The capability type to search
 * @param[in]   capability_value  The capability value to search
 * @param[in]   user_data         The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @post  This function invokes account_cb().
 *
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 */
int account_query_account_by_capability(account_cb callback, const char *capability_type, account_capability_state_e capability_value, void *user_data);

/**
 * @brief  Retrieves all accounts with the capability type.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback         The callback function to invoke
 * @param[in]   capability_type  The capability type to search
 * @param[in]   user_data        The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @post  This function invokes account_cb().
 *
 * @see account_foreach_account_from_db()
 * @see account_query_account_by_account_id()
 * @see account_query_account_by_user_name()
 * @see account_query_account_by_package_name()
 */
int account_query_account_by_capability_type(account_cb callback, const char *capability_type, void *user_data);

/**
 * @brief  Retrieves all capabilities with the account database ID.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback       The callback function to invoke
 * @param[in]   account_db_id  The account database ID to search
 * @param[in]   user_data      The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @post  This function invokes capability_cb().
 *
 * @see account_get_capability()
 * @see account_set_capability()
 */
int account_query_capability_by_account_id(capability_cb callback, int account_db_id, void *user_data);


/**
 * @brief  Gets the count of accounts in the account database.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[out]  count  The out parameter for count of all accounts
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 */
int account_get_total_count_from_db(int *count);

/**
 * @brief  Updates the sync status of an account with the given account ID.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read \n
 *              %http://tizen.org/privilege/account.write
 * @remarks     This API need both privileges \n
 *              Only can update an account which was added by same package applications
 * @param[in]   account_db_id  The account ID for which sync status needs to be changed
 * @param[in]   sync_status    The new sync status
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 */
int account_update_sync_status_by_id(int account_db_id, const account_sync_state_e sync_status);


/* Account type API */


/**
 * @brief  Creates a handle to the account provider.
 *
 * @since_tizen 3.0
 * @remarks     You must release @a account_type handle using account_type_destroy().\n
 *
 * @param[in]  account_type  The account provider handle
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_type_destroy()
 */
int account_type_create(account_type_h *account_type);

/**
 * @brief  Destroys the account provider handle and releases all its resources.
 *
 * @since_tizen 3.0
 *
 * @remarks    When you get @a account_type_h using account_type_create(), you must release the handle using account_destroy() to avoid the memory leak.
 *
 * @param[in]  account_type  The account provider handle
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_type_create()
 */
int account_type_destroy(account_type_h account_type);

/**
 * @brief  Retrieves capability information with your application ID.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback   The callback function carries the capability name of an app ID
 * @param[in]   app_id     The application ID to search
 * @param[in]   user_data  The user data \n
 *                         If you have your private data to carry into callback function, then you can use it.
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
 */
int account_type_query_provider_feature_by_app_id(provider_feature_cb callback, const char *app_id, void *user_data);

/**
 * @brief   Checks whether the given application ID supports the capability.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @remarks     The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in]   app_id      The application ID
 * @param[in]   capability  The capability \n
 *                          For example, ACCOUNT_SUPPORTS_CAPABILITY_CONTACT or "http://tizen.org/account/capability/contact"
 *
 * @return  @c TRUE if the application supports the given capability, \n
 *          otherwise @c FALSE if the application does not support the given capability
 * @retval  @c TRUE means the application supports the given capability
 * @retval  @c FALSE means the application does not support the given capability
 * @exception #ACCOUNT_ERROR_NONE               Successful
 * @exception #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @exception #ACCOUNT_ERROR_RECORD_NOT_FOUND   Related record does not exist
 * @excaption #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @exception #ACCOUNT_ERROR_PERMISSION_DENIED  DB access fail by permission
 * @excaption #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @excaption #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 */
bool account_type_query_supported_feature(const char *app_id, const char *capability);

/**
 * @brief  Gets the application ID of an account provider.
 *
 * @since_tizen 3.0
 * @remarks     You must release @a app_id using free().
 *
 * @param[in]   account_type  The account provider handle \n
 *                            It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db().
 * @param[out]  app_id        The application ID of an account provider item
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 */
int account_type_get_app_id(account_type_h account_type, char **app_id);

/**
 * @brief  Gets the service provider ID of an account provider.
 *
 * @since_tizen 3.0
 * @remarks     You must release @a service_provider_id using free().
 *
 * @param[in]   account_type         The account provider handle \n
 *                                   It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db().
 * @param[out]  service_provider_id  The service provider text ID of an account provider item
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 */
int account_type_get_service_provider_id(account_type_h account_type, char **service_provider_id);

/**
 * @brief  Gets the icon path of an account provider.
 *
 * @since_tizen 3.0
 * @remarks     You must release @a icon_path using free().
 *
 * @param[in]   account_type  The account provider handle \n
 *                            It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db().
 * @param[out]  icon_path     The icon path of the account provider item
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 */
int account_type_get_icon_path(account_type_h account_type, char **icon_path);

/**
 * @brief  Gets the small icon path of an account provider.
 *
 * @since_tizen 3.0
 * @remarks    You must release @a small_icon_path using free().
 *
 * @param[in]   account_type     The account provider handle\n
 *                               It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db().
 * @param[out]  small_icon_path  The small icon path of the account provider item
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 */
int account_type_get_small_icon_path(account_type_h account_type, char **small_icon_path);

/**
 * @brief  Checks whether the given account provider supports multiple accounts.
 *
 * @since_tizen 3.0
 * @param[in]   account_type              The account provider handle \n
 *                                        It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db.
 * @param[out]  multiple_account_support  The flag indicating support for multiple accounts accounts\n
 *                                        TRUE or FALSE.
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of memory
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 */
int account_type_get_multiple_account_support(account_type_h account_type, int *multiple_account_support);

/**
 * @brief  Gets capability information with the given account provider handle.
 *
 * @since_tizen 3.0
 * @param[in]   account_type  The account provider handle\n
 *                            It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db().
 * @param[in]   callback      The callback function that carries the capability name of the app ID
 * @param[in]   user_data     The user data \n
 *                            If you have your private data to carry into callback function, then you can use it.
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 *
 */
int account_type_get_provider_feature_all(account_type_h account_type, provider_feature_cb callback, void *user_data);

/**
 * @brief  Gets the specific label information detail of an account provider.
 *
 * @since_tizen 3.0
 * @param[in]   account_type The account provider handle\n
 *                           It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db().
 * @param[in]   locale       The locale is specified as an ISO 3166 alpha-2 two letter country-code followed by ISO 639-1 for the two-letter language code.\n
 *                           For example, "ko_KR" or "ko-kr" for Korean, "en_US" or "en-us" for American English.
 * @param[out]  label        The label text given for the locale
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   No label for the given locale
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 */

int account_type_get_label_by_locale(account_type_h account_type, const char *locale, char **label);

/**
 * @brief  Gets the label information detail of an account provider.
 *
 * @since_tizen 3.0
 * @param[in]   account_type  The account provider handle\n
 *                            It should be given by account_type_query_* functions or account_type_foreach_account_type_from_db().
 * @param[in]   callback      The callback function carrying the label information
 * @param[in]   user_data     The user data to be passed to the callback function
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_type_foreach_account_type_from_db()
 * @see account_type_query_by_app_id()
 */
int account_type_get_label(account_type_h account_type, account_label_cb callback, void *user_data);

/**
 * @brief  Retrieves the label information with your application ID.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback    The callback function that carries label_h for label information \n
 *                         label_h contains label info as parameter.
 * @param[in]   app_id     The application ID to search
 * @param[in]   user_data  The user data \n
 *                         If you have your private data to carry into callback function, then you can use it.
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
 * @see account_type_query_by_app_id()
 * @see account_type_foreach_account_type_from_db()
 */
int account_type_query_label_by_app_id(account_label_cb callback, const char *app_id, void *user_data);

/**
 * @brief  Retrieves the account provider information with your application ID.
 *
 * @since_tizen 3.0
 * @privlevel     public
 * @privilege     %http://tizen.org/privilege/account.read
 * @param[in]     app_id        The application ID to search
 * @param[in/out] account_type  The account handle which has to be created by account_type_create() before calling this function and released by account_type_destroy() after calling this function.

 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   Queried data does not exist
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @see account_type_create()
 * @see account_type_get_app_id()
 * @see account_type_get_service_provider_id()
 * @see account_type_get_icon_path()
 * @see account_type_get_small_icon_path()
 * @see account_type_get_multiple_account_support()
 * @see account_type_get_label()
 * @see account_type_destroy()
 */
int account_type_query_by_app_id(const char *app_id, account_type_h *account_type);

/**
 * @brief  Retrieves all account priovider information.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback   The account provider information \n
 *                         You can get the account information through account_type_get_* with the carried account_type_handle.
 * @param[in]   user_data  The user data \n
 *                         It will be carried through your callback function.
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
 * @see account_type_create()
 * @see account_type_get_app_id()
 * @see account_type_get_service_provider_id()
 * @see account_type_get_icon_path()
 * @see account_type_get_small_icon_path()
 * @see account_type_get_multiple_account_support()
 * @see account_type_get_label()
 * @see account_type_destroy()
 */
int account_type_foreach_account_type_from_db(account_type_cb callback, void *user_data);

/**
 * @brief  Retrieves the label information with the given application ID and locale.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   app_id  The application ID
 * @param[in]   locale  The locale is specified as an ISO 3166 alpha-2 two letter country-code followed by ISO 639-1 for the two-letter language code.\n
 *                      For example, "ko_KR" or "ko-kr" for Korean, "en_US" or "en-us" for American English.
 * @param[out]  label   The label text corresponding app_id and locale \n
 *                      It must be free text.
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
 */
int account_type_query_label_by_locale(const char *app_id, const char *locale, char **label);

/**
 * @brief  Retrieves account provider information with the capability name.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   callback        The callback function to retrieve account provider information
 * @param[in]   key             The capability value to search account provider \n
 *                              For example, ACCOUNT_SUPPORTS_CAPABILITY_CONTACT or "http://tizen.org/account/capability/contact"
 * @param[in]  user_data        If you have your private data to carry into callback function, then you can use it
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   Record not found
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 */
int account_type_query_by_provider_feature(account_type_cb callback, const char *key, void *user_data);

/**
 * @brief  Checks whether the given app_id exists in the account provider DB.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[in]   app_id  The application ID to check
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_RECORD_NOT_FOUND   Record not found
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid app ID
 * @retval  #ACCOUNT_ERROR_DB_FAILED          Database operation failed
 * @retval  #ACCOUNT_ERROR_PERMISSION_DENIED  DB Access fail by permission
 * @retval  #ACCOUNT_ERROR_DATABASE_BUSY      SQLite handler is busy
 * @retval  #ACCOUNT_ERROR_DB_NOT_OPENED      Account database did not opened
 *
 * @see account_type_query_by_app_id()
 */
int account_type_query_app_id_exist(const char *app_id);



/* End of account provider API  */

/**
 * @brief  Creates a handle for the account event subscription.
 *
 * @since_tizen 3.0
 * @remarks     You must release @a account_subscribe handle using account_unsubscribe_notification().
 *
 * @param[in]   account_subscribe  The account subscription handle
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE               Successful
 * @retval  #ACCOUNT_ERROR_OUT_OF_MEMORY      Out of Memory
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see account_unsubscribe_notification()
 * @see account_subscribe_notification()
 */
int account_subscribe_create(account_subscribe_h *account_subscribe);

/**
 * @brief  Starts to subscribe account event through the given callback function.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read \n
 * @param[in]   account_subscribe  The account subscription handle
 * @param[in]   callback           The callback function that is called when an account is removed and a data of account is updated from the account database \n
 *                                 It will be called with event message and account ID.
 * @param[in]   user_data          The user_data that is delivered to callback
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE                     Successful
 * @retval  #ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL  Subscription fail
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER        Invalid parameter
 *
 * @pre account_subscribe_create()
 *
 * @see account_unsubscribe_notification()
 * @see account_subscribe_notification()
 */
int account_subscribe_notification(account_subscribe_h account_subscribe, account_event_cb callback, void *user_data);

/**
 * @brief  Destroys the account subscribe handle and releases all its resources.
 *
 * @since_tizen 3.0
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read \n
 * @remarks     You must call @a account_unsubscribe_notification when you do not need to subscribe account event.
 *
 * @param[in]   account_subscribe  The account subscription handle
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE                     Successful
 * @retval  #ACCOUNT_ERROR_INVALID_PARAMETER        Invalid parameter
 * @retval  #ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL  Unsubscription failed
 *
 * @see account_create()
 */
int account_unsubscribe_notification(account_subscribe_h account_subscribe);

#ifdef __cplusplus
}
#endif


#endif /* __ACCOUNT_H_ */
