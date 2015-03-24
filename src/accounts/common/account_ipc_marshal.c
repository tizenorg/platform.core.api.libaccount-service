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

#include <stdio.h>	//snprintf
#include <stdlib.h> //calloc
#include <string.h>
#include "account_ipc_marshal.h"

#include "dbg.h"

#define ACCOUNT_DATA_KEY_ID "id"
#define ACCOUNT_DATA_KEY_USER_NAME "user_name"
#define ACCOUNT_DATA_KEY_EMAIL "email_address"
#define ACCOUNT_DATA_KEY_DISPLAY_NAME "display_name"
#define ACCOUNT_DATA_KEY_ICON_PATH "icon_path"
#define ACCOUNT_DATA_KEY_SOURCE "source"
#define ACCOUNT_DATA_KEY_PACKAGE_NAME "package_name"
#define ACCOUNT_DATA_KEY_ACCESS_TOKEN "access_token"
#define ACCOUNT_DATA_KEY_DOMAIN_NAME "domain_name"
#define ACCOUNT_DATA_KEY_AUTH_TYPE "auth_type"
#define ACCOUNT_DATA_KEY_SECRET "secret"
#define ACCOUNT_DATA_KEY_SYNC_SUPPORT "sync_support"
#define ACCOUNT_DATA_KEY_USER_DATA_INT "user_data_int"
#define ACCOUNT_DATA_KEY_USER_DATA_TXT "user_data_txt"
#define ACCOUNT_DATA_KEY_CAPABILITY_LIST "capability_list"
#define ACCOUNT_DATA_KEY_ACCOUNT_LIST "account_list"
#define ACCOUNT_DATA_KEY_CUSTOM_LIST "custom_list"

#define ACCOUNT_TYPE_DATA_KEY_ID "id"
#define ACCOUNT_TYPE_DATA_KEY_APP_ID "app_id"
#define ACCOUNT_TYPE_DATA_KEY_SERVICE_PROVIDER_ID "service_provider_id"
#define ACCOUNT_TYPE_DATA_KEY_ICON_PATH "icon_path"
#define ACCOUNT_TYPE_DATA_KEY_SMALL_ICON_PATH "small_icon_path"
#define ACCOUNT_TYPE_DATA_KEY_MULTI_SUPPORT "multiple_account_support"
#define ACCOUNT_TYPE_DATA_KEY_LABEL_LIST "label_list"
#define ACCOUNT_TYPE_DATA_KEY_ACC_TYPE_LIST "account_type_list"
#define ACCOUNT_TYPE_DATA_KEY_PROVIDER_FEATURE_LIST "provider_feature_list"

#define ACCOUNT_CAPABILITY_DATA_KEY_ID "id"
#define ACCOUNT_CAPABILITY_DATA_KEY_TYPE "type"
#define ACCOUNT_CAPABILITY_DATA_KEY_VALUE "value"
#define ACCOUNT_CAPABILITY_DATA_KEY_PACKAGE_NAME "package_name"
#define ACCOUNT_CAPABILITY_DATA_KEY_USER_NAME "user_name"
#define ACCOUNT_CAPABILITY_DATA_KEY_ACC_ID "account_id"

#define ACCOUNT_CUSTOM_DATA_KEY_ACC_ID "account_id"
#define ACCOUNT_CUSTOM_DATA_KEY_APP_ID "app_id"
#define ACCOUNT_CUSTOM_DATA_KEY_KEY "key"
#define ACCOUNT_CUSTOM_DATA_KEY_VALUE "value"

static label_s*
_variant_to_label(GVariant *variant)
{
	gchar *app_id = NULL;
	gchar *label = NULL;
	gchar *locale = NULL;

	g_return_val_if_fail (variant != NULL, NULL);

	g_variant_get (variant, "(sss)", &app_id, &label, &locale);

	label_s* label_data = (label_s*) calloc(1, sizeof(label_s));
	label_data->app_id = g_strdup(app_id);
	label_data->label = g_strdup(label);
	label_data->locale = g_strdup(locale);

	g_free (app_id);
	g_free (label);
	g_free (locale);

	return label_data;
}

GSList*
variant_to_label_list(GVariant *variant)
{
	GSList *list = NULL;
	GVariantIter iter;
	GVariant *value;

	g_return_val_if_fail (variant != NULL, NULL);

	g_variant_iter_init (&iter, variant);
	while ((value = g_variant_iter_next_value (&iter))) {
		list = g_slist_append (list,
							  _variant_to_label(value));
		g_variant_unref (value);
	}

	return list;
}


static GVariant *
_label_to_variant (label_s *label_data)
{
	GVariant *variant;

	g_return_val_if_fail (label_data != NULL, NULL);

	variant = g_variant_new ("(sss)",
							 label_data->app_id ? label_data->app_id : "",
							 label_data->label ? label_data->label : "",
							 label_data->locale ? label_data->locale : "");

	return variant;
}

GVariant *
label_list_to_variant (GSList *list)
{
	GVariantBuilder builder;
	GVariant *variant;
	label_s *label_data;

	GSList* iter;
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);
	for (iter = list ; iter != NULL; iter = g_slist_next (iter))
	{
		label_data = (label_s*) (iter->data);
		g_variant_builder_add_value (
									&builder,
									_label_to_variant(label_data));
	}
	variant = g_variant_builder_end (&builder);

	return variant;
}


