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

#include <stdlib.h>
#include <glib.h>
#include <fcntl.h>
#include <glib/gprintf.h>
#ifdef G_OS_UNIX
#include <gio/gunixfdlist.h>
#include <unistd.h>
#endif
#include <vconf.h>

#include <dbg.h>
#include <account-private.h>
#include <account_ipc_marshal.h>
#include <account-mgr-stub.h>

#include "account.h"
#include "account-error.h"
#include "account-types.h"
#include "account_internal.h"
#include "account_private_client.h"

#define ACCOUNT_DB_OPEN_READONLY 0
#define ACCOUNT_DB_OPEN_READWRITE 1

#define VCONF_OK 0

static AccountManager *_acc_mgr = NULL;

static char *_account_get_text(const char *text_data);
static int _account_gslist_free(GSList* list);
static int _account_glist_free(GList* list);

static int _account_free_capability_items(account_capability_s *data)
{
	_ACCOUNT_FREE(data->type);
	_ACCOUNT_FREE(data->package_name);
	_ACCOUNT_FREE(data->user_name);

	return ACCOUNT_ERROR_NONE;
}

static int _account_custom_item_free(account_custom_s *data)
{
	_ACCOUNT_FREE(data->app_id);
	_ACCOUNT_FREE(data->key);
	_ACCOUNT_FREE(data->value);

	return ACCOUNT_ERROR_NONE;
}

