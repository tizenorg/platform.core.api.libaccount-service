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

#ifndef __ACCOUNT_PRODUCT_H__
#define __ACCOUNT_PRODUCT_H__

#include <account.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @internal
 * @addtogroup CAPI_ACCOUNT_MANAGER_MODULE
 * @{
 */

/**
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

/**
 * @brief  Gets the count of accounts whose secrect state is visible in the account database.
 * @since_tizen 2.3
 *
 * @privlevel   public
 * @privilege   %http://tizen.org/privilege/account.read
 * @param[out]  count  The out parameter for count of all accounts
 *
 * @return  @c 0 on success,
 *          otherwise a negative error value
 * @retval  #ACCOUNT_ERROR_NONE       Successful
 * @retval  #ACCOUNT_ERROR_DB_FAILED  Database operation failed
 * @retval  #ACCOUNT_ERROR_ACCESS_DENIED DB Access fail by permission
 *
 * @pre  This function requires an open connection to an account service by account_connect().
 *
 * @see account_connect()
 */
ACCOUNT_API int account_get_total_count_from_db_ex(int *count);

/**
* @}
*/

#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_PRODUCT_H__ */