static provider_feature_s*
_variant_to_provider_feature(GVariant *variant)
{
	gchar *key = NULL;
	gchar *app_id = NULL;

	g_return_val_if_fail (variant != NULL, NULL);

	g_variant_get (variant, "(ss)", &key, &app_id);

	provider_feature_s* provider_feature_data = (provider_feature_s*) calloc(1, sizeof(provider_feature_s));
	provider_feature_data->key = g_strdup(key);
	provider_feature_data->app_id = g_strdup(app_id);

	g_free (key);
	g_free (app_id);

	return provider_feature_data;
}

GSList*
variant_to_provider_feature_list(GVariant *variant)
{
	GSList *list = NULL;
	GVariantIter iter;
	GVariant *value;

	g_return_val_if_fail (variant != NULL, NULL);

	g_variant_iter_init (&iter, variant);
	while ((value = g_variant_iter_next_value (&iter))) {
		list = g_slist_append (list,
							  _variant_to_provider_feature(value));
		g_variant_unref (value);
	}

	return list;
}

static GVariant *
_provider_feature_to_variant (const provider_feature_s *pro_feature_data)
{
	GVariant *variant;

	g_return_val_if_fail (pro_feature_data != NULL, NULL);

	variant = g_variant_new ("(ss)",
							 pro_feature_data->key ? pro_feature_data->key : "",
							 pro_feature_data->app_id ? pro_feature_data->app_id : "");

	return variant;
}

GVariant *
provider_feature_list_to_variant (GSList *list)
{
	GVariantBuilder builder;
	GVariant *variant;
	provider_feature_s *provider_feature_data;

	GSList* iter;
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);
	for (iter = list ; iter != NULL; iter = g_slist_next (iter)) {
		provider_feature_data = (provider_feature_s *) (iter->data);
		g_variant_builder_add_value (
									&builder,
									_provider_feature_to_variant(provider_feature_data));
	}
	variant = g_variant_builder_end (&builder);

	return variant;
}

GVariant *
marshal_account(const account_s* account)
{
	_INFO("_marshal_account start");

	const account_s* in_data = account;
	GVariantBuilder builder;
	int i;

	g_variant_builder_init (&builder, G_VARIANT_TYPE_VARDICT);

	g_variant_builder_add (&builder, "{sv}",
						   ACCOUNT_DATA_KEY_ID,
						   g_variant_new_int32 (in_data->id));

	_INFO("in_data->id=[%d]", in_data->id);

	if (in_data->user_name != NULL)
	{
		_INFO("user name");

		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_USER_NAME,
							   g_variant_new_string (in_data->user_name));
	}
	if (in_data->email_address != NULL)
	{
		_INFO("email");

		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_EMAIL,
							   g_variant_new_string (in_data->email_address));
	}
	if (in_data->display_name != NULL)
	{
		_INFO("user display name");

		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_DISPLAY_NAME,
							   g_variant_new_string (in_data->display_name));
	}
	if (in_data->icon_path != NULL)
	{
		_INFO("icon path");

		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_ICON_PATH,
							   g_variant_new_string (in_data->icon_path));
	}
	if (in_data->source != NULL)
	{
		_INFO("source");

		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_SOURCE,
							   g_variant_new_string (in_data->source));
	}
	if (in_data->package_name != NULL)
	{
		_INFO("pkg name");
		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_PACKAGE_NAME,
							   g_variant_new_string (in_data->package_name));
	}
	if (in_data->access_token != NULL)
	{
		_INFO("acc token");
		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_ACCESS_TOKEN,
							   g_variant_new_string (in_data->access_token));
	}
	if (in_data->domain_name != NULL)
	{
		_INFO("domain");

		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_DOMAIN_NAME,
							   g_variant_new_string (in_data->domain_name));
	}

	_INFO("auth_type = %d", in_data->auth_type);
	g_variant_builder_add (&builder, "{sv}", ACCOUNT_DATA_KEY_AUTH_TYPE, g_variant_new_int32 (in_data->auth_type));

	_INFO("secret = %d", in_data->secret);
	g_variant_builder_add (&builder, "{sv}", ACCOUNT_DATA_KEY_SECRET, g_variant_new_int32 (in_data->secret));

	_INFO("sync_support = %d", in_data->sync_support);
	g_variant_builder_add (&builder, "{sv}", ACCOUNT_DATA_KEY_SYNC_SUPPORT, g_variant_new_int32 (in_data->sync_support));

	_INFO("user_data_int");
	for(i=0; i<USER_INT_CNT; i++)
	{
		g_variant_builder_add (&builder, "{sv}",
				ACCOUNT_DATA_KEY_USER_DATA_INT,
				marshal_user_int_array((const int*) in_data->user_data_int));
	}

	_INFO("user_data_txt");
	for(i=0; i<USER_TXT_CNT; i++)
	{
		g_variant_builder_add (&builder, "{sv}",
				ACCOUNT_DATA_KEY_USER_DATA_TXT,
				marshal_user_txt_array((char* const*)in_data->user_data_txt));
	}

	if (in_data->capablity_list != NULL)
	{
		_INFO("capablity_list");

		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_CAPABILITY_LIST,
							   marshal_capability_list (in_data->capablity_list));
	}

	if (in_data->custom_list != NULL)
	{
		_INFO("custom_list");

		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_DATA_KEY_CUSTOM_LIST,
							   marshal_custom_list (in_data->custom_list));
	}

	//TODO: add support for rest

	_INFO("__marshal_account end");

	return g_variant_builder_end (&builder);
}