static int _account_custom_gslist_free(GSList* list)
{
	if(!list){
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	GSList* iter;

	for (iter = list; iter != NULL; iter = g_slist_next(iter)) {
		account_custom_s *custom_data = (account_custom_s*)iter->data;
		_account_custom_item_free(custom_data);
		_ACCOUNT_FREE(custom_data);
	}

	g_slist_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}
/*
static int _account_list_free(GList* list)
{
	if(!list){
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	g_list_free_full(list, g_free);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}
*/

static int _account_free_account_items(account_s *data)
{
	_ACCOUNT_FREE(data->user_name);
	_ACCOUNT_FREE(data->email_address);
	_ACCOUNT_FREE(data->display_name);
	_ACCOUNT_FREE(data->icon_path);
	_ACCOUNT_FREE(data->source);
	_ACCOUNT_FREE(data->package_name);
	_ACCOUNT_FREE(data->domain_name);
	_ACCOUNT_FREE(data->access_token);

	int i;
	for(i=0;i<USER_TXT_CNT;i++)
		_ACCOUNT_FREE(data->user_data_txt[i]);

	_account_gslist_free(data->capablity_list);
	_account_glist_free(data->account_list);
	_account_custom_gslist_free(data->custom_list);
//	_account_list_free(data->domain_list);
//	_account_list_free(data->mechanism_list);

	return ACCOUNT_ERROR_NONE;
}

static int _account_gslist_free(GSList* list)
{
	if(!list){
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	GSList* iter;

	for (iter = list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = (account_capability_s*)iter->data;
		_account_free_capability_items(cap_data);
		_ACCOUNT_FREE(cap_data);
	}

	g_slist_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}

static int _account_glist_free(GList* list)
{
	if(!list){
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	GList* iter;

	for (iter = list; iter != NULL; iter = g_list_next(iter)) {
		account_s *account_record = (account_s*)iter->data;
		_account_free_account_items(account_record);
		_ACCOUNT_FREE(account_record);
	}

	g_list_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}

static char *_account_get_text(const char *text_data)
{
	char *text_value = NULL;

	if (text_data != NULL) {
		text_value = strdup(text_data);
	}
	return text_value;
}

//FIXME : add true singleton logic
AccountManager *
_account_manager_get_instance ()
{
	_INFO("_account_manager_get_instance");
	if (_acc_mgr != NULL)
	{
		_INFO("instance already created.");
		return _acc_mgr;
	}

#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();
#endif

	GDBusConnection *connection = NULL;
	GError *error = NULL;

	connection = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, &error);

	_INFO("after g_bus_get_sync");


	/* Create the object */
	_acc_mgr =
		account_manager_proxy_new_sync(connection,
										 G_DBUS_PROXY_FLAGS_NONE,
										 "org.tizen.account.manager",
										"/org/tizen/account/manager",
										NULL,
										 &error);
	_INFO("_account_manager_get_instance end");
	return _acc_mgr;
}

GDBusErrorEntry _account_svc_errors[] =
{
	{ACCOUNT_ERROR_NONE, _ACCOUNT_SVC_ERROR_PREFIX".NoError"},
	{ACCOUNT_ERROR_OUT_OF_MEMORY, _ACCOUNT_SVC_ERROR_PREFIX".OutOfMemory"},
	{ACCOUNT_ERROR_INVALID_PARAMETER, _ACCOUNT_SVC_ERROR_PREFIX".InvalidParameter"},
	{ACCOUNT_ERROR_DUPLICATED, _ACCOUNT_SVC_ERROR_PREFIX".Duplicated"},
	{ACCOUNT_ERROR_NO_DATA, _ACCOUNT_SVC_ERROR_PREFIX".NoData"},
	{ACCOUNT_ERROR_RECORD_NOT_FOUND, _ACCOUNT_SVC_ERROR_PREFIX".RecordNotFound"},
	{ACCOUNT_ERROR_DB_FAILED, _ACCOUNT_SVC_ERROR_PREFIX".DBFailed"},
	{ACCOUNT_ERROR_DB_NOT_OPENED, _ACCOUNT_SVC_ERROR_PREFIX".DBNotOpened"},
	{ACCOUNT_ERROR_QUERY_SYNTAX_ERROR, _ACCOUNT_SVC_ERROR_PREFIX".QuerySynTaxError"},
	{ACCOUNT_ERROR_ITERATOR_END, _ACCOUNT_SVC_ERROR_PREFIX".IteratorEnd"},
	{ACCOUNT_ERROR_NOTI_FAILED, _ACCOUNT_SVC_ERROR_PREFIX".NotiFalied"},
	{ACCOUNT_ERROR_PERMISSION_DENIED, _ACCOUNT_SVC_ERROR_PREFIX".PermissionDenied"},
	{ACCOUNT_ERROR_XML_PARSE_FAILED, _ACCOUNT_SVC_ERROR_PREFIX".XMLParseFailed"},
	{ACCOUNT_ERROR_XML_FILE_NOT_FOUND, _ACCOUNT_SVC_ERROR_PREFIX".FileNotFound"},
	{ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL, _ACCOUNT_SVC_ERROR_PREFIX".SubscriptionFailed"},
	{ACCOUNT_ERROR_NOT_REGISTERED_PROVIDER, _ACCOUNT_SVC_ERROR_PREFIX".NotRegisteredProvider"},
	{ACCOUNT_ERROR_NOT_ALLOW_MULTIPLE, _ACCOUNT_SVC_ERROR_PREFIX".NotAllowMultiple"},
	{ACCOUNT_ERROR_DATABASE_BUSY, _ACCOUNT_SVC_ERROR_PREFIX".database_busy"},
};

static int _account_get_error_code(bool is_success, GError *error)
{
	if (!is_success)
	{
		_INFO("Received error Domain[%d] Message[%s] Code[%d]", error->domain, error->message, error->code);

		if (g_dbus_error_is_remote_error(error))
		{
			gchar *remote_error = g_dbus_error_get_remote_error(error);
			if (remote_error)
			{
				_INFO("Remote error[%s]", remote_error);

				//FIXME: Temp fix, error->code sent from daemon is not the same as the one received.
				//However error->message matches, so doing reverse lookup
				int error_enum_count = G_N_ELEMENTS(_account_svc_errors);
				int i = 0;
				for (i = 0; i < error_enum_count; i++)
				{
					if (g_strcmp0(_account_svc_errors[i].dbus_error_name, remote_error) == 0)
					{
						_INFO("Remote error code matched[%d]", _account_svc_errors[i].error_code);
						return _account_svc_errors[i].error_code;
					}
				}
			}
		}
		//All undocumented errors mapped to ACCOUNT_ERROR_PERMISSION_DENIED
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_connect(void)
{
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_connect_readonly(void)
{
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_disconnect(void)
{
	return ACCOUNT_ERROR_NONE;
}


ACCOUNT_API int account_insert_to_db(account_h account, int *account_db_id)
{
	_INFO("1. account_insert_to_db start");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((account_db_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT ID POINTER IS NULL"));

	account_s *account_data = (account_s*) account;
	int error_code = ACCOUNT_ERROR_NONE;
	GError *error = NULL;

	_INFO("2. Before _account_manager_get_instance()");
	AccountManager* acc_mgr = _account_manager_get_instance();
	ACCOUNT_CATCH_ERROR((acc_mgr != NULL), {}, ACCOUNT_ERROR_PERMISSION_DENIED, "Failed to get dbus.");

	int db_id = -1;
	GVariant* account_serialized = marshal_account(account_data);

	_INFO("3. Before account_manager_call_account_add_sync");
	bool is_success = account_manager_call_account_add_sync(acc_mgr, account_db_path, account_serialized, &db_id, NULL, &error);
	ACCOUNT_CATCH_ERROR((is_success != false), {}, _account_get_error_code(is_success, error), "Failed to get dbus.");

	*account_db_id = db_id;
	account_data->id = db_id;

	_INFO("4. account_insert_to_db end, added db id [%d] [%d] [%d]", db_id, *account_db_id, account_data->id);

	return ACCOUNT_ERROR_NONE;

CATCH:
	//Failed to get dbus.
	_ERR("account_manager_call_account_add_sync()=[%d]", error_code);

	return error_code;
}

ACCOUNT_API int account_delete_from_db_by_id(int account_db_id)
{
	_INFO("1. account_delete_from_db_by_id starting [%d]", account_db_id);
	char* account_db_path = ACCOUNT_DB_PATH;
	int error_code = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((account_db_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT ID IS LESS THAN ZERO."));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	_INFO("2. Before account_manager_call_account_query_account_by_account_id_sync");
	GVariant *account_serialized_old = NULL;
	bool is_success = account_manager_call_account_query_account_by_account_id_sync(acc_mgr, account_db_path, account_db_id, &account_serialized_old, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_query_account_by_account_id_sync failed [%d]", error_code);
		return error_code;
	}

	_INFO("3. Before account_manager_call_account_delete_from_db_by_id_sync");
	is_success = account_manager_call_account_delete_from_db_by_id_sync(acc_mgr, account_db_path, account_db_id, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_delete_from_db_by_id_sync failed [%d]", error_code);
		return error_code;
	}

	_INFO("4. Before account_delete_from_db_by_id end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_delete_from_db_by_user_name(char *user_name, char *package_name)
{
	int error_code = ACCOUNT_ERROR_NONE;
	char* account_db_path = ACCOUNT_DB_PATH;
	_INFO("account_delete_from_db_by_user_name start");

	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("user_name is null!"));
	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name is null!"));


	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_list_variant = NULL;
	bool is_success = account_manager_call_account_query_account_by_user_name_sync(acc_mgr, account_db_path, user_name, &account_list_variant, NULL, &error);

	error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		_ERR("account_query_account_by_user_name error=[%d]", error_code);
		return error_code;
	}

	_INFO("before unmarshal_account_list");
	GSList* account_list = unmarshal_account_list(account_list_variant);
	_INFO("after unmarshal_account_list");
	if (account_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	//TODO free account_list, account_list_variant

	is_success = account_manager_call_account_delete_from_db_by_user_name_sync(acc_mgr, account_db_path, user_name, package_name, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_delete_from_db_by_user_name_sync failed [%d]", error_code);
		return error_code;
	}

	return ACCOUNT_ERROR_NONE;
}

int _account_delete_from_db_by_package_name(const char *package_name, bool permission)
{
	_INFO("_account_delete_from_db_by_package_name starting permission opions = %d", permission);
	char* account_db_path = ACCOUNT_DB_PATH;
	int error_code = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name is null!"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}
/*
	//First get account list of user_name, used for gSSO DB deletion
	GVariant* account_list_variant = NULL;
	bool is_success = account_manager_call_account_query_account_by_package_name_sync(acc_mgr, account_db_path, package_name, &account_list_variant, NULL, &error);

	error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	_INFO("before unmarshal_account_list");
	GSList* account_list = unmarshal_account_list(account_list_variant);
	_INFO("after unmarshal_account_list");
	if (account_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}
*/
	bool is_success = account_manager_call_account_delete_from_db_by_package_name_sync(acc_mgr, account_db_path, package_name, permission, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_delete_from_db_by_package_name_sync failed [%d]", error_code);
		return error_code;
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_delete_from_db_by_package_name(const char *package_name)
{
	_INFO("account_delete_from_db_by_package_name start");
	return _account_delete_from_db_by_package_name(package_name, true);
}

ACCOUNT_INTERNAL_API int account_delete_from_db_by_package_name_without_permission(const char *package_name)
{
	_INFO("account_delete_from_db_by_package_name starting without permission");
	return _account_delete_from_db_by_package_name(package_name, false);
}

ACCOUNT_API int account_update_to_db_by_id(account_h account, int account_id)
{
	//First we will update account db
	_INFO("1. account_update_to_db_by_id start");
	char* account_db_path = ACCOUNT_DB_PATH;
	int error_code = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Account id is not valid"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	_INFO("2. Before account_manager_call_account_query_account_by_account_id_sync");
	GVariant *account_serialized_old = NULL;
	bool is_success = account_manager_call_account_query_account_by_account_id_sync(acc_mgr, account_db_path, account_id, &account_serialized_old, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_query_account_by_account_id_sync failed [%d]", error_code);
		return error_code;
	}

	_INFO("3. Before account_manager_call_account_update_to_db_by_id_sync");
	GVariant* account_serialized = marshal_account((account_s*) account);
	is_success = account_manager_call_account_update_to_db_by_id_sync(acc_mgr, account_db_path, account_serialized, account_id, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_update_to_db_by_id_sync failed [%d]", error_code);
		return error_code;
	}

	_INFO("4. account_update_to_db_by_id end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_update_to_db_by_id_ex(account_h account, int account_id)
{
	int ret = -1;
	ret = account_update_to_db_by_id(account, account_id);

	return ret;
}

ACCOUNT_INTERNAL_API int account_update_to_db_by_id_without_permission(account_h account, int account_id)
{
	//First we will update account db
	//Then we will update gSSO DB, if it fails then we will rollback account db updates

	_INFO("account_update_to_db_by_id_without_permission start");
	char* account_db_path = ACCOUNT_DB_PATH;
	int error_code = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Account id is not valid"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant *account_serialized_old = NULL;
	_INFO("before query() account_id[%d]", account_id);
	bool is_success = account_manager_call_account_query_account_by_account_id_sync(acc_mgr, account_db_path, account_id, &account_serialized_old, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_query_account_by_account_id_sync failed [%d]", error_code);
		return error_code;
	}

	_INFO("before marshal() : account_id[%d], user_name=%s", account_id, ((account_s*)account)->user_name);
	GVariant* account_serialized = marshal_account((account_s*) account);
	_INFO("after marshal() : account_id[%d]", account_id);
	if (account_serialized == NULL)
	{
		_ERR("Invalid input");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	_INFO("before call update() : account_id[%d]", account_id);
	is_success = account_manager_call_account_update_to_db_by_id_ex_sync(acc_mgr, account_db_path, account_serialized, account_id, NULL, &error);

	_INFO("after call update() : is_success=%d", is_success);
	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_update_to_db_by_id_ex_sync failed [%d]", error_code);
		return error_code;
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_update_to_db_by_user_name(account_h account, const char *user_name, const char *package_name)
{
	//First we will update account db
	_INFO("account_update_to_db_by_user_name starting");
	char* account_db_path = ACCOUNT_DB_PATH;
	int error_code = ACCOUNT_ERROR_NONE;

	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("USER NAME IS NULL"));
	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("PACKAGE NAME IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant *account_serialized_old = NULL;
	account_s *account_data = (account_s*) account;
	bool is_success = account_manager_call_account_query_account_by_account_id_sync(acc_mgr, account_db_path, account_data->id, &account_serialized_old, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_query_account_by_account_id_sync failed [%d]", error_code);
		return error_code;
	}

	GVariant* account_serialized = marshal_account(account_data);
	is_success = account_manager_call_account_update_to_db_by_user_name_sync(acc_mgr, account_db_path, account_serialized, user_name, package_name, NULL, &error);

	if (!is_success)
	{
		error_code = _account_get_error_code(is_success, error);
		_ERR("account_manager_call_account_update_to_db_by_user_name_sync failed [%d]", error_code);
		return error_code;
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_create(account_h *account)
{
	_INFO("account_create start");
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)malloc(sizeof(account_s));

	if (data == NULL) {
		ACCOUNT_FATAL("Memory Allocation Failed");
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}
	ACCOUNT_MEMSET(data, 0, sizeof(account_s));

	/*Setting account as visible by default*/
	data->secret = ACCOUNT_SECRECY_VISIBLE;

	/*Setting account as not supporting sync by default*/
	data->sync_support = ACCOUNT_SYNC_NOT_SUPPORT;

	data->auth_type = ACCOUNT_AUTH_TYPE_INVALID;

//	data->account_list = NULL;
	data->capablity_list = NULL;
	data->custom_list = NULL;
//	data->domain_list = NULL;
//	data->mechanism_list = NULL;

	*account = (account_h)data;

	_INFO("account_create end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_destroy(account_h account)
{
	_INFO("account_destroy start");
	account_s *data = (account_s*)account;

	ACCOUNT_RETURN_VAL((data != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Account handle is null!"));

	_account_free_account_items(data);
	_ACCOUNT_FREE(data);

	_INFO("account_destroy end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_user_name(account_h account, const char *user_name)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_name) {
		ACCOUNT_SLOGE("(%s)-(%d) user_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->user_name);
	data->user_name = _account_get_text(user_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_display_name(account_h account, const char *display_name)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!display_name) {
		ACCOUNT_SLOGE("(%s)-(%d) display_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->display_name);
	data->display_name = _account_get_text(display_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_email_address(account_h account, const char *email_address)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!email_address) {
		ACCOUNT_SLOGE("(%s)-(%d) email_address is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->email_address);
	data->email_address = _account_get_text(email_address);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_icon_path(account_h account, const char *icon_path)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		ACCOUNT_SLOGE("(%s)-(%d) icon_path is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->icon_path);
	data->icon_path = _account_get_text(icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_source(account_h account, const char *source)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!source) {
		ACCOUNT_SLOGE("(%s)-(%d) source is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->source);
	data->source = _account_get_text(source);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_package_name(account_h account, const char *package_name)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!package_name) {
		ACCOUNT_SLOGE("(%s)-(%d) package_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->package_name);
	data->package_name = _account_get_text(package_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_domain_name(account_h account, const char *domain_name)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!domain_name) {
		ACCOUNT_SLOGE("(%s)-(%d) domain_name is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->domain_name);
	data->domain_name = _account_get_text(domain_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_access_token(account_h account, const char *access_token)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!access_token) {
		ACCOUNT_SLOGE("(%s)-(%d) access_token is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->access_token);
	data->access_token = _account_get_text(access_token);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_user_text(account_h account, int idx, const char *user_txt)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_txt) {
		ACCOUNT_SLOGE("(%s)-(%d) user_txt is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (idx >= USER_TXT_CNT || idx < 0) {
		ACCOUNT_SLOGE("(%s)-(%d) idx rage should be between 0-4.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	_ACCOUNT_FREE(data->user_data_txt[idx]);
	data->user_data_txt[idx] = _account_get_text(user_txt);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_custom(account_h account, const char* key, const char* value)
{
	if (!account) {
		ACCOUNT_SLOGE("(%s)-(%d) account handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!key) {
		ACCOUNT_SLOGE("(%s)-(%d) key is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!value) {
		ACCOUNT_SLOGE("(%s)-(%d) value is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	GSList *iter;
	bool b_is_new = TRUE;

	for (iter = data->custom_list; iter != NULL; iter = g_slist_next(iter)) {

		account_custom_s* custom_data = NULL;
		custom_data = (account_custom_s*)iter->data;
		ACCOUNT_SLOGD( "account_custom_s->key = %s, account_custom_s->value = %s \n", custom_data->key, custom_data->value);

		if(!strcmp(custom_data->key, key)) {
			_ACCOUNT_FREE(custom_data->value);
			custom_data->value = _account_get_text(value);
			b_is_new = FALSE;
		}
	}

	if(b_is_new) {
		account_custom_s* custom_data = (account_custom_s*)malloc(sizeof(account_custom_s));

		if (custom_data == NULL) {
			return ACCOUNT_ERROR_OUT_OF_MEMORY;
		}
		ACCOUNT_MEMSET(custom_data, 0, sizeof(account_custom_s));
		custom_data->account_id = data->id;
		custom_data->app_id = _account_get_text(data->package_name);
		custom_data->key = _account_get_text(key);
		custom_data->value = _account_get_text(value);
		data->custom_list = g_slist_append(data->custom_list, (gpointer)custom_data);
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_auth_type(account_h account, const account_auth_type_e auth_type)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",  __FUNCTION__, __LINE__));

	if ( ((int)auth_type < 0) || (auth_type > ACCOUNT_AUTH_TYPE_CLIENT_LOGIN)) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->auth_type = (int)auth_type;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_secret(account_h account, const account_secrecy_state_e secret)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",	__FUNCTION__, __LINE__));

	if ( ((int)secret < 0) || (secret > ACCOUNT_SECRECY_VISIBLE)) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->secret = (int)secret;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_sync_support(account_h account, const account_sync_state_e sync_support)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",	__FUNCTION__, __LINE__));

	if ( ((int)sync_support < 0) || (sync_support > ACCOUNT_SUPPORTS_SYNC)) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->sync_support= (int)sync_support;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_user_int(account_h account, int idx, const int user_int)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (idx >= USER_INT_CNT ||idx < 0) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	data->user_data_int[idx] = user_int;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_set_capability(account_h account, const char* capability_type, account_capability_state_e capability_value)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("account handle is null"));
	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type is null"));

	if ((capability_value != ACCOUNT_CAPABILITY_DISABLED) && (capability_value != ACCOUNT_CAPABILITY_ENABLED)) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	GSList *iter = NULL;
	bool b_is_new = TRUE;

	for(iter = data->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = NULL;
		cap_data = (account_capability_s*)iter->data;

		if(!strcmp(cap_data->type, capability_type)) {
			cap_data->value = capability_value;
			b_is_new = FALSE;
			break;
		}
	}

	if(b_is_new) {
		account_capability_s* cap_data = (account_capability_s*)malloc(sizeof(account_capability_s));

		if (cap_data == NULL)
			return ACCOUNT_ERROR_OUT_OF_MEMORY;
		ACCOUNT_MEMSET(cap_data, 0, sizeof(account_capability_s));

		cap_data->type = _account_get_text(capability_type);
		cap_data->value = capability_value;
		data->capablity_list = g_slist_append(data->capablity_list, (gpointer)cap_data);
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_user_name(account_h account, char **user_name)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!user_name) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*user_name) = NULL;
	*user_name = _account_get_text(data->user_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_display_name(account_h account, char **display_name)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!display_name) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*display_name) = NULL;

	*display_name = _account_get_text(data->display_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_email_address(account_h account,char **email_address)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!email_address) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*email_address) = NULL;

	*email_address = _account_get_text(data->email_address);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int  account_get_icon_path(account_h account, char **icon_path)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*icon_path) = NULL;

	*icon_path = _account_get_text(data->icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_source(account_h account, char **source)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!source) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*source) = NULL;

	*source = _account_get_text(data->source);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_package_name(account_h account, char **package_name)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!package_name) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*package_name) = NULL;

	*package_name = _account_get_text(data->package_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_domain_name(account_h account, char **domain_name)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!domain_name) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*domain_name) = NULL;

	*domain_name = _account_get_text(data->domain_name);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_access_token(account_h account, char **access_token)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!access_token) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	(*access_token) = NULL;

	*access_token = _account_get_text(data->access_token);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_user_text(account_h account, int user_text_index, char **text)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!text) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	ACCOUNT_RETURN_VAL((user_text_index >=0 && user_text_index < USER_TXT_CNT ), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("INVALID USER TEXT INDEX"));

	account_s *data = (account_s*)account;

	(*text) = NULL;

	*text = _account_get_text(data->user_data_txt[user_text_index]);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_auth_type(account_h account, account_auth_type_e *auth_type)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!auth_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s* data = (account_s*)account;

	*auth_type = data->auth_type;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_secret(account_h account, account_secrecy_state_e *secret)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!secret) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s* data = (account_s*)account;

	*secret = data->secret;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_sync_support(account_h account, account_sync_state_e *sync_support)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!sync_support) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s* data = (account_s*)account;

	*sync_support = data->sync_support;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_account_id(account_h account, int *account_id)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!account_id) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	*account_id = data->id;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_user_int(account_h account, int user_int_index, int *integer)
{
	if (!account) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_RETURN_VAL((user_int_index >=0 && user_int_index < USER_TXT_CNT ), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("INVALID USER TEXT INDEX"));

	if (!integer) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_s *data = (account_s*)account;

	*integer = data->user_data_int[user_int_index];

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_capability(account_h account, const char* capability_type, account_capability_state_e* capability_value)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type is NULL"));
	ACCOUNT_RETURN_VAL((capability_value != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_value is NULL"));

	GSList *iter;
	account_s *data = (account_s*)account;

	_ERR("before for()");
	for (iter = data->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = NULL;

		cap_data = (account_capability_s*)iter->data;

	_ERR("capability_type = %s, data->type = %s", capability_type, cap_data->type);
	_ERR("capability_value = %d, data->value= %d", capability_value, cap_data->value);
		if(!strcmp(capability_type, cap_data->type)) {
			*capability_value = cap_data->value;
			return ACCOUNT_ERROR_NONE;
		}
	}
	_ERR("after for()");

	return ACCOUNT_ERROR_RECORD_NOT_FOUND;
}

ACCOUNT_API int account_get_capability_all(account_h account, capability_cb callback, void *user_data)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_s *data = (account_s*)account;

	for (iter = data->capablity_list; iter != NULL; iter = g_slist_next(iter)) {
		account_capability_s *cap_data = NULL;

		cap_data = (account_capability_s*)iter->data;

		if(callback(cap_data->type, cap_data->value, user_data)!=TRUE){
			return ACCOUNT_ERROR_NONE;
		}
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_custom(account_h account, const char* key, char** value)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((key != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO KEY TO REQUEST"));
	ACCOUNT_RETURN_VAL((value != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("VALUE POINTER IS NULL"));

	GSList *iter;
	account_s *data = (account_s*)account;

	_ERR("before for()");
	for (iter = data->custom_list; iter != NULL; iter = g_slist_next(iter)) {
		account_custom_s *custom_data = NULL;

		custom_data = (account_custom_s*)iter->data;

		_ERR("key = %s, custom_data->key = %s", key, custom_data->key);
		_ERR("value = %s, custom_data->value = %s", value, custom_data->value);

		if(!strcmp(key, custom_data->key)) {
			(*value) = NULL;
			*value = _account_get_text(custom_data->value);
			return ACCOUNT_ERROR_NONE;
		}
	}
	_ERR("after for()");

	return ACCOUNT_ERROR_RECORD_NOT_FOUND;
}

ACCOUNT_API int account_get_custom_all(account_h account, account_custom_cb callback, void* user_data)
{
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_s *data = (account_s*)account;

	for (iter = data->custom_list; iter != NULL; iter = g_slist_next(iter)) {
		bool cb_ret = FALSE;
		account_custom_s *custom_data = NULL;

		custom_data = (account_custom_s*)iter->data;

		cb_ret = callback(custom_data->key, custom_data->value, user_data);
		if(!cb_ret) {
			break;
		}
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_foreach_account_from_db(account_cb callback, void *user_data)
{
	_INFO("account_foreach_account_from_db start");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT Callback IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_list_variant = NULL;
	bool is_success = account_manager_call_account_query_all_sync(acc_mgr, account_db_path, &account_list_variant, NULL, &error);

	int error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	_INFO("before unmarshal_account_list");
	GSList* account_list = unmarshal_account_list(account_list_variant);
	_INFO("after unmarshal_account_list");
	GSList* iter;

	for (iter = account_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_list");
		account_s *account = NULL;
		account = (account_s*)iter->data;
		_INFO("Before _account_query_identity_info_by_id");

		_INFO("account->id=%d", account->id);
		if (callback((account_h)account, user_data) == false)
		{
			_INFO("application callback requested to discontinue.");
			break;
		}
		_INFO("After one iteration callback");
	}
	_INFO("account_foreach_account_from_db end");

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_query_account_by_account_id(int account_db_id, account_h *account)
{
	_INFO("account_query_account_by_account_id start [%d]", account_db_id);
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((account_db_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));
	ACCOUNT_RETURN_VAL((account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT POINTER IS NULL"));
	ACCOUNT_RETURN_VAL((*account != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_variant = NULL;
	bool is_success = account_manager_call_account_query_account_by_account_id_sync(acc_mgr, account_db_path, account_db_id, &account_variant, NULL, &error);

	int error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	_INFO("before unmarshal_account");
	account_s* account_data = umarshal_account(account_variant);
	_INFO("after unmarshal_account");

	if (account_data == NULL)
	{
		_ERR("Failed to unmarshal");
		return ACCOUNT_ERROR_DB_FAILED;
	}

	*account = (account_h) account_data;

	_INFO("account_query_account_by_account_id end");

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_query_account_by_user_name(account_cb callback, const char* user_name, void* user_data)
{
	_INFO("account_query_account_by_user_name starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((user_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("USER NAME IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_list_variant = NULL;
	bool is_success = account_manager_call_account_query_account_by_user_name_sync(acc_mgr, account_db_path, user_name, &account_list_variant, NULL, &error);

	int error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	_INFO("before unmarshal_account_list");
	GSList* account_list = unmarshal_account_list(account_list_variant);
	_INFO("after unmarshal_account_list");
	if (account_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	GSList* iter;

	for (iter = account_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_list");
		account_s *account = NULL;
		account = (account_s*)iter->data;
		if (callback((account_h)account, user_data) == false)
		{
			_INFO("application callback requested to discontinue.");
			break;
		}
	}
	_INFO("account_query_account_by_user_name end");

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_query_account_by_package_name(account_cb callback, const char *package_name, void *user_data)
{
	_INFO("account_query_account_by_package_name starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("PACKAGE NAME IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_list_variant = NULL;
	bool is_success = account_manager_call_account_query_account_by_package_name_sync(acc_mgr, account_db_path, package_name, &account_list_variant, NULL, &error);

	int error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	_INFO("before unmarshal_account_list");
	GSList* account_list = unmarshal_account_list(account_list_variant);
	_INFO("after unmarshal_account_list");
	if (account_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	GSList* iter;

	for (iter = account_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_list");
		account_s *account = NULL;
		account = (account_s*)iter->data;

		if (callback((account_h)account, user_data) == false)
		{
			_INFO("application callback requested to discontinue.");
			break;
		}
	}
	_INFO("account_query_account_by_package_name end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_query_account_by_capability(account_cb callback, const char* capability_type, account_capability_state_e capability_value, void *user_data)
{
	_INFO("account_query_account_by_capability starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type IS NULL"));

	if (((int)capability_value  < 0) || (capability_value > ACCOUNT_CAPABILITY_ENABLED)) {
		ACCOUNT_SLOGE("(%s)-(%d) capability_value is not equal to 0 or 1.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_list_variant = NULL;
	bool is_success = account_manager_call_account_query_account_by_capability_sync(acc_mgr, account_db_path, capability_type, capability_value, &account_list_variant, NULL, &error);

	int error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	_INFO("before unmarshal_account_list");
	GSList* account_list = unmarshal_account_list(account_list_variant);
	_INFO("after unmarshal_account_list");
	if (account_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	GSList* iter;

	for (iter = account_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_list");
		account_s *account = NULL;
		account = (account_s*)iter->data;

		if (callback((account_h)account, user_data) == false)
		{
			_INFO("application callback requested to discontinue.");
			break;
		}
	}
	_INFO("account_query_account_by_capability end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_query_account_by_capability_type(account_cb callback, const char* capability_type, void* user_data)
{
	_INFO("account_query_account_by_capability_type starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((capability_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_list_variant = NULL;
	bool is_success = account_manager_call_account_query_account_by_capability_type_sync(acc_mgr, account_db_path, capability_type, &account_list_variant, NULL, &error);

	int error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	_INFO("before unmarshal_account_list");
	GSList* account_list = unmarshal_account_list(account_list_variant);
	_INFO("after unmarshal_account_list");
	if (account_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	GSList* iter;

	for (iter = account_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_list");
		account_s *account = NULL;
		account = (account_s*)iter->data;

		if (callback((account_h)account, user_data) == false)
		{
			_INFO("application callback requested to discontinue.");
			break;
		}
	}
	_INFO("account_query_account_by_capability end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_query_capability_by_account_id(capability_cb callback, int account_id, void *user_data)
{
	_INFO("account_query_capability_by_account_id starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((account_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GError *error = NULL;
	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* capability_list_variant = NULL;
	bool is_success = account_manager_call_account_query_capability_by_account_id_sync(acc_mgr, account_db_path, account_id, &capability_list_variant, NULL, &error);

	int error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	_INFO("before unmarshal_capability_list");
	GSList* capability_list = unmarshal_capability_list(capability_list_variant);
	_INFO("after unmarshal_capability_list");
	if (capability_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	GSList* iter;

	for (iter = capability_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_list");
		account_capability_s *capability = NULL;
		capability = (account_capability_s*)iter->data;
		_INFO("");
		if (callback(capability->type, capability->value, user_data) == false)
		{
			_INFO("application callback requested to discontinue.");
			break;
		}
		_INFO("");
	}
	_INFO("account_query_capability_by_account_id end");
	return ACCOUNT_ERROR_NONE;
}

static int _account_get_total_count(int *count, bool include_hidden)
{
	_INFO("account_get_total_count_from_db starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	if (!count)
	{
		ACCOUNT_SLOGE("(%s)-(%d) count is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	GError *error = NULL;

	if (count == NULL)
	{
		_INFO("Invalid input");
		return -1;
	}

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	int temp_count = -1;
	bool is_success = account_manager_call_account_get_total_count_from_db_sync(acc_mgr, account_db_path, include_hidden, &temp_count, NULL, &error);
	int error_code = _account_get_error_code(is_success, error);
	if (error_code != ACCOUNT_ERROR_NONE)
	{
		return error_code;
	}

	*count = temp_count;
	_INFO("account_get_total_count_from_db end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_get_total_count_from_db(int *count)
{
	_INFO("account_get_total_count_from_db starting");

	return _account_get_total_count(count, true);
}

ACCOUNT_API int account_get_total_count_from_db_ex(int *count)
{
	_INFO("account_get_total_count_from_db_ex starting");

	return _account_get_total_count(count, false);
}

ACCOUNT_API int account_update_sync_status_by_id(int account_db_id, const account_sync_state_e sync_status)
{
	_INFO("account_update_sync_status_by_id starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((account_db_id > 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT INDEX IS LESS THAN 0"));
	if ( ((int)sync_status < 0) || (sync_status > ACCOUNT_SYNC_STATUS_RUNNING)) {
		ACCOUNT_SLOGE("(%s)-(%d) sync_status is less than 0 or more than enum max.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	bool is_success = account_manager_call_account_update_sync_status_by_id_sync(acc_mgr, account_db_path, account_db_id, sync_status, NULL, &error);

	return _account_get_error_code(is_success, error);
}

static int _account_type_free_label_items(label_s *data)
{
	_ACCOUNT_FREE(data->app_id);
	_ACCOUNT_FREE(data->label);
	_ACCOUNT_FREE(data->locale);

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_gslist_free(GSList* list)
{
	ACCOUNT_RETURN_VAL((list != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("GSlist is NULL"));

	GSList* iter;

	for (iter = list; iter != NULL; iter = g_slist_next(iter)) {
		label_s *label_data = (label_s*)iter->data;
		_account_type_free_label_items(label_data);
		_ACCOUNT_FREE(label_data);
	}

	g_slist_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_item_free(account_type_s *data)
{
	_ACCOUNT_FREE(data->app_id);
	_ACCOUNT_FREE(data->service_provider_id);
	_ACCOUNT_FREE(data->icon_path);
	_ACCOUNT_FREE(data->small_icon_path);

	return ACCOUNT_ERROR_NONE;
}
/*
static int _account_type_glist_free(GList* list)
{
	ACCOUNT_RETURN_VAL((list != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Glist is NULL"));

	GList* iter;

	for (iter = list; iter != NULL; iter = g_list_next(iter)) {
		account_type_s *account_type_record = (account_type_s*)iter->data;
		_account_type_item_free(account_type_record);
		_ACCOUNT_FREE(account_type_record);
	}

	g_list_free(list);
	list = NULL;

	return ACCOUNT_ERROR_NONE;
}
*/
static int _account_type_free_account_type_items(account_type_s *data)
{
	_account_type_item_free(data);

	_account_type_gslist_free(data->label_list);
//	_account_type_gslist_free(data->provider_feature_list);
//	_account_type_glist_free(data->account_type_list);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_create(account_type_h *account_type)
{
	if (!account_type) {
		ACCOUNT_SLOGE("(%s)-(%d) account type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)malloc(sizeof(account_type_s));

	if (data == NULL)
	{
		ACCOUNT_ERROR("Memory Allocation Failed");
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}

	ACCOUNT_MEMSET(data, 0, sizeof(account_type_s));

	data->id = -1;
	data->app_id = NULL;
	data->service_provider_id = NULL;
	data->icon_path = NULL;
	data->small_icon_path = NULL;
	data->multiple_account_support = false;
	data->label_list = NULL;
//	data->account_type_list = NULL;
	data->provider_feature_list = NULL;

	*account_type = (account_type_h)data;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_destroy(account_type_h account_type)
{
	_INFO("account_type_destroy");

	account_type_s *data = (account_type_s*)account_type;

	if (data == NULL)
	{
		_ERR("Account type handle is null!");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	_account_type_free_account_type_items(data);
	_ACCOUNT_FREE(data);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_set_app_id(account_type_h account_type, const char *app_id)
{
	if (!account_type) {
		ACCOUNT_SLOGE("(%s)-(%d) account_type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!app_id) {
		ACCOUNT_SLOGE("(%s)-(%d) app_id is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	_ACCOUNT_FREE(data->app_id);
	data->app_id = _account_get_text(app_id);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_set_service_provider_id(account_type_h account_type, const char *service_provider_id)
{
	if (!account_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!service_provider_id) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	_ACCOUNT_FREE(data->service_provider_id);
	data->service_provider_id = _account_get_text(service_provider_id);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_set_icon_path(account_type_h account_type, const char *icon_path)
{
	if (!account_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	_ACCOUNT_FREE(data->icon_path);
	data->icon_path = _account_get_text(icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_set_small_icon_path(account_type_h account_type, const char *small_icon_path)
{
	if (!account_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!small_icon_path) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	_ACCOUNT_FREE(data->small_icon_path);
	data->small_icon_path = _account_get_text(small_icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_set_multiple_account_support(account_type_h account_type, const bool multiple_account_support)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account handle is NULL.\n",  __FUNCTION__, __LINE__));

	account_type_s *data = (account_type_s*)account_type;

	data->multiple_account_support = multiple_account_support;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_set_label(account_type_h account_type, const char* label, const char* locale)
{
	if (!account_type) {
		ACCOUNT_SLOGE("(%s)-(%d) account_type handle is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if(!label || !locale) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;
	label_s *label_data = (label_s*)malloc(sizeof(label_s));

	if (label_data == NULL) {
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}
	ACCOUNT_MEMSET(label_data, 0, sizeof(label_s));

	label_data->label = _account_get_text(label);
	label_data->locale = _account_get_text(locale);

	data->label_list = g_slist_append(data->label_list, (gpointer)label_data);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_set_provider_feature(account_type_h account_type, const char* provider_feature)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("account type handle is null"));
	ACCOUNT_RETURN_VAL((provider_feature != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("provider_feature is null"));

	account_type_s *data = (account_type_s*)account_type;

	GSList *iter = NULL;
	bool b_is_new = TRUE;

	for(iter = data->provider_feature_list; iter != NULL; iter = g_slist_next(iter)) {
		provider_feature_s *feature_data = NULL;
		feature_data = (provider_feature_s*)iter->data;

		if(!strcmp(feature_data->key, provider_feature)) {
			b_is_new = FALSE;
			break;
		}
	}

	if(b_is_new) {
		provider_feature_s* feature_data = (provider_feature_s*)malloc(sizeof(provider_feature_s));

		if (feature_data == NULL)
			return ACCOUNT_ERROR_OUT_OF_MEMORY;
		ACCOUNT_MEMSET(feature_data, 0, sizeof(provider_feature_s));

		feature_data->key = _account_get_text(provider_feature);
		data->provider_feature_list = g_slist_append(data->provider_feature_list, (gpointer)feature_data);
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_query_provider_feature_by_app_id(provider_feature_cb callback, const char* app_id, void *user_data )
{
	_INFO("account_type_query_provider_feature_by_app_id start");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GError *error = NULL;
	gint error_code = ACCOUNT_ERROR_NONE;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* feature_list_variant = NULL;
	bool is_success = account_manager_call_account_type_query_provider_feature_by_app_id_sync(acc_mgr, account_db_path, app_id, &feature_list_variant, NULL, &error);

	_INFO("account_manager_call_account_type_query_provider_feature_by_app_id_sync end=[%d]", is_success);

	if (!is_success)
	{
//		error_code = error->code;
		error_code = _account_get_error_code(is_success, error);
		_ERR("Account IPC call returned error[%d]", error_code);
		return error_code;
	}

	GSList* provider_feature_list = variant_to_provider_feature_list(feature_list_variant);
	if (provider_feature_list == NULL)
	{
		error_code = ACCOUNT_ERROR_NO_DATA;
		_ERR("[%d]", error_code);
		return error_code;
	}

	GSList *iter;
	for (iter = provider_feature_list; iter != NULL; iter = g_slist_next(iter)) {
		provider_feature_s *feature_data = NULL;

		feature_data = (provider_feature_s*)iter->data;

		if(callback(feature_data->app_id, feature_data->key, user_data)!=TRUE) {
			ACCOUNT_DEBUG("Callback func returs FALSE, its iteration is stopped!!!!\n");
			return ACCOUNT_ERROR_NONE;
		}
	}

	_INFO("account_type_query_provider_feature_by_app_id end");
	return error_code;
}

ACCOUNT_API bool account_type_query_supported_feature(const char* app_id, const char* capability)
{
	_INFO("account_type_query_supported_feature start");
	char* account_db_path = ACCOUNT_DB_PATH;

	if (app_id == NULL || capability == NULL)
	{
		set_last_result(ACCOUNT_ERROR_INVALID_PARAMETER);
		return false;
	}

	int is_supported = 0;
	GError *error = NULL;
	gint ret = ACCOUNT_ERROR_NONE;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		set_last_result(ACCOUNT_ERROR_PERMISSION_DENIED);
		return false;
	}

	bool is_success = account_manager_call_account_type_query_supported_feature_sync(acc_mgr, account_db_path, app_id, capability, &is_supported, NULL, &error);

	_INFO("account_manager_call_account_type_query_supported_feature_sync end=[%d]", is_success);

	if (!is_success)
	{
		ret = _account_get_error_code(is_success, error);
//		ret = error->code;
		_ERR("Account IPC call returned error[%d]", ret);
		set_last_result(ret);
		return false;
	}

	set_last_result(ACCOUNT_ERROR_NONE);
	_INFO("account_type_query_supported_feature end");
	return is_supported;
}

ACCOUNT_API int account_type_get_app_id(account_type_h account_type, char **app_id)
{
	if (!account_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!app_id) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	(*app_id) = NULL;
	*app_id = _account_get_text(data->app_id);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_service_provider_id(account_type_h account_type, char **service_provider_id)
{
	if (!account_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!service_provider_id) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	(*service_provider_id) = NULL;
	*service_provider_id = _account_get_text(data->service_provider_id);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_icon_path(account_type_h account_type, char **icon_path)
{
	if (!account_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!icon_path) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	(*icon_path) = NULL;
	*icon_path = _account_get_text(data->icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_small_icon_path(account_type_h account_type, char **small_icon_path)
{
	if (!account_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	if (!small_icon_path) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	(*small_icon_path) = NULL;
	*small_icon_path = _account_get_text(data->small_icon_path);

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_multiple_account_support(account_type_h account_type, int *multiple_account_support)
{
	if (!account_type) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (!multiple_account_support) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_type_s *data = (account_type_s*)account_type;

	*multiple_account_support = data->multiple_account_support;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_provider_feature_all(account_type_h account_type, provider_feature_cb callback, void* user_data)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_type_s *data = (account_type_s*)account_type;

	for (iter = data->provider_feature_list; iter != NULL; iter = g_slist_next(iter)) {
		provider_feature_s *feature_data = NULL;

		feature_data = (provider_feature_s*)iter->data;

		if(callback(feature_data->app_id, feature_data->key, user_data)!=TRUE) {
			ACCOUNT_DEBUG("Callback func returs FALSE, its iteration is stopped!!!!\n");
			return ACCOUNT_ERROR_NONE;
		}
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_get_label_by_locale(account_type_h account_type, const char* locale, char** label)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((label != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("INVALID PARAMETER"));

	GSList *iter;
	account_type_s *data = (account_type_s*)account_type;

	for (iter = data->label_list; iter != NULL; iter = g_slist_next(iter)) {
		label_s *label_data = NULL;

		label_data = (label_s*)iter->data;

		*label = NULL;

		if(!strcmp(locale, label_data->locale)) {
			*label = _account_get_text(label_data->label);
			return ACCOUNT_ERROR_NONE;
		}
		gchar** tokens = g_strsplit(locale, "-", 2);
		if(tokens != NULL) {
			if((char*)(tokens[1]) != NULL) {
				char* upper_token = g_ascii_strup(tokens[1], strlen(tokens[1]));
				if(upper_token != NULL) {
					char* converted_locale = g_strdup_printf("%s_%s", tokens[0], upper_token);
					if(!strcmp(converted_locale, label_data->locale)) {
						_ACCOUNT_FREE(converted_locale);
						_ACCOUNT_FREE(upper_token);
						g_strfreev(tokens);
						*label = _account_get_text(label_data->label);
						return ACCOUNT_ERROR_NONE;
					}
					_ACCOUNT_FREE(converted_locale);
				}
				_ACCOUNT_FREE(upper_token);
			}
		}
		g_strfreev(tokens);
	}

	return ACCOUNT_ERROR_RECORD_NOT_FOUND;
}

ACCOUNT_API int account_type_get_label(account_type_h account_type, account_label_cb callback, void *user_data)
{
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO CALLBACK FUNCTION"));

	GSList *iter;
	account_type_s *data = (account_type_s*)account_type;

	for (iter = data->label_list; iter != NULL; iter = g_slist_next(iter)) {
		label_s *label_data = NULL;

		label_data = (label_s*)iter->data;

		if(callback(label_data->app_id, label_data->label, label_data->locale, user_data)!=TRUE) {
			ACCOUNT_DEBUG("Callback func returs FALSE, its iteration is stopped!!!!\n");
			return ACCOUNT_ERROR_NONE;
		}
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_insert_to_db(account_type_h account_type, int* account_type_id)
{
	_INFO("account_type_insert_to_db starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((account_type_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE ID POINTER IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	int db_id = -1;
	GVariant* account_type_serialized = marshal_account_type((account_type_s*) account_type);
	bool is_success = account_manager_call_account_type_add_sync(acc_mgr, account_db_path, account_type_serialized, &db_id, NULL, &error);

	int ret = _account_get_error_code(is_success, error);
	if (ret != ACCOUNT_ERROR_NONE)
	{
		return ret;
	}

	_INFO("account_type_insert_to_db end id=[%d]", db_id);

	*account_type_id = db_id;

	account_type_s* account_type_data = (account_type_s*)account_type;
	account_type_data->id = db_id;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_INTERNAL_API int account_type_update_to_db_by_app_id(const account_type_h account_type, const char* app_id)
{
	_INFO("account_type_update_to_db_by_app_id starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("DATA IS NULL"));
	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_type_variant = marshal_account_type((account_type_s*) account_type);
	if (account_type_variant == NULL)
	{
		_ERR("Failed to serialize");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	bool is_success = account_manager_call_account_type_update_to_db_by_app_id_sync(acc_mgr, account_db_path, account_type_variant, app_id, NULL, &error);

	return _account_get_error_code(is_success, error);
}

ACCOUNT_INTERNAL_API int account_type_delete_by_app_id(const char* app_id)
{
	_INFO("account_type_delete_by_app_id starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	bool is_success = account_manager_call_account_type_delete_by_app_id_sync(acc_mgr, account_db_path, app_id, NULL, &error);

	return _account_get_error_code(is_success, error);
}

ACCOUNT_API int account_type_query_label_by_app_id(account_label_cb callback, const char* app_id, void *user_data )
{
	_INFO("account_type_query_label_by_app_id starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT Callback IS NULL"));
	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* label_list_variant = NULL;
	bool is_success = account_manager_call_account_type_query_label_by_app_id_sync(acc_mgr, account_db_path, app_id, &label_list_variant, NULL, &error);

	int ret = _account_get_error_code(is_success, error);
	if (ret != ACCOUNT_ERROR_NONE)
	{
		return ret;
	}

	_INFO("before variant_to_label_list");
	GSList* label_list = variant_to_label_list (label_list_variant);
	_INFO("after variant_to_label_list");
	if (label_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	GSList* iter;

	for (iter = label_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_list");
		label_s *label_record = NULL;
		label_record = (label_s*)iter->data;
		_INFO("");
		if (callback(label_record->app_id, label_record->label, label_record->locale, user_data) == false)
		{
			_INFO("application callback requested to discontinue.");
			break;
		}
		_INFO("");
	}
	_INFO("account_type_query_label_by_app_id end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_query_by_app_id(const char* app_id, account_type_h *account_type)
{
	_INFO("account_type_query_by_app_id starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((app_id != 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE'S POINTER IS NULL"));
	ACCOUNT_RETURN_VAL((*account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_type_variant = NULL;
	account_type_s *in_data = (account_type_s*) (*account_type);

	bool is_success = account_manager_call_account_type_query_by_app_id_sync(acc_mgr, account_db_path, app_id, &account_type_variant, NULL, &error);

	int ret = _account_get_error_code(is_success, error);
	if (ret != ACCOUNT_ERROR_NONE)
	{
		return ret;
	}

	account_type_s* received_account_type = umarshal_account_type (account_type_variant);
	ACCOUNT_RETURN_VAL((received_account_type != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("INVALID DATA RECEIVED FROM SVC"));

	in_data->id = received_account_type->id;
	in_data->app_id = _account_get_text(received_account_type->app_id);
	in_data->service_provider_id = _account_get_text(received_account_type->service_provider_id);
	in_data->icon_path = _account_get_text(received_account_type->icon_path);
	in_data->small_icon_path = _account_get_text(received_account_type->small_icon_path);
	in_data->multiple_account_support = received_account_type->multiple_account_support;
	in_data->label_list = received_account_type->label_list;
	in_data->provider_feature_list = received_account_type->provider_feature_list;

	_account_type_item_free(received_account_type);
	_ACCOUNT_FREE(received_account_type);
	_INFO("account_type_query_by_app_id end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_foreach_account_type_from_db(account_type_cb callback, void *user_data)
{
	_INFO("account_type_foreach_account_type_from_db starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT Callback IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_type_list_variant = NULL;
	_INFO("before account_type_query_all_sync()");
	bool is_success = account_manager_call_account_type_query_all_sync(acc_mgr, account_db_path, &account_type_list_variant, NULL, &error);

	_INFO("after account_type_query_all_sync()");
	int ret = _account_get_error_code(is_success, error);
	if (ret != ACCOUNT_ERROR_NONE)
	{
		return ret;
	}

	_INFO("before unmarshal_account_type_list");
	GSList* account_type_list = unmarshal_account_type_list(account_type_list_variant);
	_INFO("after unmarshal_account_type_list");
	if (account_type_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	GSList* iter;

	for (iter = account_type_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_list");
		account_type_s *account_type = NULL;
		account_type = (account_type_s*)iter->data;

		if (callback((account_type_h)account_type, user_data) == false)
		{
			_INFO("application callback requested to discontinue.");
			break;
		}
	}
	_INFO("account_type_foreach_account_type_from_db end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_query_label_by_locale(const char* app_id, const char* locale, char** label)
{
	_INFO("account_type_query_label_by_locale starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO APP ID"));
	ACCOUNT_RETURN_VAL((locale != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("NO LOCALE"));
	ACCOUNT_RETURN_VAL((label != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("label char is null"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	char* label_temp = NULL;
	_INFO("before account_type_query_label_by_locale_sync()");
	bool is_success = account_manager_call_account_type_query_label_by_locale_sync(acc_mgr, account_db_path, app_id, locale, &label_temp, NULL, &error);

	_INFO("after account_type_query_label_by_locale_sync() : is_success=%d", is_success);
	int ret = _account_get_error_code(is_success, error);
	if (ret != ACCOUNT_ERROR_NONE)
	{
		return ret;
	}

	if (label_temp == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	*label = _account_get_text(label_temp);
	_INFO("account_type_query_label_by_locale end");
	return ACCOUNT_ERROR_NONE;

}

ACCOUNT_API int account_type_query_by_provider_feature(account_type_cb callback, const char* key, void* user_data)
{
	_INFO("account_type_query_by_provider_feature starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((key != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("capability_type IS NULL"));
	ACCOUNT_RETURN_VAL((callback != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("CALL BACK IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	GVariant* account_type_list_variant = NULL;
	bool is_success = account_manager_call_account_type_query_by_provider_feature_sync(acc_mgr, account_db_path, key, &account_type_list_variant, NULL, &error);

	int ret = _account_get_error_code(is_success, error);
	if (ret != ACCOUNT_ERROR_NONE)
	{
		return ret;
	}

	_INFO("before unmarshal_account_type_list");
	GSList* account_type_list = unmarshal_account_type_list(account_type_list_variant);
	_INFO("after unmarshal_account_type_list");
	if (account_type_list == NULL)
	{
		return ACCOUNT_ERROR_NO_DATA;
	}

	GSList* iter;

	for (iter = account_type_list; iter != NULL; iter = g_slist_next(iter))
	{
		_INFO("iterating received account_type_list");
		account_type_s *account_type = NULL;
		account_type = (account_type_s*)iter->data;
		_INFO("");
		if (callback((account_type_h)account_type, user_data) == false)
		{
			_INFO("Application callback requested not to continue");
			break;
		}
		_INFO("");
	}
	_INFO("account_type_query_by_provider_feature end");
	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_type_query_app_id_exist(const char* app_id)
{
	_INFO("account_type_query_app_id_exist starting");
	char* account_db_path = ACCOUNT_DB_PATH;

	ACCOUNT_RETURN_VAL((app_id != 0), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));

	GError *error = NULL;

	AccountManager* acc_mgr = _account_manager_get_instance();
	if (acc_mgr == NULL)
	{
		_ERR("g_bus_get_sync failed");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	bool is_success = account_manager_call_account_type_query_app_id_exist_sync(acc_mgr, account_db_path, app_id, NULL, &error);

	return _account_get_error_code(is_success, error);
}


static void _account_subscribe_vconf_callback(keynode_t *key, void *user_data)
{
	account_subscribe_s* tmp = (account_subscribe_s*)user_data;
	char *msg = NULL, *vconf_key = NULL;
	const char *key_name = NULL;
	char event_msg[256] ={0, };
	int account_id = -1;

	if(!key) {
		ACCOUNT_ERROR("No subscribtion msg !!!!!\n");
		return;
	}

	if(!tmp) {
		ACCOUNT_ERROR("user data required\n");
		return;
	}

	key_name = vconf_keynode_get_name(key);

	if ( key_name == NULL ) {
		ACCOUNT_ERROR("vconf_keynode_get_name(key) fail\n");
		return;
	}

	if(!memcmp(key_name, VCONFKEY_ACCOUNT_MSG_STR, strlen(VCONFKEY_ACCOUNT_MSG_STR)))
	{
		vconf_key = vconf_keynode_get_str(key);

		if( vconf_key == NULL){
			ACCOUNT_ERROR("vconf key is NULL.\n");
			return;
		}
		msg = strdup(vconf_key);

		char* event_type = NULL;
		char* id = NULL;
		char* ptr = NULL;

		event_type = strtok_r(msg, ":", &ptr);
		id = strtok_r(NULL, ":", &ptr);

		ACCOUNT_SLOGD("msg(%s), event_type(%s), id(%s)", msg, event_type, id);

		ACCOUNT_SNPRINTF(event_msg,sizeof(event_msg),"%s", event_type);

		account_id = atoi(id);

		if(tmp->account_subscription_callback)
			tmp->account_subscription_callback(event_msg, account_id, tmp->user_data);
	}

	_ACCOUNT_FREE(msg);

}

ACCOUNT_API int account_subscribe_create(account_subscribe_h* account_subscribe)
{
	if (!account_subscribe) {
		ACCOUNT_SLOGE("account is NULL.\n", __FUNCTION__, __LINE__);
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	account_subscribe_s *data = (account_subscribe_s*)calloc(1,sizeof(account_subscribe_s));

	if(!data) {
		ACCOUNT_FATAL("OUT OF MEMORY\n");
		return ACCOUNT_ERROR_OUT_OF_MEMORY;
	}

	*account_subscribe = (account_subscribe_h)data;

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_subscribe_notification(account_subscribe_h account_subscribe, account_event_cb callback, void* user_data)
{
	ACCOUNT_RETURN_VAL((account_subscribe != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account subscribe handle is NULL.\n",	__FUNCTION__, __LINE__));

	account_subscribe_s* tmp =(account_subscribe_s*)account_subscribe;

	tmp->account_subscription_callback = callback;
	tmp->user_data = user_data;

	int ret = -1;
	ret = vconf_notify_key_changed(VCONFKEY_ACCOUNT_MSG_STR,
				(vconf_callback_fn)_account_subscribe_vconf_callback,
				(void*)tmp);

	ACCOUNT_SLOGI("vconf_notify_key_changed ret = %d", ret);

	if(ret != VCONF_OK) {
		ACCOUNT_ERROR("Vconf Subscription Failed ret = %d", ret);
		return ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL;
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_unsubscribe_notification(account_subscribe_h account_subscribe)
{
	ACCOUNT_RETURN_VAL((account_subscribe != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account subscribe handle is NULL.\n",	__FUNCTION__, __LINE__));

	account_subscribe_s* tmp =(account_subscribe_s*)account_subscribe;

	_ACCOUNT_FREE(tmp);

	if (vconf_ignore_key_changed(VCONFKEY_ACCOUNT_MSG_STR,
	   (vconf_callback_fn)_account_subscribe_vconf_callback) != 0) {
		ACCOUNT_ERROR("Vconf Subscription Failed !!!!!\n");
		return ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL;
	}

	return ACCOUNT_ERROR_NONE;
}

static void _account_subscribe_vconf_callback_ex(keynode_t *key, void *user_data)
{
	account_subscribe_s* tmp = (account_subscribe_s*)user_data;
	char *msg = NULL, *vconf_key = NULL;
	char event_msg[256] ={0, };
	int account_id = -1;
	const char *key_name = NULL;

	if(!key) {
		ACCOUNT_ERROR("No subscribtion msg !!!!!\n");
		return;
	}

	if(!tmp) {
		ACCOUNT_ERROR("user data required\n");
		return;
	}

	key_name = vconf_keynode_get_name(key);

	if ( key_name == NULL ) {
		ACCOUNT_ERROR("vconf_keynode_get_name(key) fail\n");
		return;
	}

	if(!memcmp(key_name, VCONFKEY_ACCOUNT_MSG_STR, strlen(VCONFKEY_ACCOUNT_MSG_STR)))
	{
		vconf_key = vconf_keynode_get_str(key);

		if( vconf_key == NULL){
			ACCOUNT_ERROR("vconf key is NULL.\n");
			return;
		}
		msg = strdup(vconf_key);

		char* event_type = NULL;
		char* id = NULL;
		char* ptr = NULL;

		event_type = strtok_r(msg, ":", &ptr);
		id = strtok_r(NULL, ":", &ptr);

		ACCOUNT_SLOGD("msg(%s), event_type(%s), id(%s)", msg, event_type, id);

		ACCOUNT_SNPRINTF(event_msg,sizeof(event_msg),"%s", event_type);

		account_id = atoi(id);

		if(tmp->account_subscription_callback)
			tmp->account_subscription_callback(event_msg, account_id, tmp->user_data);
	}

	_ACCOUNT_FREE(msg);

}

ACCOUNT_API int account_unsubscribe_notification_ex(account_subscribe_h account_subscribe)
{
	ACCOUNT_RETURN_VAL((account_subscribe != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account subscribe handle is NULL.\n",	__FUNCTION__, __LINE__));

	account_subscribe_s* tmp =(account_subscribe_s*)account_subscribe;

	_ACCOUNT_FREE(tmp);

	if (vconf_ignore_key_changed(VCONFKEY_ACCOUNT_MSG_STR,
	   (vconf_callback_fn)_account_subscribe_vconf_callback_ex) != 0) {
		ACCOUNT_ERROR("Vconf Subscription Failed !!!!!\n");
		return ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL;
	}

	return ACCOUNT_ERROR_NONE;
}

ACCOUNT_API int account_subscribe_notification_ex(account_subscribe_h account_subscribe, account_event_cb callback, void* user_data)
{
	ACCOUNT_RETURN_VAL((account_subscribe != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("(%s)-(%d) account subscribe handle is NULL.\n",	__FUNCTION__, __LINE__));

	account_subscribe_s* tmp =(account_subscribe_s*)account_subscribe;

	tmp->account_subscription_callback = callback;
	tmp->user_data = user_data;

	int ret = -1;
	ret = vconf_notify_key_changed(VCONFKEY_ACCOUNT_MSG_STR,
				(vconf_callback_fn)_account_subscribe_vconf_callback_ex,
				(void*)tmp);

	ACCOUNT_SLOGI("vconf_notify_key_changed ret = %d", ret);

	if(ret != VCONF_OK) {
		ACCOUNT_ERROR("Vconf Subscription Failed ret = %d", ret);
		return ACCOUNT_ERROR_EVENT_SUBSCRIPTION_FAIL;
	}

	return ACCOUNT_ERROR_NONE;
}
