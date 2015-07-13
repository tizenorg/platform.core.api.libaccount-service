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

#ifndef __ACCOUNT_PRIVATE_CLIENT_H__
#define __ACCOUNT_PRIVATE_CLIENT_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include "account-types.h"

#ifdef TIZEN_PROFILE_MOBILE
#include "mobile/account.h"
#else
#include "wearable/account.h"
#endif

typedef struct _account_subscribe_s
{
	    account_event_cb account_subscription_callback;
	    void* user_data;
}account_subscribe_s;

#ifdef __cplusplus
}
#endif

#endif /* __ACCOUNT_PRIVATE_CLIENT_H__*/