account_s* umarshal_account(GVariant* in_data)
{
	_INFO("");

	if (in_data == NULL)
	{
		_ERR("Null input");
		return NULL;
	}

	GVariant* temp_var = in_data;
	gchar* print_type = NULL;
	print_type = g_variant_print(temp_var, TRUE);
	if (print_type == NULL)
	{
		_ERR("Invalid input");
		return NULL;
	}
	_INFO("input=%s", print_type);

	account_s* account = (account_s*)calloc(1, sizeof(account_s));
	if (account == NULL)
	{
		_ERR("Out of memory");
		return NULL;
	}

	_INFO("");

	GVariantIter iter;
	gchar *key = NULL;
	GVariant *value = NULL;

	_INFO("");
	g_variant_iter_init (&iter, in_data);
	_INFO("");
	while (g_variant_iter_next (&iter, "{sv}", &key, &value))
	{
		_INFO("[%s]", key);

		if (!strcmp(key, ACCOUNT_DATA_KEY_ID))
		{
			_INFO("");

			account->id = g_variant_get_int32 (value);
			_INFO("id = %d", account->id);
		}

		if (!strcmp(key, ACCOUNT_DATA_KEY_USER_NAME))
		{
			_INFO("");
			account->user_name = g_strdup(g_variant_get_string (value, NULL));
			_INFO("unmarshaled user name=[%s]", account->user_name);
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_EMAIL))
		{
			_INFO("");
			account->email_address = g_strdup(g_variant_get_string (value, NULL));
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_DISPLAY_NAME))
		{
			_INFO("");

			account->display_name = g_strdup(g_variant_get_string (value, NULL));
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_ICON_PATH))
		{
			_INFO("");

			account->icon_path = g_strdup(g_variant_get_string (value, NULL));
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_SOURCE))
		{
			_INFO("");

			account->source = g_strdup(g_variant_get_string (value, NULL));
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_PACKAGE_NAME))
		{
			_INFO("");

			account->package_name = g_strdup(g_variant_get_string (value, NULL));
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_ACCESS_TOKEN))
		{
			_INFO("");

			account->access_token = g_strdup(g_variant_get_string (value, NULL));
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_DOMAIN_NAME))
		{
			_INFO("");

			account->domain_name = g_strdup(g_variant_get_string (value, NULL));
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_AUTH_TYPE))
		{
			_INFO("");

			account->auth_type = g_variant_get_int32 (value);
			_INFO("auth_type = %d", account->auth_type);
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_SECRET))
		{
			_INFO("");

			account->secret = g_variant_get_int32 (value);
			_INFO("secret = %d", account->secret);
		}

		else if (!strcmp(key, ACCOUNT_DATA_KEY_SYNC_SUPPORT))
		{
			account->sync_support = g_variant_get_int32 (value);
			_INFO("sync_support = %d", account->sync_support);
		}
		else if (!strcmp(key, ACCOUNT_DATA_KEY_USER_DATA_INT))
		{
			int i;
			int* tmp_user_ints = unmarshal_user_int_array(value);
			for(i=0; i<USER_INT_CNT; i++)
			{
				account->user_data_int[i] = tmp_user_ints[i];
			}
			_ACCOUNT_FREE(tmp_user_ints);
		}
		else if (!strcmp(key, ACCOUNT_DATA_KEY_USER_DATA_TXT))
		{
			int i;
			char** tmp_user_txts = unmarshal_user_txt_array(value);
			for(i=0; i<USER_TXT_CNT; i++)
			{
				account->user_data_txt[i] = strdup(tmp_user_txts[i]);
				_ACCOUNT_FREE(tmp_user_txts[i]);
			}
			_ACCOUNT_FREE(tmp_user_txts);
		}
		else if (!strcmp(key, ACCOUNT_DATA_KEY_CAPABILITY_LIST))
		{
			account->capablity_list = unmarshal_capability_list (value);
			_INFO("unmarshalled capablity_list");
		}
		else if (!strcmp(key, ACCOUNT_DATA_KEY_CUSTOM_LIST))
		{
			account->custom_list = unmarshal_custom_list (value);
			_INFO("unmarshalled custom_list");
		}
		//TODO: support for rest
	}

	return account;
}

GVariant* marshal_account_list_double(GList* account_list)
{
	_INFO("marshal_account_list start");
	if (account_list == NULL)
	{
		_ERR("input NULL.");
		return NULL;
	}

	GVariantBuilder builder;
	account_s *account_data;

	GList* iter;
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

	for (iter = account_list ; iter != NULL; iter = g_list_next (iter))
	{
		_INFO("iterating account_list");
		account_data = (account_s *) (iter->data);
		g_variant_builder_add_value(&builder, marshal_account(account_data));
		_INFO("end one iteration of account_list");
	}
	_INFO("marshal_account_list end");
	return g_variant_builder_end (&builder);
}

