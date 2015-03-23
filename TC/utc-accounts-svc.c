#include "assert.h"
#include <account.h>
#include <account_internal.h>
#include <account-types.h>
#include <account-error.h>
#include <string.h>
#include <glib.h>

#define TEST_PACKAGE_NAME               "core-accounts-svc-tests"	// app_id
#define USER_NAME						"user_name"
#define ICON_PATH						"/usr/share/icons/default/small/com.samsung.phone.png"

#define check_fail(exp) \
	if (!(exp)) { \
		_is_fail = true; \
		FILE *fp = fopen(ERR_LOG,"w"); \
			fprintf(fp, \
				"Assert fail in %s:%d\n", __FILE__, __LINE__); \
			fprintf(fp, \
				"Following expression is not true:\n" \
				"%s\n", #exp); \
			fclose(fp); \
		return false; \
	}

#define check_fail_eq(var, ref) \
	if (var != ref) { \
		FILE *fp = fopen(ERR_LOG,"w"); \
			fprintf(fp, \
				"Assert fail in %s:%d\n", __FILE__, __LINE__); \
			fprintf(fp, \
				"Values \"%s\" and \"%s\" are not equal:\n" \
				"%s == %d, %s == %d\n", \
				#var, #ref, #var, (int)var, #ref, (int)ref); \
			fclose(fp); \
		return false; \
	}


#define is_callback_fail() \
	if (_is_fail == true) \
		return 1;

//& set: AccountsSvc
static bool connected = false;
static bool created = false;
static account_h account = NULL;
static GMainLoop* mainloop = NULL;
static int _is_fail = true;
static const char* package_name = TEST_PACKAGE_NAME;
static const char* icon_path = ICON_PATH;
static const char* label_default = "AccountTest";
static const char* label_en_gb = "Application en-gb Test";
static const char* contact_capability = ACCOUNT_SUPPORTS_CAPABILITY_CONTACT;
static const char* calendar_capability = ACCOUNT_SUPPORTS_CAPABILITY_CALENDAR;
static const char* user_name = USER_NAME;
static const account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;

static gboolean timeout_cb(gpointer data) {
	printf("timeout!\n");
	_is_fail = true;
	g_main_loop_quit((GMainLoop*)data);
	return FALSE;
}

void utc_account_startup(void)
{
	account_type_h account_type = NULL;
	int account_type_id = -1;
	int ret1 = ACCOUNT_ERROR_NONE, ret2 = ACCOUNT_ERROR_NONE;
	ret1 = account_connect();
	if (ACCOUNT_ERROR_NONE == ret1) {
		connected = true;
		ret2 = account_create(&account);
		if (ACCOUNT_ERROR_NONE == ret2) {
			created = true;
		}
	}

	if(account_type_query_app_id_exist(package_name) == ACCOUNT_ERROR_RECORD_NOT_FOUND) {
		ret1 = account_type_create(&account_type);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_app_id_internal(account_type, package_name);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_service_provider_id_internal(account_type, "http://www.samsung.com/");
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_multiple_account_support_internal(account_type, true);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_label_internal(account_type, label_default, NULL);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_label_internal(account_type, label_en_gb, "en_GB");
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_icon_path_internal(account_type, icon_path);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_small_icon_path_internal(account_type, icon_path);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_provider_feature_internal(account_type, contact_capability);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_set_provider_feature_internal(account_type, calendar_capability);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_insert_to_db_internal(account_type, &account_type_id);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);

		ret1 = account_type_destroy(account_type);
		assert_eq(ret1, ACCOUNT_ERROR_NONE);
	}
}

void utc_account_cleanup(void)
{
	int ret1 = ACCOUNT_ERROR_NONE, ret2 = ACCOUNT_ERROR_NONE;
	ret1 = account_destroy(account);
	if (ACCOUNT_ERROR_NONE == ret1) {
		created = false;
		ret2 = account_disconnect();
		if (ACCOUNT_ERROR_NONE == ret2) {
			connected = false;
		}
	}
}

static void _account_free_text(char *text)
{
	if (text != NULL) {
		free(text);
		text = NULL;
	}
}

int utc_account_connect_p(void)
{
	assert(connected);

	return 0;
}


int utc_account_connect_n(void)
{
	/*
	   There is no way to test account_connect_n().
	 */
	return 0;
}

int utc_account_connect_readonly_p(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	if(connected)
	{
		ret = account_disconnect();
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	ret = account_connect_readonly();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_connect_readonly_n(void)
{
	/*
	   There is no way to test account_connect_readonly_n().
	 */
	return 0;
}

int utc_account_disconnect_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	ret = account_disconnect();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_connect();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_disconnect_n(void)
{
	/*
	   There is no way to test account_disconnect_n().
	 */
	return 0;
}

int utc_account_destroy_p(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	account_h account_data = NULL;
	assert(connected);

	ret = account_create(&account_data);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_destroy(account_data);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}


int utc_account_destroy_n(void)
{
	assert(connected);
	assert(created);
	utc_account_cleanup();
	int ret = ACCOUNT_ERROR_NONE;

	ret = rename("/opt/usr/dbspace/.account.db", "/opt/usr/dbspace/.account-tmp.db");

	ret = rename("/opt/usr/dbspace/.account.db-journal", "/opt/usr/dbspace/.account-tmp.db-journal");

	ret = account_destroy(NULL);

	assert_neq(ret, ACCOUNT_ERROR_NONE);

	ret = rename("/opt/usr/dbspace/.account-tmp.db", "/opt/usr/dbspace/.account.db");
	ret = rename("/opt/usr/dbspace/.account-tmp.db-journal", "/opt/usr/dbspace/.account.db-journal");

	utc_account_startup();

	return 0;
}


int utc_account_create_p(void)
{
	assert(connected);
	assert(created);

	return 0;
}

int utc_account_create_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_create(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_insert_to_db_p(void)
{
	assert(connected);
	assert(created);
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	char* ret_user_name = NULL;
	char* ret_package_name = NULL;
	const char* display_name = "display_name";
	char* ret_display_name = NULL;
	const char* icon_path = "icon_path";
	char* ret_icon_path = NULL;
	const char* domain_name = "domain_name";
	char* ret_domain_name = NULL;
	const char* email_address = "email_address";
	char* ret_email_address = NULL;
	const char* source = "source";
	char* ret_source = NULL;
	const char* key = "key";
	const char* value = "value";
	char* ret_value = NULL;
	const char* user_text = "user_text";
	char* ret_user_text = NULL;
	const char* access_token = "access_token";
	char* ret_access_token = NULL;
	const account_auth_type_e auth_type = ACCOUNT_AUTH_TYPE_OAUTH;
	const account_auth_type_e ret_auth_type = 0;
	const account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;
	const account_capability_state_e ret_capability_state = 0;
	const account_secrecy_state_e secrecy_state = ACCOUNT_SECRECY_VISIBLE;
	const account_secrecy_state_e ret_secrecy_state = 0;
	const account_sync_state_e sync_state = ACCOUNT_SYNC_STATUS_OFF;
	const account_sync_state_e ret_sync_state = 0;
	account_h ret_account = NULL;
	int i = -1;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_display_name(account, display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_icon_path(account, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_domain_name(account, domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_email_address(account, email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_source(account, source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_custom(account, key, value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_access_token(account, access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	for (i = 0; i < 5; i++){
		ret = account_set_user_text(account, i, user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	for (i = 0; i < 5; i++){
		ret = account_set_user_int(account, i, i*10);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	ret = account_set_auth_type(account, auth_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_secret(account, secrecy_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_sync_support(account, sync_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_destroy(account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_create(&ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_query_account_by_account_id(account_id, &ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_user_name(ret_account, &ret_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_user_name, user_name), 0);
	_account_free_text(ret_user_name);

	ret = account_get_package_name(ret_account, &ret_package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_package_name, package_name), 0);
	_account_free_text(ret_package_name);

	ret = account_get_display_name(ret_account, &ret_display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_display_name, display_name), 0);
	_account_free_text(ret_display_name);

	ret = account_get_icon_path(ret_account, &ret_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_icon_path, icon_path), 0);
	_account_free_text(ret_icon_path);

	ret = account_get_domain_name(ret_account, &ret_domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_domain_name, domain_name), 0);
	_account_free_text(ret_domain_name);

	ret = account_get_email_address(ret_account, &ret_email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_email_address, email_address), 0);
	_account_free_text(ret_email_address);

	ret = account_get_source(ret_account, &ret_source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_source, source), 0);
	_account_free_text(ret_source);

	ret = account_get_custom(ret_account, key, &ret_value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_value, value), 0);
	_account_free_text(ret_value);

	ret = account_get_capability(ret_account, contact_capability, &ret_capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(ret_capability_state, capability_state);

	ret = account_get_access_token(ret_account, &ret_access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_access_token, access_token), 0);
	_account_free_text(ret_access_token);

	for (i = 0; i < 5; i++){
		ret = account_get_user_text(ret_account, i, &ret_user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
		assert_eq(strcmp(ret_user_text, user_text), 0);
		_account_free_text(ret_user_text);
	}

	for (i = 0; i < 5; i++){
		int ret_user_int = -1;
		ret = account_get_user_int(ret_account, i, &ret_user_int);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
		assert_eq(ret_user_int, i*10);
	}

	ret = account_get_auth_type(ret_account, &ret_auth_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(ret_auth_type, auth_type);

	ret = account_get_secret(ret_account, &ret_secrecy_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(ret_secrecy_state, secrecy_state);

	ret = account_get_sync_support(ret_account, &ret_sync_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(ret_sync_state, sync_state);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_insert_to_db_n(void)
{
	assert(connected);
	assert(created);
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_insert_to_db(NULL, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_insert_to_db(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_display_name_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* display_name = "display_name";

	ret = account_set_display_name(account, display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_display_name_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* display_name = "display_name";

	ret = account_set_display_name(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_display_name(NULL, display_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_user_name_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_user_name_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_user_name(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_user_name(NULL, user_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_icon_path_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* icon_path = "icon_path";

	ret = account_set_icon_path(account, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_icon_path_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* icon_path = "icon_path";

	ret = account_set_icon_path(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_icon_path(NULL, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_domain_name_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* domain_name = "domain_name";

	ret = account_set_domain_name(account, domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_domain_name_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* domain_name = "domain_name";

	ret = account_set_domain_name(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_domain_name(NULL, domain_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_email_address_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* email_address = "email_address";

	ret = account_set_email_address(account, email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_email_address_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* email_address = "email_address";

	ret = account_set_email_address(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_email_address(NULL, email_address);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_source_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* source = "source";

	ret = account_set_source(account, source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_source_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* source = "source";

	ret = account_set_source(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_source(NULL, source);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_custom_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* key = "key";
	const char* value = "value";

	ret = account_set_custom(account, key, value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_custom_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* key = "key";
	const char* value = "value";

	ret = account_set_custom(account, NULL, value);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_custom(account, key, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_custom(NULL, key, value);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_custom_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* key = "key";
	const char* value = "value";
	char* ret_value = NULL;

	ret = account_set_custom(account, key, value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_custom(account, key, &ret_value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_value, value), 0);
	_account_free_text(ret_value);

    return 0;
}

int utc_account_get_custom_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	char* returned_val = NULL;

	ret = account_get_custom(account, NULL, &returned_val);
	free(returned_val);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

static bool account_custom_cb_func (char* key, char* value, void *user_data) {
	const char* original_value = (const char*)user_data;
	if(strcmp(value, original_value) == 0) {
		_is_fail = false;
	}
//	g_main_loop_quit(mainloop);
//	mainloop = NULL;

	return true;
}

int utc_account_get_custom_all_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* key = "key";
	const char* value = "value";

	ret = account_set_custom(account, key, value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);
	ret = account_get_custom_all(account, account_custom_cb_func, value);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);
//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	return 0;
}

int utc_account_get_custom_all_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_custom_all(account, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_custom_all(NULL, account_custom_cb_func, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_package_name_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_package_name_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_package_name(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_package_name(NULL, package_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_access_token_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* access_token = "access_token";

	ret = account_set_access_token(account, access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_access_token_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_access_token(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_auth_type_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_auth_type(account, ACCOUNT_AUTH_TYPE_OAUTH);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_auth_type_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_auth_type(account, -999);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_auth_type(NULL, ACCOUNT_AUTH_TYPE_OAUTH);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_secret_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_secret(account, ACCOUNT_SECRECY_INVISIBLE);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_secret_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_secret(account, -999);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_secret(NULL, ACCOUNT_SECRECY_INVISIBLE);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_sync_support_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_sync_support(account, ACCOUNT_SYNC_STATUS_IDLE);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_set_sync_support_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_sync_support(account, -999);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_sync_support(NULL, ACCOUNT_SYNC_STATUS_IDLE);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_user_text_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* user_text = "user_text";

	int i;
	for ( i = 0; i < 5; i++){
		ret = account_set_user_text(account, i, user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	return 0;
}

int utc_account_set_user_text_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const char* user_text = "user_text";

	int i;
	for (i = 0; i < 5; i++){
		ret = account_set_user_text(account, i, NULL);
		assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);
	}
	ret = account_set_user_text(account, 100, user_text);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_user_text(NULL, 1, user_text);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_user_int_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	int i;
	for (i = 0; i < 5; i++){
		ret = account_set_user_int(account, i, 999);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	return 0;
}

int utc_account_set_user_int_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	int i;
	for (i = 0; i < 5; i++){
		ret = account_set_user_int(NULL, i, 999);
		assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);
	}
	ret = account_set_user_int(account, 100, 999);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_set_capability_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_capability(account, contact_capability, ACCOUNT_CAPABILITY_ENABLED);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}


int utc_account_set_capability_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_capability(NULL, contact_capability, ACCOUNT_CAPABILITY_ENABLED);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_capability(account, NULL, ACCOUNT_CAPABILITY_ENABLED);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_set_capability(account, contact_capability, -1);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

static bool capability_cb_func (const char* capability_type, account_capability_state_e capability_state, void *user_data) {
	account_capability_state_e* _capability_state = (account_capability_state_e*)user_data;
	if(capability_state == *_capability_state)
		_is_fail = false;
//	g_main_loop_quit(mainloop);
//	mainloop = NULL;

	return true;
}

int utc_account_get_capability_all_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	const account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;

	ret = account_set_capability(account, contact_capability, capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);
	ret = account_get_capability_all(account, capability_cb_func, &capability_state);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);
//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	return 0;
}

int utc_account_get_capability_all_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_capability_all(NULL, capability_cb_func, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_capability_all(account, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_display_name_p(void)
{
	assert(connected);
	assert(created);
	const char* display_name = "display_name";
	char * ret_display_name = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_display_name(account, display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_display_name(account, &ret_display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_display_name, display_name), 0);
	_account_free_text(ret_display_name);

	return 0;
}

int utc_account_get_display_name_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	char * display_name;

	ret = account_get_display_name(NULL, &display_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_display_name(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_account_id_p(void)
{
	assert(connected);
	assert(created);
	int account_id = -1;
	int ret_account_id = -2;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_query_account_by_account_id(account_id, &account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_account_id(account, &ret_account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(account_id, ret_account_id);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_get_account_id_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;

	ret = account_get_account_id(NULL, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_account_id(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_user_name_p(void)
{
	assert(connected);
	assert(created);
	char *ret_user_name;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_user_name(account, &ret_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_user_name, user_name), 0);
	_account_free_text(ret_user_name);

	return 0;
}

int utc_account_get_user_name_n(void)
{
	assert(connected);
	assert(created);
	char *get_user_name;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_user_name(NULL, &get_user_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_user_name(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_icon_path_p(void)
{
	assert(connected);
	assert(created);
	char *ret_icon_path = NULL;
	const char* icon_path = "icon_path";
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_icon_path(account, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_icon_path(account, &ret_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_icon_path, icon_path), 0);
	_account_free_text(ret_icon_path);

	return 0;
}

int utc_account_get_icon_path_n(void)
{
	assert(connected);
	assert(created);
	char *ret_icon_path;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_icon_path(NULL, &ret_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_icon_path(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_domain_name_p(void)
{
	assert(connected);
	assert(created);
	char *ret_domain_name;
	const char* domain_name = "domain_name";
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_domain_name(account, domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_domain_name(account, &ret_domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_domain_name, domain_name), 0);
	_account_free_text(ret_domain_name);

	return 0;
}

int utc_account_get_domain_name_n(void)
{
	assert(connected);
	assert(created);
	char *domain_name;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_domain_name(NULL, &domain_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_domain_name(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_email_address_p(void)
{
	assert(connected);
	assert(created);
	char *ret_email_address;
	const char* email_address = "email_address";
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_email_address(account, email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_email_address(account, &ret_email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_email_address, email_address), 0);
	_account_free_text(ret_email_address);

	return 0;
}

int utc_account_get_email_address_n(void)
{
	assert(connected);
	assert(created);
	char *ret_email_address;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_email_address(NULL, &ret_email_address);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_email_address(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_source_p(void)
{
	assert(connected);
	assert(created);
	char *ret_source = NULL;
	const char* source = "source";
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_source(account, source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_source(account, &ret_source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_source, source), 0);
	_account_free_text(ret_source);

	return 0;
}

int utc_account_get_source_n(void)
{
	assert(connected);
	assert(created);
	char *ret_source = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_source(NULL, &ret_source);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_source(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_package_name_p(void)
{
	assert(connected);
	assert(created);
	char *ret_package_name = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_package_name(account, &ret_package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_package_name, package_name), 0);
	_account_free_text(ret_package_name);

	return 0;
}

int utc_account_get_package_name_n(void)
{
	assert(connected);
	assert(created);
	char *ret_package_name = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_package_name(NULL, &ret_package_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_package_name(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_access_token_p(void)
{
	assert(connected);
	assert(created);
	char *ret_access_token = NULL;
	const char* access_token = "access_token";
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_access_token(account, access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_access_token(account, &ret_access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(access_token, ret_access_token), 0);
	_account_free_text(ret_access_token);

	return 0;
}

int utc_account_get_access_token_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	char *ret_access_token = NULL;

	ret = account_get_access_token(NULL, &ret_access_token);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_access_token(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_auth_type_p(void)
{
	assert(connected);
	assert(created);
	account_auth_type_e auth_type;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_auth_type(account, ACCOUNT_AUTH_TYPE_XAUTH);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_auth_type(account, &auth_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(auth_type, ACCOUNT_AUTH_TYPE_XAUTH);

	return 0;
}

int utc_account_get_auth_type_n(void)
{
	assert(connected);
	assert(created);
	account_auth_type_e auth_type;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_auth_type(NULL, &auth_type);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_auth_type(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_secret_p(void)
{
	assert(connected);
	assert(created);
	account_secrecy_state_e secret;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_secret(account, ACCOUNT_SECRECY_INVISIBLE);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_secret(account, &secret);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(secret, ACCOUNT_SECRECY_INVISIBLE);

	return 0;
}

int utc_account_get_sync_support_p(void)
{
	assert(connected);
	assert(created);
	account_sync_state_e sync_support;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_sync_support(account, ACCOUNT_SYNC_STATUS_IDLE);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_sync_support(account, &sync_support);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(sync_support, ACCOUNT_SYNC_STATUS_IDLE);

	return 0;
}

int utc_account_get_secret_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	account_secrecy_state_e secret;

	ret = account_get_secret(NULL, &secret);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_secret(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_sync_support_n(void)
{
	assert(connected);
	assert(created);
	account_sync_state_e sync_support;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_sync_support(NULL, &sync_support);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_sync_support(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_user_text_p(void)
{
	assert(connected);
	assert(created);
	char *ret_user_text = NULL;
	const char* user_text = "user_text";
	int ret = ACCOUNT_ERROR_NONE;

	int i;
	for ( i = 0; i < 5; i++){
		ret = account_set_user_text(account, i, user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}
	for ( i = 0; i < 5; i++){
		ret = account_get_user_text(account, i, &ret_user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
		assert_eq(strcmp(ret_user_text, user_text), 0);
		_account_free_text(ret_user_text);
	}

	return 0;
}

int utc_account_get_user_text_n(void)
{
	assert(connected);
	assert(created);
	char *ret_user_text = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_user_text(NULL, 0, &ret_user_text);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_user_text(account, 100, &ret_user_text);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_user_text(account, 0, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_user_int_p(void)
{
	assert(connected);
	assert(created);
	int user_int;
	int ret = ACCOUNT_ERROR_NONE;

	int i;
	for (i = 0; i < 5; i++){
		ret = account_set_user_int(account, i, i*10);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	for (i = 0; i < 5; i++){
		ret = account_get_user_int(account, i, &user_int);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
		assert_eq(user_int, i*10);
	}

	return 0;
}

int utc_account_get_user_int_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_user_int(NULL, 0, 100);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_user_int(account, 100, 100);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_user_int(account, 0, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

static bool capability_call_back(const char* capability_type, account_capability_state_e capability_state, void *user_data)
{
	char* str = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	check_fail(user_data!=NULL);
	check_fail(capability_type!=NULL);
	account_capability_state_e _capability_state = ACCOUNT_CAPABILITY_ENABLED;
	account_h account_data = *((account_h*)user_data);
	const char* capability = contact_capability;

	ret = account_get_package_name(account_data, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, package_name)==0);

	ret = account_get_user_name(account_data, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, user_name)==0);

	check_fail(strcmp(capability_type, capability)==0);
	ret = account_get_capability(account_data, capability, &_capability_state);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(_capability_state == capability_state);

	_is_fail = false;

//	g_main_loop_quit(mainloop);
//	mainloop = NULL;

	return 0;
}

int utc_account_get_capability_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	account_capability_state_e value;

	ret = account_set_capability(account, contact_capability, ACCOUNT_CAPABILITY_ENABLED);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_capability(account, contact_capability, &value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(value, ACCOUNT_CAPABILITY_ENABLED);

	return 0;
}

int utc_account_get_capability_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	account_capability_state_e value;

	ret = account_get_capability(NULL, contact_capability, &value);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_capability(account, NULL, &value);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_get_capability(account, contact_capability, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

static bool account_call_back(account_h account, void *user_data)
{
	const char* key = "key";
	char* str = NULL;
	char* comparison_str = NULL;
	int value = -1;
	int comparison_value = -1;
	int ret = ACCOUNT_ERROR_NONE;
	int i = -1;
	check_fail(user_data!=NULL);
	account_h _account = *((account_h*)user_data);

	ret = account_get_package_name(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_package_name(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_user_name(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_user_name(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_display_name(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_display_name(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_icon_path(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_icon_path(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_domain_name(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_domain_name(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_email_address(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_email_address(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_email_address(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_email_address(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_source(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_source(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_custom(_account, key, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_custom(account, key, &comparison_str);
	check_fail_eq(ret, ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_capability(_account, contact_capability, &value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_capability(account, contact_capability, &comparison_value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(value == comparison_value);

	ret = account_get_access_token(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_access_token(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	for (i = 0; i < 5; i++){
		ret = account_get_user_text(_account, i, &str);
		check_fail(ret == ACCOUNT_ERROR_NONE);
		ret = account_get_user_text(account, i, &comparison_str);
		check_fail(ret == ACCOUNT_ERROR_NONE);
		check_fail(strcmp(str, comparison_str)==0);
		_account_free_text(str);
		_account_free_text(comparison_str);
	}

	for (i = 0; i < 5; i++){
		ret = account_get_user_int(_account, i, &value);
		check_fail(ret == ACCOUNT_ERROR_NONE);
		ret = account_get_user_int(account, i, &comparison_value);
		check_fail(ret == ACCOUNT_ERROR_NONE);
		check_fail(value == comparison_value);
	}

	ret = account_get_auth_type(_account, &value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_auth_type(account, &comparison_value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(value == comparison_value);

	ret = account_get_secret(_account, &value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_secret(account, &comparison_value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(value == comparison_value);

	ret = account_get_sync_support(_account, &value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_sync_support(account, &comparison_value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(value == comparison_value);

	_is_fail = false;

//	g_main_loop_quit(mainloop);
//	mainloop = NULL;

	return true;
}

int utc_account_foreach_account_from_db_p(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;
	char* ret_user_name = NULL;
	const char* display_name = "display_name";
	const char* icon_path = "icon_path";
	const char* domain_name = "domain_name";
	const char* email_address = "email_address";
	const char* source = "source";
	const char* key = "key";
	const char* value = "value";
	const char* user_text = "user_text";
	const char* access_token = "access_token";
	const account_auth_type_e auth_type = ACCOUNT_AUTH_TYPE_OAUTH;
	const account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;
	const account_secrecy_state_e secrecy_state = ACCOUNT_SECRECY_VISIBLE;
	const account_sync_state_e sync_state = ACCOUNT_SYNC_STATUS_OFF;
	int i = -1;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_display_name(account, display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_icon_path(account, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_domain_name(account, domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_email_address(account, email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_source(account, source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_custom(account, key, value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_access_token(account, access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	for (i = 0; i < 5; i++){
		ret = account_set_user_text(account, i, user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	for (i = 0; i < 5; i++){
		ret = account_set_user_int(account, i, i*10);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	ret = account_set_auth_type(account, auth_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_secret(account, secrecy_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_sync_support(account, sync_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);

	ret = account_foreach_account_from_db(account_call_back, &account);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	return 0;
}

int utc_account_foreach_account_from_db_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_foreach_account_from_db(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_query_account_by_account_id_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;
	account_h ret_account = NULL;
	char* ret_user_name = NULL;
	char* ret_package_name = NULL;
	account_capability_state_e ret_capability_type = 0;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, ACCOUNT_CAPABILITY_ENABLED);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_create(&ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_query_account_by_account_id(account_id, &ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	ret = account_get_user_name(ret_account, &ret_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	ret = account_get_package_name(ret_account, &ret_package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	ret = account_get_capability(ret_account, contact_capability, &ret_capability_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_user_name, user_name), 0);
	assert_eq(strcmp(ret_package_name, package_name), 0);
	assert_eq(ret_capability_type, ACCOUNT_CAPABILITY_ENABLED);
	_account_free_text(ret_user_name);
	_account_free_text(ret_package_name);
	ret = account_destroy(ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_query_account_by_account_id_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_query_account_by_account_id(-999, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_query_account_by_user_name_p(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;
	char* ret_user_name = NULL;
	const char* display_name = "display_name";
	const char* icon_path = "icon_path";
	const char* domain_name = "domain_name";
	const char* email_address = "email_address";
	const char* source = "source";
	const char* key = "key";
	const char* value = "value";
	const char* user_text = "user_text";
	const char* access_token = "access_token";
	const account_auth_type_e auth_type = ACCOUNT_AUTH_TYPE_OAUTH;
	const account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;
	const account_secrecy_state_e secrecy_state = ACCOUNT_SECRECY_VISIBLE;
	const account_sync_state_e sync_state = ACCOUNT_SYNC_STATUS_OFF;
	int i = -1;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_display_name(account, display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_icon_path(account, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_domain_name(account, domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_email_address(account, email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_source(account, source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_custom(account, key, value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_access_token(account, access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	for (i = 0; i < 5; i++){
		ret = account_set_user_text(account, i, user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	for (i = 0; i < 5; i++){
		ret = account_set_user_int(account, i, i*10);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	ret = account_set_auth_type(account, auth_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_secret(account, secrecy_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_sync_support(account, sync_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);

	ret = account_query_account_by_user_name(account_call_back, user_name, &account);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_query_account_by_user_name_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_query_account_by_user_name(account_call_back, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_query_account_by_user_name(NULL, user_name, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_query_account_by_package_name_p(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;
	char* ret_user_name = NULL;
	const char* display_name = "display_name";
	const char* icon_path = "icon_path";
	const char* domain_name = "domain_name";
	const char* email_address = "email_address";
	const char* source = "source";
	const char* key = "key";
	const char* value = "value";
	const char* user_text = "user_text";
	const char* access_token = "access_token";
	const account_auth_type_e auth_type = ACCOUNT_AUTH_TYPE_OAUTH;
	const account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;
	const account_secrecy_state_e secrecy_state = ACCOUNT_SECRECY_VISIBLE;
	const account_sync_state_e sync_state = ACCOUNT_SYNC_STATUS_OFF;
	int i = -1;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_display_name(account, display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_icon_path(account, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_domain_name(account, domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_email_address(account, email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_source(account, source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_custom(account, key, value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_access_token(account, access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	for (i = 0; i < 5; i++){
		ret = account_set_user_text(account, i, user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	for (i = 0; i < 5; i++){
		ret = account_set_user_int(account, i, i*10);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	ret = account_set_auth_type(account, auth_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_secret(account, secrecy_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_sync_support(account, sync_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);

	ret = account_query_account_by_package_name(account_call_back, package_name, &account);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_query_account_by_package_name_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_query_account_by_package_name(account_call_back, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_query_account_by_package_name(NULL, package_name, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_query_account_by_capability_p(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;
	char* ret_user_name = NULL;
	const char* display_name = "display_name";
	const char* icon_path = "icon_path";
	const char* domain_name = "domain_name";
	const char* email_address = "email_address";
	const char* source = "source";
	const char* key = "key";
	const char* value = "value";
	const char* user_text = "user_text";
	const char* access_token = "access_token";
	const account_auth_type_e auth_type = ACCOUNT_AUTH_TYPE_OAUTH;
	const account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;
	const account_secrecy_state_e secrecy_state = ACCOUNT_SECRECY_VISIBLE;
	const account_sync_state_e sync_state = ACCOUNT_SYNC_STATUS_OFF;
	int i = -1;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_display_name(account, display_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_icon_path(account, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_domain_name(account, domain_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_email_address(account, email_address);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_source(account, source);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_custom(account, key, value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_access_token(account, access_token);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	for (i = 0; i < 5; i++){
		ret = account_set_user_text(account, i, user_text);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	for (i = 0; i < 5; i++){
		ret = account_set_user_int(account, i, i*10);
		assert_eq(ret, ACCOUNT_ERROR_NONE);
	}

	ret = account_set_auth_type(account, auth_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_secret(account, secrecy_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_sync_support(account, sync_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);

	ret = account_query_account_by_capability(account_call_back, contact_capability, capability_state, &account);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_query_account_by_capability_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_query_account_by_capability(NULL, contact_capability, ACCOUNT_CAPABILITY_ENABLED, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_query_account_by_capability(account_call_back, NULL, ACCOUNT_CAPABILITY_ENABLED, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_query_account_by_capability(account_call_back, contact_capability, -1, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

static bool account_cb_func(account_h account, void *user_data) {
	char* str = NULL;
	char* comparison_str = NULL;
	int value = -1;
	int comparison_value = -1;
	int ret = ACCOUNT_ERROR_NONE;
	check_fail(user_data!=NULL);
	account_h _account = *((account_h*)user_data);

	ret = account_get_package_name(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_package_name(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_user_name(_account, &str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_user_name(account, &comparison_str);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(str, comparison_str)==0);
	_account_free_text(str);
	_account_free_text(comparison_str);

	ret = account_get_capability(_account, contact_capability, &value);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_get_capability(account, contact_capability, &comparison_value);
	check_fail_eq(ret, ACCOUNT_ERROR_NONE);
	check_fail(value == comparison_value);

	_is_fail = false;
//	g_main_loop_quit(mainloop);
//	mainloop = NULL;

	return true;
}

int utc_account_query_account_by_capability_type_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, ACCOUNT_CAPABILITY_ENABLED);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);

	ret = account_query_account_by_capability_type(account_cb_func, contact_capability, &account);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_query_account_by_capability_type_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	const char* type = "type";

	ret = account_query_account_by_capability_type(NULL, type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_query_account_by_capability_type(account_cb_func, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_query_capability_by_account_id_p(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;
	account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);

	ret = account_query_capability_by_account_id(capability_call_back, account_id, &account);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_query_capability_by_account_id_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_query_capability_by_account_id(capability_call_back, -1, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_query_capability_by_account_id(NULL, 1, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_get_total_count_from_db_p(void)
{
	assert(connected);
	int count = -1;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_total_count_from_db(&count);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_get_total_count_from_db_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_get_total_count_from_db(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_update_sync_status_by_id_p(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = -1;
	account_h ret_account = NULL;
	account_sync_state_e sync_state = -1;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_update_sync_status_by_id(account_id, ACCOUNT_SYNC_STATUS_IDLE);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_create(&ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_query_account_by_account_id(account_id, &ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_sync_support(ret_account, &sync_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(sync_state, ACCOUNT_SYNC_STATUS_IDLE);

	ret = account_destroy(ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_update_sync_status_by_id_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_update_sync_status_by_id(-1, ACCOUNT_SYNC_STATUS_IDLE);
	assert_neq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_delete_from_db_by_id_p(void)
{
	assert(connected);
	assert(created);
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_delete_from_db_by_id_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_delete_from_db_by_id(-1);
	assert_neq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_delete_from_db_by_user_name_p(void)
{
	assert(connected);
	assert(created);
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_user_name(user_name, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_delete_from_db_by_user_name_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_delete_from_db_by_user_name(NULL, package_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_delete_from_db_by_package_name_p(void)
{
	assert(connected);
	assert(created);

	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_package_name(package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_delete_from_db_by_package_name_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_delete_from_db_by_package_name(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_delete_from_db_by_package_name_without_permission_p(void)
{
	assert(connected);
	assert(created);

	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_package_name_without_permission(package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_delete_from_db_by_package_name_without_permission_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_delete_from_db_by_package_name_without_permission(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_update_to_db_by_id_p(void)
{
	assert(connected);
	assert(created);
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	char* update_user_name = "update_user_name";
	char* ret_user_name = NULL;
	account_h ret_account = NULL;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_user_name(account, update_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_update_to_db_by_id(account, account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_create(&ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_query_account_by_account_id(account_id, &ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_user_name(account, &ret_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_user_name, update_user_name), 0);

	_account_free_text(ret_user_name);
	ret = account_destroy(ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_update_to_db_by_id_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_update_to_db_by_id(account, -1);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_update_to_db_by_user_name_p(void)
{
	assert(connected);
	assert(created);
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	int value = -1;
	char* ret_user_name = NULL;
	account_h ret_account = NULL;
	account_capability_state_e capability_state = ACCOUNT_CAPABILITY_ENABLED;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_capability(account, contact_capability, capability_state);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_update_to_db_by_user_name(account, user_name, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_create(&ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_query_account_by_account_id(account_id, &ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_user_name(account, &ret_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_user_name, user_name), 0);
	_account_free_text(ret_user_name);

	ret = account_get_capability(account, contact_capability, &value);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(value, ACCOUNT_CAPABILITY_ENABLED);

	ret = account_destroy(ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_update_to_db_by_user_name_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_update_to_db_by_user_name(account, NULL, package_name);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}



int utc_account_subscribe_create_p(void)
{
	assert(connected);
	account_subscribe_h account_subscribe;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_subscribe_create(&account_subscribe);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_subscribe_notification(account_subscribe, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_unsubscribe_notification(account_subscribe);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}


int utc_account_subscribe_create_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_subscribe_create(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}


int utc_account_subscribe_notification_p(void)
{
	assert(connected);
	account_subscribe_h account_subscribe;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_subscribe_create(&account_subscribe);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_subscribe_notification(account_subscribe, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_unsubscribe_notification(account_subscribe);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}


int utc_account_subscribe_notification_n(void)
{
	assert(connected);
	account_subscribe_h account_subscribe;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_subscribe_create(&account_subscribe);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_subscribe_notification(NULL, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_subscribe_notification(account_subscribe, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_unsubscribe_notification(account_subscribe);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_unsubscribe_notification_p(void)
{
	assert(connected);
	account_subscribe_h account_subscribe;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_subscribe_create(&account_subscribe);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_subscribe_notification(account_subscribe, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_unsubscribe_notification(account_subscribe);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}


int utc_account_unsubscribe_notification_n(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_unsubscribe_notification(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_type_create_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_create_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_create(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_type_destroy_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_destroy_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_destroy(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_type_set_app_id_p(void)
{
	int ret = account_type_set_app_id(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_app_id_n(void)
{
	int ret = account_type_set_app_id(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_app_id_internal_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "application_id";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_app_id_internal_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "application_id";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(NULL, app_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_set_app_id_internal(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_app_id_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "application_id";
	char* ret_app_id = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_app_id(account_type, &ret_app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_app_id, app_id), 0);
	_account_free_text(ret_app_id);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_app_id_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	char* ret_app_id = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_app_id(NULL, &ret_app_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_app_id(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_icon_path_p(void)
{
	int ret = account_type_set_icon_path(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_icon_path_n(void)
{
	int ret = account_type_set_icon_path(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_icon_path_internal_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* icon_path = "icon_path";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_icon_path_internal(account_type, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_icon_path_internal_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* icon_path = "icon_path";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_icon_path_internal(NULL, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_set_icon_path_internal(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_icon_path_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	char* ret_icon_path = NULL;
	const char* icon_path = "icon_path";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_icon_path_internal(account_type, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_icon_path(account_type, &ret_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_icon_path, icon_path), 0);
	_account_free_text(ret_icon_path);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_icon_path_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	char* ret_icon_path = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_icon_path(NULL, &ret_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_icon_path(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_label_p(void)
{
	int ret = account_type_set_label(NULL, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_label_n(void)
{
	int ret = account_type_set_label(NULL, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_label_internal_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* label = "label";
	const char* locale = "locale";
	char* ret_label = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_label_internal_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* label = "label";
	const char* locale = "locale";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(NULL, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_set_label_internal(account_type, NULL, locale);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_set_label_internal(account_type, label, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_label_by_locale_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* label = "label";
	const char* locale = "locale";
	char* ret_label = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_label_by_locale(account_type, locale, &ret_label);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_label, label), 0);
	_account_free_text(ret_label);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_label_by_locale_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	char* ret_label = NULL;
	const char* locale = "locale";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_label_by_locale(NULL, locale, &ret_label);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_label_by_locale(account_type, locale, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

static bool account_label_cb_func(char* app_id, char* label, char* locale, void *user_data) {
	//this callback function checks that query function implements normally through confirmming whether same labels or not.
	if(user_data != NULL)
	{
		const char* _label = (const char*)user_data;
		if(strcmp(label, _label)==0)
			_is_fail = false;
	}

//	g_main_loop_quit(mainloop);
//	mainloop = NULL;
	return TRUE;
}

int utc_account_type_get_label_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* label = "label";
	const char* locale = "locale";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);

	ret = account_type_get_label(account_type, account_label_cb_func, (void*)label);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);
//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_label_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_label(NULL, account_label_cb_func, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_label(account_type, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_multiple_account_support_p(void)
{
	int ret = account_type_set_multiple_account_support(NULL, true);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_multiple_account_support_n(void)
{
	int ret = account_type_set_multiple_account_support(NULL, true);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_multiple_account_support_internal_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	int ret_val = -1;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_multiple_account_support_internal(account_type, true);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_multiple_account_support_internal_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_set_multiple_account_support_internal(NULL, true);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_type_get_multiple_account_support_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	int ret_val = 0;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_multiple_account_support_internal(account_type, true);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_multiple_account_support(account_type, &ret_val);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(ret_val, true);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_multiple_account_support_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	bool ret_val = true;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_multiple_account_support(NULL, &ret_val);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_multiple_account_support(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

static bool provider_feature_cb_func(char* app_id, char* key, void* user_data) {
	//this callback function checks that query function implements normally through confirmming whether same provider feature or not.
	if(user_data != NULL)
	{
		const char* _capability = (const char*)user_data;
		check_fail(strcmp(key, _capability)==0)
	}
//	g_main_loop_quit(mainloop);
//	mainloop = NULL;
	_is_fail = false;

	return TRUE;
}

int utc_account_type_set_provider_feature_p(void)
{
	int ret = account_type_set_provider_feature(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return TRUE;
}

int utc_account_type_set_provider_feature_n(void)
{
	int ret = account_type_set_provider_feature(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return TRUE;
}

int utc_account_type_set_provider_feature_internal_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* provider_feature = "provider_feature";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, provider_feature);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_provider_feature_internal_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* provider_feature = "provider_feature";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(NULL, provider_feature);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_set_provider_feature_internal(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_provider_feature_all_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* provider_feature = "provider_feature";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, provider_feature);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

//	mainloop = g_main_loop_new(NULL, FALSE);
//	assert(mainloop);

	ret = account_type_get_provider_feature_all(account_type, provider_feature_cb_func, (void *)provider_feature);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);
//	int timeout_id = g_timeout_add(3000, timeout_cb, mainloop);
//	g_main_loop_run(mainloop);
//	g_source_remove(timeout_id);
	_is_fail = true;

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_provider_feature_all_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_provider_feature_all(NULL, provider_feature_cb_func, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_provider_feature_all(account_type, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_service_provider_id_p(void)
{
	int ret = account_type_set_service_provider_id(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_service_provider_id_n(void)
{
	int ret = account_type_set_service_provider_id(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_service_provider_id_internal_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* service_provider_id = "service_provider_id";
	char* ret_service_provider_id = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_service_provider_id_internal_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* service_provider_id = "service_provider_id";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(NULL, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_set_service_provider_id_internal(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_service_provider_id_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* service_provider_id = "service_provider_id";
	char* ret_service_provider_id = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_service_provider_id(account_type, &ret_service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_service_provider_id, service_provider_id), 0);
	_account_free_text(ret_service_provider_id);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_service_provider_id_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	char* ret_service_provider_id = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_service_provider_id(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_service_provider_id(NULL, &ret_service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_small_icon_path_p(void)
{
	int ret = account_type_set_small_icon_path(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_small_icon_path_n(void)
{
	int ret = account_type_set_small_icon_path(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_small_icon_path_internal_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* small_icon_path = "small_icon_path";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_small_icon_path_internal(account_type, small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_set_small_icon_path_internal_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* small_icon_path = "small_icon_path";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_small_icon_path_internal(NULL, small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_set_small_icon_path_internal(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_small_icon_path_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	char* ret_small_icon_path = NULL;
	const char* small_icon_path = "small_icon_path";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_small_icon_path_internal(account_type, small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_small_icon_path(account_type, &ret_small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_small_icon_path, small_icon_path), 0);
	_account_free_text(ret_small_icon_path);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_get_small_icon_path_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	char* ret_small_icon_path = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_small_icon_path(NULL, &ret_small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_small_icon_path(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_insert_to_db_p(void)
{
	int ret = account_type_insert_to_db(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_insert_to_db_n(void)
{
	int ret = account_type_insert_to_db(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_insert_to_db_internal_p(void)
{
	account_type_h account_type = NULL;
	account_type_h ret_account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	int account_type_id = -1;
	const char* app_id = "app_id_insert_test";
	char* ret_app_id = NULL;
	const char* service_provider_id = TEST_PACKAGE_NAME;
	char* ret_service_provider_id = NULL;
	const char* icon_path = "icon_path";
	char* ret_icon_path = NULL;
	const char* small_icon_path = "small_icon_path";
	char* ret_small_icon_path = NULL;
	const bool multi_account_support = true;
	bool ret_multi_account_support = false;
	const char* label = "label";
	char* ret_label = NULL;
	const char* locale = "locale";
	char* ret_capability = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_icon_path_internal(account_type, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_small_icon_path_internal(account_type, small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_multiple_account_support_internal(account_type, multi_account_support);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, contact_capability);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &account_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_create(&ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_by_app_id(app_id, &ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_app_id(ret_account_type, &ret_app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_app_id, app_id), 0);
	_account_free_text(ret_app_id);

	ret = account_type_get_service_provider_id(ret_account_type, &ret_service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_service_provider_id, service_provider_id), 0);
	_account_free_text(ret_service_provider_id);

	ret = account_type_get_icon_path(ret_account_type, &ret_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_icon_path, icon_path), 0);
	_account_free_text(ret_icon_path);

	ret = account_type_get_small_icon_path(account_type, &small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_small_icon_path, small_icon_path), 0);
	_account_free_text(ret_small_icon_path);

	ret = account_type_get_multiple_account_support(account_type, &ret_multi_account_support);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(ret_multi_account_support, multi_account_support);

	ret = account_type_get_label_by_locale(account_type, locale, &ret_label);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_label, label), 0);
	_account_free_text(ret_label);

	ret = account_type_get_provider_feature_all(account_type, provider_feature_cb_func, (void *)contact_capability);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	_is_fail = true;

	ret = account_type_destroy(ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_insert_to_db_internal_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	int account_type_id = 0;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(NULL, &account_type_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_insert_to_db_internal(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_update_to_db_by_app_id_p(void)
{
	int ret = account_type_update_to_db_by_app_id(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_update_to_db_by_app_id_n(void)
{
	int ret = account_type_update_to_db_by_app_id(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_update_to_db_by_app_id_internal_p(void)
{
	account_type_h account_type = NULL;
	account_type_h ret_account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	int account_type_id = 0;
	const char* app_id = "account_type_update_by_app_id";
	char* ret_app_id = NULL;
	const char* service_provider_id = TEST_PACKAGE_NAME;
	char* ret_service_provider_id = NULL;
	const char* icon_path = "icon_path";
	const char* update_icon_path = "update_icon_path";
	char* ret_icon_path = NULL;
	const char* small_icon_path = "small_icon_path";
	const char* update_small_icon_path = "update_small_icon_path";
	char* ret_small_icon_path = NULL;
	const bool multi_account_support = true;
	const bool update_multi_account_support = false;
	bool ret_multi_account_support = true;
	const char* label = "label";
	const char* update_label = "update_label";
	char* ret_label = NULL;
	const char* locale = "locale";
	char* ret_capability = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_create(&ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_icon_path_internal(account_type, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_small_icon_path_internal(account_type, small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_multiple_account_support_internal(account_type, multi_account_support);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, contact_capability);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &account_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_by_app_id(app_id, &account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_icon_path_internal(account_type, update_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_small_icon_path_internal(account_type, update_small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_multiple_account_support_internal(account_type, update_multi_account_support);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, update_label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, calendar_capability);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_update_to_db_by_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_by_app_id(app_id, &ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_app_id(ret_account_type, &ret_app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_app_id, app_id), 0);
	_account_free_text(ret_app_id);

	ret = account_type_get_service_provider_id(ret_account_type, &ret_service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_service_provider_id, service_provider_id), 0);
	_account_free_text(ret_service_provider_id);

	ret = account_type_get_icon_path(ret_account_type, &ret_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_icon_path, update_icon_path), 0);
	_account_free_text(ret_icon_path);

	ret = account_type_get_small_icon_path(ret_account_type, &ret_small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_small_icon_path, update_small_icon_path), 0);
	_account_free_text(ret_small_icon_path);

	ret = account_type_get_multiple_account_support(ret_account_type, &ret_multi_account_support);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(ret_multi_account_support, update_multi_account_support);

	ret = account_type_get_label_by_locale(account_type, locale, &ret_label);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_label, update_label), 0);
	_account_free_text(ret_label);

	ret = account_type_get_provider_feature_all(ret_account_type, provider_feature_cb_func, (void *)calendar_capability);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	_is_fail = true;

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_update_to_db_by_app_id_internal_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "small_icon_path";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_update_to_db_by_app_id_internal(NULL, app_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_update_to_db_by_app_id_internal(account_type, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_query_by_app_id_p(void)
{
	account_type_h account_type = NULL;
	account_type_h ret_account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "account_type_query_by_app_id";
	char* ret_app_id = NULL;
	const char* service_provider_id = TEST_PACKAGE_NAME;
	char* ret_service_provider_id = NULL;
	int account_type_id = 0;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_create(&ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &account_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_by_app_id(app_id, &ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_get_app_id(ret_account_type, &ret_app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_app_id, app_id), 0);
	_account_free_text(ret_app_id);

	ret = account_type_get_service_provider_id(ret_account_type, &ret_service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_service_provider_id, service_provider_id), 0);
	_account_free_text(ret_service_provider_id);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(ret_account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}


int utc_account_type_query_by_app_id_n(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "account_type_query_by_app_id";

	ret = account_type_query_by_app_id(app_id, &account_type);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_by_app_id(NULL, &account_type);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_query_by_app_id(app_id, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}


int utc_account_type_query_app_id_exist_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "account_type_query_app_id_exist";
	int a_type_id = -1;

	ret = account_type_query_app_id_exist(app_id);
	assert_eq(ret, ACCOUNT_ERROR_RECORD_NOT_FOUND);

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &a_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_app_id_exist(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_query_app_id_exist_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_query_app_id_exist(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

static bool account_type_for_query_by_provider_cb(account_type_h account_type, void *user_data)
{
	//this callback function checks that query function implements normally through confirmming whether same app_id or not.
	int ret = ACCOUNT_ERROR_NONE;
	account_type_h ret_account_type = NULL;
	char* app_id = NULL;
	char* ret_app_id = NULL;
	char* service_provider_id = NULL;
	char* ret_service_provider_id = NULL;
	char* icon_path = NULL;
	char* ret_icon_path = NULL;
	char* small_icon_path = NULL;
	char* ret_small_icon_path = NULL;
	bool multi_account_support = true;
	char* label = NULL;
	char* ret_label = NULL;
	const char* locale = "locale";
	char* capability = "capability";
	bool ret_multi_account_support = false;

	check_fail(account_type != NULL && user_data != NULL && user_data != NULL);

	ret_account_type = *((account_type_h*)user_data);

	ret = account_type_get_app_id(account_type, &app_id);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_app_id(ret_account_type, &ret_app_id);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_app_id, app_id)==0);
	_account_free_text(app_id);
	_account_free_text(ret_app_id);

	ret = account_type_get_service_provider_id(account_type, &service_provider_id);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_service_provider_id(ret_account_type, &ret_service_provider_id);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_service_provider_id, service_provider_id)==0);
	_account_free_text(service_provider_id);
	_account_free_text(ret_service_provider_id);

	ret = account_type_get_icon_path(account_type, &icon_path);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_icon_path(ret_account_type, &ret_icon_path);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_icon_path, icon_path)==0);
	_account_free_text(icon_path);
	_account_free_text(ret_icon_path);

	ret = account_type_get_small_icon_path(account_type, &small_icon_path);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_small_icon_path(ret_account_type, &ret_small_icon_path);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_small_icon_path, small_icon_path)==0);
	_account_free_text(small_icon_path);
	_account_free_text(ret_small_icon_path);

	ret = account_type_get_multiple_account_support(account_type, &multi_account_support);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_multiple_account_support(ret_account_type, &ret_multi_account_support);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(ret_multi_account_support == multi_account_support);

	ret = account_type_get_label_by_locale(account_type, locale, &label);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_label_by_locale(account_type, locale, &ret_label);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_label, label) == 0);
	_account_free_text(label);
	_account_free_text(ret_label);

	_is_fail = true;
	ret = account_type_get_provider_feature_all(account_type, provider_feature_cb_func,  capability);
	check_fail(_is_fail == false);
	check_fail(ret == ACCOUNT_ERROR_NONE);

	_is_fail = true;
	ret = account_type_get_provider_feature_all(ret_account_type, provider_feature_cb_func, capability);
	check_fail(_is_fail == false);
	check_fail(ret == ACCOUNT_ERROR_NONE);

	_is_fail = false;

//	g_main_loop_quit(mainloop);
//	mainloop = NULL;

    return true;
}


int utc_account_type_query_by_provider_feature_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "account_type_query_by_provider_feature";
	const char* service_provider_id = TEST_PACKAGE_NAME;
	const char* icon_path = "icon_path";
	const char* small_icon_path = "small_icon_path";
	const char* capability = "capability";
	const bool multi_account_support = true;
	const char* label = "label";
	const char* locale = "locale";

	int a_type_id = -1;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_icon_path_internal(account_type, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_small_icon_path_internal(account_type, small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_multiple_account_support_internal(account_type, multi_account_support);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, capability);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &a_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_by_provider_feature(account_type_for_query_by_provider_cb, capability, &account_type);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	_is_fail = true;

	account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}


int utc_account_type_query_by_provider_feature_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	const char* key = contact_capability;

	ret = account_type_query_by_provider_feature(account_type_for_query_by_provider_cb, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_query_by_provider_feature(NULL, key, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

static bool account_type_for_query_by_app_id_cb(char* app_id, char* label, char* locale, void *user_data)
{
	//this callback function checks that query function implements normally through confirmming whether same label or not.
	int ret = ACCOUNT_ERROR_NONE;
	account_type_h account_type = NULL;
	char* ret_label = NULL;

	check_fail( app_id!=NULL && label!=NULL && locale!=NULL && user_data!=NULL );
	account_type = *((account_type_h*)user_data);

	ret = account_type_get_label_by_locale(account_type, locale, &ret_label);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_label, label) == 0);
	_account_free_text(ret_label);

	_is_fail = false;

	return TRUE;
}

int utc_account_type_query_label_by_app_id_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	int a_type_id = -1;

	const char* label_t = "label_test";
	const char* locale_t = "locale_test";
	const char* app_id = "account_type_query_label_by_app_id";
	const char* service_provider_id = TEST_PACKAGE_NAME;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label_t, locale_t);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &a_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_label_by_app_id(account_type_for_query_by_app_id_cb, app_id, &account_type);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	_is_fail = true;

	account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_query_label_by_app_id_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "account_type_query_label_by_app_id";

	ret = account_type_query_label_by_app_id(NULL, app_id, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_query_label_by_app_id(account_type_for_query_by_app_id_cb, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_type_query_label_by_locale_p(void)
{
	account_type_h account_type = NULL;
	int ret = ACCOUNT_ERROR_NONE;
	const char* label = "label";
	const char* locale = "locale";
	const char* app_id = "account_type_query_label_by_locale";
	char* ret_label = NULL;
	int a_type_id = -1;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &a_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_label_by_locale(app_id, locale, &ret_label);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_label, label), 0);
	_account_free_text(ret_label);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_query_label_by_locale_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "account_type_query_label_by_locale";
	const char* locale = "locale";
	char* ret_label = NULL;

	ret = account_type_get_label_by_locale(NULL, locale, &ret_label);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_get_label_by_locale(app_id, locale, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_type_query_provider_feature_by_app_id_p(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	account_type_h account_type = NULL;
	const char* app_id = "application_id";
	const char* provider_feature = "provider_feature";
	int account_type_id = -1;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, provider_feature);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &account_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_provider_feature_by_app_id(provider_feature_cb_func, app_id, (void *)provider_feature);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	_is_fail = true;

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_query_provider_feature_by_app_id_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;
	const char* app_id = "application_id";

	ret = account_type_query_provider_feature_by_app_id(NULL, app_id, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_query_provider_feature_by_app_id(provider_feature_cb_func, NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_type_query_supported_feature_p(void)
{
	account_type_h account_type = NULL;
	bool ret = true;
	int account_type_id = -1;
	const char* app_id = "application_id";
	const char* capability = "capability";

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, capability);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &account_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_supported_feature(app_id, capability);
	assert_eq(ret, true);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_query_supported_feature_n(void)
{
	bool ret = true;
	const char* app_id = "application_id";
	const char* capability = "capability";

	ret = account_type_query_supported_feature(NULL, capability);
	assert_eq(ret, FALSE);
	assert_eq(get_last_result(), ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_type_query_supported_feature(app_id, NULL);
	assert_eq(ret, FALSE);

	return 0;
}

int utc_account_update_to_db_by_id_ex_p(void)
{
	assert(connected);
	assert(created);
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	char* update_user_name = "update_user_name";
	char* ret_user_name = NULL;
	account_h ret_account = NULL;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_user_name(account, update_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_update_to_db_by_id_ex(account, account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_create(&ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_query_account_by_account_id(account_id, &ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_user_name(account, &ret_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_user_name, update_user_name), 0);

	_account_free_text(ret_user_name);
	ret = account_destroy(ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_update_to_db_by_id_ex_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_update_to_db_by_id_ex(account, -1);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_update_to_db_by_id_without_permission_p(void)
{
	assert(connected);
	assert(created);
	account_h ret_account = NULL;
	int account_id = -1;
	int ret = ACCOUNT_ERROR_NONE;
	const char* update_user_name = "update_user_name";
	char* ret_user_name = NULL;

	ret = account_set_user_name(account, user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_insert_to_db(account, &account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_user_name(account, update_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_set_package_name(account, package_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_update_to_db_by_id_without_permission(account, account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_create(&ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_query_account_by_account_id(account_id, &ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_get_user_name(ret_account, &ret_user_name);
	assert_eq(ret, ACCOUNT_ERROR_NONE);
	assert_eq(strcmp(ret_user_name, update_user_name), 0);
	_account_free_text(ret_user_name);

	ret = account_destroy(ret_account);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_delete_from_db_by_id(account_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_update_to_db_by_id_without_permission_n(void)
{
	assert(connected);
	assert(created);
	int ret = ACCOUNT_ERROR_NONE;
	int account_id = 2;

	ret = account_update_to_db_by_id_without_permission(NULL, account_id);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	ret = account_update_to_db_by_id_without_permission(account, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

int utc_account_type_delete_by_app_id_p(void)
{
	int ret = account_type_delete_by_app_id(NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_delete_by_app_id_n(void)
{
	int ret = account_type_delete_by_app_id(NULL);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_delete_by_app_id_internal_p(void)
{
	account_type_h account_type = NULL;
	const char* app_id = "app_id_delete_test";
	int ret = ACCOUNT_ERROR_NONE;
	int account_type_id = -1;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &account_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_app_id_exist(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_query_app_id_exist(app_id);
	assert_eq(ret, ACCOUNT_ERROR_RECORD_NOT_FOUND);

	return 0;
}

int utc_account_type_delete_by_app_id_internal_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_delete_by_app_id_internal(NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

static bool account_type_cb_func(account_type_h account_type, void *user_data) {
	//this callback function checks that query function implements normally through confirmming whether same app_id or not.
	int ret = ACCOUNT_ERROR_NONE;
	account_type_h ret_account_type = NULL;
	char* app_id = NULL;
	char* ret_app_id = NULL;
	char* service_provider_id = NULL;
	char* ret_service_provider_id = NULL;
	char* icon_path = NULL;
	char* ret_icon_path = NULL;
	char* small_icon_path = NULL;
	char* ret_small_icon_path = NULL;
	bool multi_account_support = true;
	char* label = NULL;
	char* ret_label = NULL;
	const char* locale = "locale";
	char* capability = NULL;
	char* ret_capability = NULL;
	bool ret_multi_account_support = false;

	check_fail(account_type != NULL && user_data != NULL && user_data != NULL);

	ret_account_type = *((account_type_h*)user_data);

	ret = account_type_get_app_id(account_type, &app_id);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	if( strcmp(app_id, TEST_PACKAGE_NAME)==0 )
	{
		_is_fail=false;
		return true;
	}
	ret = account_type_get_app_id(ret_account_type, &ret_app_id);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_app_id, app_id)==0);
	_account_free_text(app_id);
	_account_free_text(ret_app_id);

	ret = account_type_get_service_provider_id(account_type, &service_provider_id);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_service_provider_id(ret_account_type, &ret_service_provider_id);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_service_provider_id, service_provider_id)==0);
	_account_free_text(service_provider_id);
	_account_free_text(ret_service_provider_id);

	ret = account_type_get_icon_path(account_type, &icon_path);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_icon_path(ret_account_type, &ret_icon_path);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_icon_path, icon_path)==0);
	_account_free_text(icon_path);
	_account_free_text(ret_icon_path);

	ret = account_type_get_small_icon_path(account_type, &small_icon_path);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_small_icon_path(ret_account_type, &ret_small_icon_path);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_small_icon_path, small_icon_path)==0);
	_account_free_text(small_icon_path);
	_account_free_text(ret_small_icon_path);

	ret = account_type_get_multiple_account_support(account_type, &multi_account_support);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_multiple_account_support(ret_account_type, &ret_multi_account_support);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(ret_multi_account_support == multi_account_support);

	ret = account_type_get_label_by_locale(account_type, locale, &label);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	ret = account_type_get_label_by_locale(account_type, locale, &ret_label);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(strcmp(ret_label, label) == 0);
	_account_free_text(label);
	_account_free_text(ret_label);

	_is_fail = true;
	ret = account_type_get_provider_feature_all(account_type, provider_feature_cb_func, (void *)contact_capability);

	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(_is_fail == false);

	_is_fail = true;
	ret = account_type_get_provider_feature_all(account_type, provider_feature_cb_func, (void *)contact_capability);
	check_fail(ret == ACCOUNT_ERROR_NONE);
	check_fail(_is_fail == false);

	_is_fail = false;

//	g_main_loop_quit(mainloop);
//	mainloop = NULL;

    return true;
}

int utc_account_type_foreach_account_type_from_db_p(void)
{
	assert(connected);
	int ret = ACCOUNT_ERROR_NONE;
	int account_type_id = -1;
	account_type_h account_type = NULL;
	const char* app_id = "account_type_foreach_account_type_from_db";
	char* ret_app_id = NULL;
	const char* service_provider_id = TEST_PACKAGE_NAME;
	char* ret_service_provider_id = NULL;
	const char* icon_path = "icon_path";
	char* ret_icon_path = NULL;
	const char* small_icon_path = "small_icon_path";
	char* ret_small_icon_path = NULL;
	const bool multi_account_support = true;
	bool ret_multi_account_support = true;
	const char* label = "label";
	char* ret_label = NULL;
	const char* locale = "locale";
	char* ret_capability = NULL;

	ret = account_type_create(&account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_app_id_internal(account_type, app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_service_provider_id_internal(account_type, service_provider_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_icon_path_internal(account_type, icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_small_icon_path_internal(account_type, small_icon_path);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_multiple_account_support_internal(account_type, multi_account_support);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_label_internal(account_type, label, locale);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_set_provider_feature_internal(account_type, contact_capability);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	ret = account_type_insert_to_db_internal(account_type, &account_type_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	_is_fail = true;

	ret = account_type_foreach_account_type_from_db(account_type_cb_func, &account_type);
//	assert_eq(_is_fail, false);
	is_callback_fail();
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	_is_fail = true;

	ret = account_type_destroy(account_type);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	account_type_delete_by_app_id_internal(app_id);
	assert_eq(ret, ACCOUNT_ERROR_NONE);

	return 0;
}

int utc_account_type_foreach_account_type_from_db_n(void)
{
	int ret = ACCOUNT_ERROR_NONE;

	ret = account_type_foreach_account_type_from_db(NULL, NULL);
	assert_eq(ret, ACCOUNT_ERROR_INVALID_PARAMETER);

	return 0;
}

