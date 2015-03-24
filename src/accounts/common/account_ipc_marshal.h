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

#ifndef __ACC_IPC_MARSHAL_H__
#define __ACC_IPC_MARSHAL_H__

#include "account-private.h"

ACCOUNT_API GVariant* marshal_account(const account_s* account);
ACCOUNT_API account_s* umarshal_account(GVariant* in_data);

ACCOUNT_API GVariant* marshal_account_type(const account_type_s* account_type);
ACCOUNT_API account_type_s* umarshal_account_type(GVariant* in_data);

ACCOUNT_API GVariant* marshal_account_list_double(GList* account_list);
ACCOUNT_API GVariant* marshal_account_list(GSList* account_list);
ACCOUNT_API GSList* unmarshal_account_list(GVariant* variant);

ACCOUNT_API GVariant* marshal_account_type_list(GSList* account_type_list);
ACCOUNT_API GSList* unmarshal_account_type_list(GVariant* variant);

ACCOUNT_API GVariant* marshal_capability_list(GSList* capability_list);
ACCOUNT_API GSList* unmarshal_capability_list(GVariant* variant);

ACCOUNT_API GVariant* marshal_custom_list(GSList* capability_list);
ACCOUNT_API GSList* unmarshal_custom_list(GVariant* variant);

ACCOUNT_API GVariant* marshal_user_int_array(const int* user_data_int_array);
ACCOUNT_API GVariant* marshal_user_txt_array(char* const* user_txt_array);

ACCOUNT_API int* unmarshal_user_int_array(GVariant* variant);
ACCOUNT_API char** unmarshal_user_txt_array(GVariant* variant);

ACCOUNT_API account_s* create_empty_account_instance(void);

ACCOUNT_API GVariant * provider_feature_list_to_variant (GSList *list);
ACCOUNT_API GSList* variant_to_provider_feature_list(GVariant *variant);

ACCOUNT_API GVariant *label_list_to_variant (GSList *list);
ACCOUNT_API GSList *variant_to_label_list (GVariant *variant);

ACCOUNT_API account_type_s* create_empty_account_type_instance(void);

#endif