GVariant* marshal_account_list(GSList* account_list)
{
	_INFO("marshal_account_list start");
	if (account_list == NULL)
	{
		_ERR("input NULL.");
		return NULL;
	}

	GVariantBuilder builder;
	account_s *account_data;

	GSList* iter;
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

	for (iter = account_list ; iter != NULL; iter = g_slist_next (iter))
	{
		_INFO("iterating account_list");
		account_data = (account_s *) (iter->data);
		g_variant_builder_add_value(&builder, marshal_account(account_data));
		_INFO("end one iteration of account_list");
	}
	_INFO("marshal_account_list end");
	return g_variant_builder_end (&builder);
}

GSList* unmarshal_account_list(GVariant* variant)
{
	_INFO("");
	GSList *list = NULL;
	GVariantIter iter;
	GVariantIter* iter_row = NULL;
	const gchar *key = NULL;
	GVariant *value = NULL;

	if (variant == NULL)
	{
		_ERR("input NULL");
		return NULL;
	}

	_INFO ("type %s", g_variant_get_type_string (variant));
	gchar* var_type = g_variant_print (variant, TRUE);
	_INFO("var_type = %s", var_type);

	_INFO("before iter");
	g_variant_iter_init (&iter, variant);
	_INFO("after iter");

	_INFO("g_variant_iter_n_children=%d", g_variant_iter_n_children (&iter));

	while (g_variant_iter_next (&iter, "a{sv}", &iter_row))
	{
		_INFO("");
		account_s* account = (account_s*)calloc(1, sizeof(account_s));

		while (g_variant_iter_loop(iter_row, "{sv}", &key, &value))
		{
			if (!g_strcmp0(key, ACCOUNT_DATA_KEY_ID))
			{
				_INFO("");

				account->id = g_variant_get_int32 (value);
				_INFO("id = %d", account->id);
			}

			if (!g_strcmp0(key, ACCOUNT_DATA_KEY_USER_NAME))
			{
				account->user_name = g_strdup(g_variant_get_string(value, NULL));
				_INFO("[%s]", account->user_name);
			}
			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_EMAIL))
			{
				_INFO("");
				account->email_address = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_DISPLAY_NAME))
			{
				_INFO("");

				account->display_name = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_ICON_PATH))
			{
				_INFO("");

				account->icon_path = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_SOURCE))
			{
				_INFO("");

				account->source = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_PACKAGE_NAME))
			{
				_INFO("");

				account->package_name = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_ACCESS_TOKEN))
			{
				_INFO("");

				account->access_token = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_DOMAIN_NAME))
			{
				_INFO("");

				account->domain_name = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_AUTH_TYPE))
			{
				_INFO("");

				account->auth_type = g_variant_get_int32 (value);
				_INFO("auth_type = %d", account->auth_type);
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_SECRET))
			{
				_INFO("");

				account->secret = g_variant_get_int32 (value);
				_INFO("secret = %d", account->secret);
			}

			else if (!g_strcmp0(key, ACCOUNT_DATA_KEY_SYNC_SUPPORT))
			{
				account->sync_support = g_variant_get_int32 (value);
				_INFO("sync_support = %d", account->sync_support);
			}
			else if (!strcmp(key, ACCOUNT_DATA_KEY_USER_DATA_INT))
			{
				int i;
				int* tmp_user_ints = unmarshal_user_int_array(value);
				for(i=0; i<USER_INT_CNT; i++)
				{
					account->user_data_int[i] = tmp_user_ints[i];
				}
				_ACCOUNT_FREE(tmp_user_ints);
			}
			else if (!strcmp(key, ACCOUNT_DATA_KEY_USER_DATA_TXT))
			{
				int i;
				char** tmp_user_txts = unmarshal_user_txt_array(value);
				for(i=0; i<USER_TXT_CNT; i++)
				{
					account->user_data_txt[i] = strdup(tmp_user_txts[i]);
					_ACCOUNT_FREE(tmp_user_txts[i]);
				}
				_ACCOUNT_FREE(tmp_user_txts);
			}
			else if (!strcmp(key, ACCOUNT_DATA_KEY_CAPABILITY_LIST))
			{
				account->capablity_list = unmarshal_capability_list (value);
				_INFO("unmarshalled capablity_list");
			}
			else if (!strcmp(key, ACCOUNT_DATA_KEY_CUSTOM_LIST))
			{
				account->custom_list = unmarshal_custom_list (value);
				_INFO("unmarshalled custom_list");
			}
		}
		list = g_slist_append (list, account);
		_INFO("");

	}

	_INFO("");
	return list;
}

GVariant* marshal_account_type_list(GSList* account_type_list)
{
	_INFO("marshal_account_type_list start");
	if (account_type_list == NULL)
	{
		_ERR("input NULL.");
		return NULL;
	}

	GVariantBuilder builder;
	account_type_s *account_type_data = NULL;

	GSList* iter;
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

	for (iter = account_type_list ; iter != NULL; iter = g_slist_next (iter))
	{
		_INFO("iterating account_type_list");
		account_type_data = (account_type_s *) (iter->data);
		g_variant_builder_add_value(&builder, marshal_account_type((account_type_s*)account_type_data));
		_INFO("end one iteration of account_type_list");
	}
	_INFO("marshal_account_type_list end");
	return g_variant_builder_end (&builder);
}

account_s* create_empty_account_instance(void)
{
	account_s *data = (account_s*)malloc(sizeof(account_s));

	if (data == NULL)
	{
		ACCOUNT_ERROR("Memory Allocation Failed");
		return NULL;
	}

	ACCOUNT_MEMSET(data, 0, sizeof(account_s));

	data->id = -1;
	data->user_name = NULL;
	data->email_address = NULL;
	data->display_name = NULL;
	data->icon_path = NULL;
	data->source = NULL;
	data->package_name = NULL;
	data->access_token = NULL;
	data->domain_name = NULL;
	data->auth_type = 0;
	data->secret = 0;
	data->sync_support = false;
	data->capablity_list = NULL;
	data->account_list = false;
	data->custom_list = false;

	return data;
}

account_type_s* create_empty_account_type_instance(void)
{
	account_type_s *data = (account_type_s*)malloc(sizeof(account_type_s));

	if (data == NULL)
	{
		ACCOUNT_ERROR("Memory Allocation Failed");
		return NULL;
	}

	ACCOUNT_MEMSET(data, 0, sizeof(account_type_s));

	data->id = -1;
	data->app_id = NULL;
	data->service_provider_id = NULL;
	data->icon_path = NULL;
	data->small_icon_path = NULL;
	data->multiple_account_support = false;
	data->label_list = NULL;
	data->account_type_list = NULL;
	data->provider_feature_list = NULL;

	return data;
}

GSList* unmarshal_account_type_list(GVariant* variant)
{
	_INFO("");
	GSList *list = NULL;
	GVariantIter iter;
	GVariantIter* iter_row = NULL;
	const gchar *key = NULL;
	GVariant *value = NULL;

	if (variant == NULL)
	{
		_ERR("input NULL");
		return NULL;
	}

	_INFO ("type %s", g_variant_get_type_string (variant));
	gchar* var_type = g_variant_print (variant, TRUE);
	_INFO("var_type = %s", var_type);

	_INFO("before iter");
	g_variant_iter_init (&iter, variant);
	_INFO("after iter");

	_INFO("g_variant_iter_n_children=%d", g_variant_iter_n_children (&iter));

	while (g_variant_iter_next (&iter, "a{sv}", &iter_row))
	{
		_INFO("");
		account_type_s* account_type = create_empty_account_type_instance();

		while (g_variant_iter_loop(iter_row, "{sv}", &key, &value))
		{
			if (!g_strcmp0(key, ACCOUNT_TYPE_DATA_KEY_ID))
			{
				account_type->id = g_variant_get_int32(value);
			}
			else if (!g_strcmp0(key, ACCOUNT_TYPE_DATA_KEY_APP_ID))
			{
				_INFO("");
				account_type->app_id = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_TYPE_DATA_KEY_SERVICE_PROVIDER_ID))
			{
				_INFO("");
				account_type->service_provider_id = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_TYPE_DATA_KEY_ICON_PATH))
			{
				_INFO("");
				account_type->icon_path = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_TYPE_DATA_KEY_SMALL_ICON_PATH))
			{
				_INFO("");
				account_type->small_icon_path = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_TYPE_DATA_KEY_MULTI_SUPPORT))
			{
				_INFO("");
				account_type->multiple_account_support = g_variant_get_int32 (value);
			}
			else if (!g_strcmp0(key, ACCOUNT_TYPE_DATA_KEY_LABEL_LIST))
			{
				_INFO("");
				account_type->label_list = variant_to_label_list(value);
			}

			else if (!g_strcmp0(key, ACCOUNT_TYPE_DATA_KEY_PROVIDER_FEATURE_LIST))
			{
				_INFO("");
				account_type->provider_feature_list = variant_to_provider_feature_list(value);
			}

		}
		list = g_slist_append (list, account_type);
		_INFO("");

	}

	_INFO("");
	return list;
}

GVariant* marshal_account_type(const account_type_s* account_type)
{
	_INFO("marshal_account_type start");
	if (account_type == NULL)
	{
		_ERR("NULL input");
		return NULL;
	}

	const account_type_s* in_data = account_type;
	GVariantBuilder builder;


	g_variant_builder_init (&builder, G_VARIANT_TYPE_VARDICT);


	if (in_data->app_id != NULL)
	{
		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_TYPE_DATA_KEY_APP_ID,
							   g_variant_new_string (in_data->app_id));
	}

	if (in_data->service_provider_id != NULL)
	{
		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_TYPE_DATA_KEY_SERVICE_PROVIDER_ID,
							   g_variant_new_string (in_data->service_provider_id));
	}

	if (in_data->icon_path != NULL)
	{
		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_TYPE_DATA_KEY_ICON_PATH,
							   g_variant_new_string (in_data->icon_path));
	}

	if (in_data->small_icon_path != NULL)
	{
		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_TYPE_DATA_KEY_SMALL_ICON_PATH,
							   g_variant_new_string (in_data->small_icon_path));
	}

	if (in_data->multiple_account_support == false)
	{
		_INFO("Multi account support false");
		g_variant_builder_add (&builder, "{sv}", ACCOUNT_TYPE_DATA_KEY_MULTI_SUPPORT,
						   g_variant_new_int32 (0));
	}
	else
	{
		_INFO("Multi account support true");
		g_variant_builder_add (&builder, "{sv}", ACCOUNT_TYPE_DATA_KEY_MULTI_SUPPORT,
						   g_variant_new_int32 (1));
	}

	if (in_data->label_list != NULL)
	{
		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_TYPE_DATA_KEY_LABEL_LIST,
							   label_list_to_variant(in_data->label_list));
	}

	if (in_data->provider_feature_list != NULL)
	{
		g_variant_builder_add (&builder, "{sv}",
							   ACCOUNT_TYPE_DATA_KEY_PROVIDER_FEATURE_LIST,
							   provider_feature_list_to_variant(in_data->provider_feature_list));
	}


	_INFO("__marshal_account_type end");

	return g_variant_builder_end (&builder);
}

account_type_s* umarshal_account_type(GVariant* in_data)
{
	if (in_data == NULL)
	{
		_ERR("Null input");
		return NULL;
	}

	account_type_s* account_type = (account_type_s*)calloc(1, sizeof(account_type_s));
	if (account_type == NULL)
	{
		_ERR("Out of memory");
		return NULL;
	}

	GVariantIter iter;
	gchar *key = NULL;
	GVariant *value = NULL;

	g_variant_iter_init (&iter, in_data);
	while (g_variant_iter_next (&iter, "{sv}", &key, &value))
	{
		_INFO("[%s]", key);

		if (!strcmp(key, ACCOUNT_TYPE_DATA_KEY_APP_ID))
		{
			_INFO("");
			account_type->app_id = g_strdup(g_variant_get_string (value, NULL));

		}

		else if (!strcmp(key, ACCOUNT_TYPE_DATA_KEY_SERVICE_PROVIDER_ID))
		{
			_INFO("");
			account_type->service_provider_id = g_strdup(g_variant_get_string (value, NULL));

		}

		else if (!strcmp(key, ACCOUNT_TYPE_DATA_KEY_ICON_PATH))
		{
			_INFO("");

			account_type->icon_path = g_strdup(g_variant_get_string (value, NULL));

		}

		else if (!strcmp(key, ACCOUNT_TYPE_DATA_KEY_SMALL_ICON_PATH))
		{
			_INFO("");

			account_type->small_icon_path = g_strdup(g_variant_get_string (value, NULL));

		}

		else if (!strcmp(key, ACCOUNT_TYPE_DATA_KEY_MULTI_SUPPORT))
		{
			_INFO("");

			account_type->multiple_account_support = g_variant_get_int32 (value);

		}

		else if (!strcmp(key, ACCOUNT_TYPE_DATA_KEY_LABEL_LIST))
		{
			_INFO("");

			account_type->label_list = variant_to_label_list(value);

		}

		else if (!strcmp(key, ACCOUNT_TYPE_DATA_KEY_PROVIDER_FEATURE_LIST))
		{
			_INFO("");

			account_type->provider_feature_list = variant_to_provider_feature_list(value);

		}

	}
	//TODO: support for collections

	return account_type;
}

GVariant* marshal_account_capability(account_capability_s* capability)
{
	_INFO("marshal_account_capability start");

	if (capability == NULL)
	{
		_ERR("Null input");
		return NULL;
	}

	GVariantBuilder builder;
	g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);

	g_variant_builder_add(&builder, "{sv}",
						  ACCOUNT_CAPABILITY_DATA_KEY_ID, g_variant_new_int32(capability->id));

	if (capability->type != NULL)
	{
		_INFO("capability->type = %s", capability->type);
		g_variant_builder_add(&builder, "{sv}",
							  ACCOUNT_CAPABILITY_DATA_KEY_TYPE, g_variant_new_string(capability->type));

	}

	_INFO("capability->value = %d", capability->value);
	g_variant_builder_add(&builder, "{sv}",
						  ACCOUNT_CAPABILITY_DATA_KEY_VALUE, g_variant_new_int32(capability->value));

	if (capability->package_name != NULL)
	{
		g_variant_builder_add(&builder, "{sv}",
							  ACCOUNT_CAPABILITY_DATA_KEY_PACKAGE_NAME, g_variant_new_string(capability->package_name));

	}

	if (capability->user_name != NULL)
	{
		g_variant_builder_add(&builder, "{sv}",
							  ACCOUNT_CAPABILITY_DATA_KEY_USER_NAME, g_variant_new_string(capability->user_name));

	}

	g_variant_builder_add(&builder, "{sv}",
						  ACCOUNT_CAPABILITY_DATA_KEY_ACC_ID, g_variant_new_int32(capability->account_id));

	_INFO("marshal_account_capability end");

	return g_variant_builder_end(&builder);
}

GVariant* marshal_capability_list(GSList* capability_list)
{
	_INFO("marshal_capability_list start");
	if (capability_list == NULL)
	{
		_ERR("input NULL.");
		return NULL;
	}

	GVariantBuilder builder;
	account_capability_s *account_capability_data = NULL;

	GSList* iter;
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

	for (iter = capability_list ; iter != NULL; iter = g_slist_next (iter))
	{
		_INFO("iterating capability_list");
		account_capability_data = (account_capability_s *) (iter->data);
		g_variant_builder_add_value(&builder, marshal_account_capability(account_capability_data));
		_INFO("end one iteration of capability_list");
	}
	_INFO("marshal_capability_list end");
	return g_variant_builder_end (&builder);
}

GVariant* marshal_account_custom(account_custom_s* custom)
{
	_INFO("marshal_account_custom start");

	if (custom == NULL)
	{
		_ERR("Null input");
		return NULL;
	}

	GVariantBuilder builder;
	g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);

	g_variant_builder_add(&builder, "{sv}",
						  ACCOUNT_CUSTOM_DATA_KEY_ACC_ID, g_variant_new_int32(custom->account_id));

	if (custom->app_id != NULL)
	{
		g_variant_builder_add(&builder, "{sv}",
							  ACCOUNT_CUSTOM_DATA_KEY_APP_ID, g_variant_new_string(custom->app_id));

	}

	if (custom->key != NULL)
	{
		_INFO("custom->key = %s", custom->key);
		g_variant_builder_add(&builder, "{sv}",
							  ACCOUNT_CUSTOM_DATA_KEY_KEY, g_variant_new_string(custom->key));

	}

	if (custom->value != NULL)
	{
		_INFO("custom->value = %s", custom->value);
		g_variant_builder_add(&builder, "{sv}",
							  ACCOUNT_CUSTOM_DATA_KEY_VALUE, g_variant_new_string(custom->value));

	}

	_INFO("marshal_account_custom end");

	return g_variant_builder_end(&builder);
}

GVariant* marshal_custom_list(GSList* custom_list)
{
	_INFO("marshal_custom_list start");
	if (custom_list == NULL)
	{
		_ERR("input NULL.");
		return NULL;
	}

	GVariantBuilder builder;
	account_custom_s *account_custom_data = NULL;

	GSList* iter;
	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

	for (iter = custom_list ; iter != NULL; iter = g_slist_next (iter))
	{
		_INFO("iterating custom_list");
		account_custom_data = (account_custom_s *) (iter->data);
		g_variant_builder_add_value(&builder, marshal_account_custom(account_custom_data));
		_INFO("end one iteration of custom_list");
	}
	_INFO("marshal_custom_list end");
	return g_variant_builder_end (&builder);
}

GSList* unmarshal_capability_list(GVariant* variant)
{
	_INFO("unmarshal_capability_list start");
	GSList *list = NULL;
	GVariantIter iter;
	GVariantIter* iter_row = NULL;
	const gchar *key = NULL;
	GVariant *value = NULL;

	if (variant == NULL)
	{
		_ERR("input NULL");
		return NULL;
	}

	gchar* var_type = g_variant_print (variant, TRUE);
	_INFO("var_type = %s", var_type);

	_INFO("before iter");
	g_variant_iter_init (&iter, variant);
	_INFO("after iter");

	_INFO("g_variant_iter_n_children=%d", g_variant_iter_n_children (&iter));

	while (g_variant_iter_next (&iter, "a{sv}", &iter_row))
	{
		_INFO("");
		account_capability_s* account_capability = (account_capability_s*)calloc(1, sizeof(account_capability_s));

		while (g_variant_iter_loop(iter_row, "{sv}", &key, &value))
		{
			if (!g_strcmp0(key, ACCOUNT_CAPABILITY_DATA_KEY_ID))
			{
				account_capability->id = g_variant_get_int32(value);
				_INFO("[%d]", account_capability->id);
			}
			else if (!g_strcmp0(key, ACCOUNT_CAPABILITY_DATA_KEY_TYPE))
			{
				_INFO("account_capability->type = %s", value);
				account_capability->type = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_CAPABILITY_DATA_KEY_VALUE))
			{
				_INFO("account_capability->value = %s", value);

				account_capability->value = g_variant_get_int32(value);
			}

			else if (!g_strcmp0(key, ACCOUNT_CAPABILITY_DATA_KEY_PACKAGE_NAME))
			{
				_INFO("");

				account_capability->package_name = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_CAPABILITY_DATA_KEY_USER_NAME))
			{
				_INFO("");

				account_capability->user_name = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_CAPABILITY_DATA_KEY_ACC_ID))
			{
				_INFO("");

				account_capability->account_id = g_variant_get_int32(value);
			}

		}
		list = g_slist_append (list, account_capability);
		_INFO("");

	}

	_INFO("unmarshal_capability_list end");
	return list;
}

GSList* unmarshal_custom_list(GVariant* variant)
{
	_INFO("unmarshal_custom_list start");
	GSList *list = NULL;
	GVariantIter iter;
	GVariantIter* iter_row = NULL;
	const gchar *key = NULL;
	GVariant *value = NULL;

	if (variant == NULL)
	{
		_ERR("input NULL");
		return NULL;
	}

	gchar* var_type = g_variant_print (variant, TRUE);
	_INFO("var_type = %s", var_type);

	_INFO("before iter");
	g_variant_iter_init (&iter, variant);
	_INFO("after iter");

	_INFO("g_variant_iter_n_children=%d", g_variant_iter_n_children (&iter));

	while (g_variant_iter_next (&iter, "a{sv}", &iter_row))
	{
		_INFO("");
		account_custom_s* account_custom = (account_custom_s*)calloc(1, sizeof(account_custom_s));

		while (g_variant_iter_loop(iter_row, "{sv}", &key, &value))
		{
			if (!g_strcmp0(key, ACCOUNT_CUSTOM_DATA_KEY_ACC_ID))
			{
				account_custom->account_id = g_variant_get_int32(value);
				_INFO("[%d]", account_custom->account_id);
			}
			else if (!g_strcmp0(key, ACCOUNT_CUSTOM_DATA_KEY_APP_ID))
			{
				_INFO("");
				account_custom->app_id = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_CUSTOM_DATA_KEY_KEY))
			{
				_INFO("account_custom->key = %s", value);

				account_custom->key = g_strdup(g_variant_get_string (value, NULL));
			}

			else if (!g_strcmp0(key, ACCOUNT_CUSTOM_DATA_KEY_VALUE))
			{
				_INFO("account_custom->value = %s", value);

				account_custom->value = g_strdup(g_variant_get_string (value, NULL));
			}
		}
		list = g_slist_append (list, account_custom);
		_INFO("");

	}

	_INFO("unmarshal_custom_list end");
	return list;
}

GVariant* marshal_user_int_array(const int* user_data_int_array)
{
	int i;

	_INFO("marshal_user_data_int_list start");
	if (user_data_int_array == NULL)
	{
		_ERR("input NULL.");
		return NULL;
	}

	GVariantBuilder builder;

	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

	for(i = 0; i < USER_INT_CNT; i++)
	{
		char key[256];
		ACCOUNT_SNPRINTF(key, strlen(ACCOUNT_DATA_KEY_USER_DATA_INT)+3, "%s%d", ACCOUNT_DATA_KEY_USER_DATA_INT, i);
		_INFO("iterating user_data_int[%d]", i);
		g_variant_builder_add(&builder, "{sv}",
				key,
				g_variant_new_int32(user_data_int_array[i]));
		_INFO("marshal user_data_int[%d]=%d, key=%s", i, user_data_int_array[i], key);
		_INFO("end one iteration of user_data_int");
	}

	_INFO("marshal_user_data_int_list end");
	return g_variant_builder_end (&builder);
}

GVariant* marshal_user_txt_array(char* const* user_data_txt_array)
{
	int i;

	_INFO("marshal_user_data_int_list start");
	if (user_data_txt_array == NULL)
	{
		_ERR("input NULL.");
		return NULL;
	}

	GVariantBuilder builder;

	g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);

	for(i = 0; i < USER_TXT_CNT; i++)
	{
		_INFO("iterating user_data_txt[%s]", user_data_txt_array[i]);
		char key[256];
		ACCOUNT_SNPRINTF(key, strlen(ACCOUNT_DATA_KEY_USER_DATA_TXT)+3, "%s%d", ACCOUNT_DATA_KEY_USER_DATA_TXT, i);
		g_variant_builder_add(&builder, "{sv}",
				key,
				g_variant_new_string(user_data_txt_array[i]));
		_INFO("end one iteration of user_data_txt");
	}

	_INFO("marshal_user_data_int_list end");
	return g_variant_builder_end (&builder);
}

int* unmarshal_user_int_array(GVariant* variant)
{
	_INFO("unmarshal_user_int_array start");
	GVariantIter iter;
	const gchar *key = NULL;
	char compare_key[USER_INT_CNT][256];
	GVariant *value = NULL;
	int* user_data_int;
	int i;

	if (variant == NULL)
	{
		_ERR("input NULL");
		return NULL;
	}

	gchar* var_type = g_variant_print (variant, TRUE);
	_INFO("var_type = %s", var_type);

	_INFO("before iter");
	g_variant_iter_init (&iter, variant);
	_INFO("after iter");

	user_data_int = (int*)calloc(USER_INT_CNT, sizeof(int));

	for(i=0; i<USER_INT_CNT; i++)
	{
		ACCOUNT_SNPRINTF(compare_key[i], strlen(ACCOUNT_DATA_KEY_USER_DATA_INT)+3, "%s%d", ACCOUNT_DATA_KEY_USER_DATA_INT, i);
	}

	while (g_variant_iter_next (&iter, "{sv}", &key, &value))
	{
		for(i=0; i<USER_INT_CNT; i++)
		{
			if (!g_strcmp0(key, compare_key[i]))
			{
				user_data_int[i] = g_variant_get_int32(value);
				_INFO("unmarshal user_data_int[%d]=%d, key=%s, compare=%s", i, user_data_int[i],key, compare_key[i]);
			}
		}
	}

	_INFO("unmarshal_user_int_array end");
	return user_data_int;
}

char** unmarshal_user_txt_array(GVariant* variant)
{
	_INFO("unmarshal_user_txt_array start");
	GVariantIter iter;
	const gchar *key = NULL;
	char compare_key[USER_TXT_CNT][256];
	GVariant *value = NULL;
	char** user_data_txts;
	int i;

	if (variant == NULL)
	{
		_ERR("input NULL");
		return NULL;
	}

	gchar* var_type = g_variant_print (variant, TRUE);
	_INFO("var_type = %s", var_type);

	_INFO("before iter");
	g_variant_iter_init (&iter, variant);
	_INFO("after iter");

	user_data_txts = (char**)calloc(USER_TXT_CNT, sizeof(char*));

	for(i=0; i<USER_TXT_CNT; i++)
	{
		ACCOUNT_SNPRINTF(compare_key[i], strlen(ACCOUNT_DATA_KEY_USER_DATA_TXT)+3, "%s%d", ACCOUNT_DATA_KEY_USER_DATA_TXT, i);
	}

	while (g_variant_iter_next (&iter, "{sv}", &key, &value))
	{
		for(i=0; i<USER_TXT_CNT; i++)
		{
			if (!g_strcmp0(key, compare_key[i]))
			{
				user_data_txts[i] = g_strdup(g_variant_get_string (value, NULL));
				_INFO("[%s]", user_data_txts[i]);
			}
		}
	}

	_INFO("unmarshal_user_txt_array end");
	return user_data_txts;
}
