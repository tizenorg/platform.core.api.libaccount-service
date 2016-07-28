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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <db-util.h>
#include <pthread.h>
#include <vconf.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <pkgmgr_installer_info.h>

#include <dbg.h>
#include <account_free.h>
#include <account-private.h>
#include <account_db_helper.h>
#include "account_internal.h"

#ifdef TIZEN_PROFILE_MOBILE
#include "mobile/account.h"
#else
#include "wearable/account.h"
#endif

#define ACCOUNT_DB_OPEN_READONLY 0
#define ACCOUNT_DB_OPEN_READWRITE 1

#define APP_GID 100
#define MIN_USER_UID 5000
#define SERVICE_FW_UID 651
#define SERVICE_FW_GID 651
#define PW_BUF_LEN 4096

typedef sqlite3_stmt * account_stmt;

static sqlite3 *g_hAccountUserDB = NULL;
static sqlite3 *g_hAccountGlobalDB = NULL;
static int g_refCntDB = 0;
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

static int _account_user_db_close(sqlite3 *hAccountDB)
{
	int rc = 0;
	int ret = ACCOUNT_ERROR_DB_FAILED;

	if (hAccountDB) {
		rc = db_util_close(hAccountDB);
		if (rc == SQLITE_PERM) {
			ACCOUNT_ERROR("Access failed(SQLITE_PERM)");
			return ACCOUNT_ERROR_PERMISSION_DENIED;
		} else if (rc == SQLITE_BUSY) {
			ACCOUNT_ERROR("database busy");
			return ACCOUNT_ERROR_DATABASE_BUSY;
		}
		ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_FAILED, ("The database isn't connected. rc : %d", rc));
		hAccountDB = NULL;
		ret = ACCOUNT_ERROR_NONE;
	}

	return ret;
}


static int _account_user_db_open(sqlite3 **p_hAccountDB, int mode, uid_t uid)
{
	int  rc = 0;
	char account_db_dir[256] = {0, };
	char account_db_path[256] = {0, };

	ACCOUNT_MEMSET(account_db_dir, 0x00, sizeof(account_db_dir));
	ACCOUNT_MEMSET(account_db_path, 0x00, sizeof(account_db_path));

	ACCOUNT_GET_USER_DB_PATH(account_db_path, sizeof(account_db_path), uid);
	_INFO("account_db_path canonicalized = %s", account_db_path);

	if (!g_hAccountUserDB)
		_account_user_db_close(*p_hAccountDB);

	ACCOUNT_GET_USER_DB_DIR(account_db_dir, sizeof(account_db_dir), uid);
	if ((-1 == access(account_db_dir, F_OK)) && uid != OWNER_ROOT) {
		int ret;
		mkdir(account_db_dir, 777);
		ret = chown(account_db_dir, SERVICE_FW_UID, SERVICE_FW_GID);
		ACCOUNT_DEBUG("chown result = [%d]", ret);
		ret = chmod(account_db_dir, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
		ACCOUNT_DEBUG("chmod result = [%d]", ret);
		rc = db_util_open(account_db_path, p_hAccountDB, DB_UTIL_REGISTER_HOOK_METHOD);
		ret = chown(account_db_path, SERVICE_FW_UID, SERVICE_FW_GID);
		ACCOUNT_DEBUG("chown result = [%d]", ret);
		ret = chmod(account_db_path, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
		ACCOUNT_DEBUG("chmod result = [%d]", ret);
	} else {
		if (mode == ACCOUNT_DB_OPEN_READWRITE)
			rc = db_util_open(account_db_path, p_hAccountDB, DB_UTIL_REGISTER_HOOK_METHOD);
		else
			return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	if (_account_db_err_code(*p_hAccountDB) == SQLITE_PERM) {
		ACCOUNT_ERROR("Access failed(%s)", _account_db_err_msg(*p_hAccountDB));
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	if (rc == SQLITE_BUSY) {
		ACCOUNT_ERROR("busy handler fail.");
		return ACCOUNT_ERROR_DATABASE_BUSY;
	}

	ACCOUNT_RETURN_VAL((rc != SQLITE_PERM), {}, ACCOUNT_ERROR_PERMISSION_DENIED, ("Account permission denied rc : %d", rc));
	ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", rc));

	rc = _account_check_is_all_table_exists(*p_hAccountDB);

	if (rc < 0) {
		_ERR("_account_check_is_all_table_exists rc=[%d]", rc);
		return rc;
	} else if (rc == ACCOUNT_TABLE_TOTAL_COUNT) {
		_INFO("Tables OK rc=[%d]", rc);
	} else {
		int ret = _account_create_all_tables(*p_hAccountDB);
		if (ret != ACCOUNT_ERROR_NONE) {
			_ERR("_account_create_all_tables fail ret=[%d]", ret);
			return ret;
		}
	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_global_db_open(int mode)
{
	int  rc = 0;
	char account_db_dir[256] = {0, };
	char account_db_path[256] = {0, };
	uid_t uid = -1;

	_INFO("start to get DB path");

	ACCOUNT_MEMSET(account_db_dir, 0x00, sizeof(account_db_dir));
	ACCOUNT_MEMSET(account_db_path, 0x00, sizeof(account_db_path));

	if (pkgmgr_installer_info_get_target_uid(&uid) < 0) {
		ACCOUNT_ERROR("pkgmgr_installer_info_get_target_uid() fail");
		return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	if (uid != OWNER_ROOT && uid != GLOBAL_USER) {
		ACCOUNT_ERROR("global db open fail. user not both root or global user");
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	ACCOUNT_GET_GLOBAL_DB_PATH(account_db_path, sizeof(account_db_path));

	_INFO("account_db_path canonicalized = %s", account_db_path);

	if (!g_hAccountGlobalDB) {
		ACCOUNT_GET_USER_DB_DIR(account_db_dir, sizeof(account_db_dir), uid);
		if ((-1 == access(account_db_dir, F_OK)) && uid != OWNER_ROOT) {
			int ret;
			mkdir(account_db_dir, 777);
			ret = chown(account_db_dir, SERVICE_FW_UID, SERVICE_FW_GID);
			ACCOUNT_DEBUG("chown result = [%d]", ret);
			ret = chmod(account_db_dir, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH);
			ACCOUNT_DEBUG("chmod result = [%d]", ret);
			rc = db_util_open(account_db_path, &g_hAccountGlobalDB, DB_UTIL_REGISTER_HOOK_METHOD);
			ret = chown(account_db_path, SERVICE_FW_UID, SERVICE_FW_GID);
			ACCOUNT_DEBUG("chown result = [%d]", ret);
			ret = chmod(account_db_path, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
			ACCOUNT_DEBUG("chmod result = [%d]", ret);
		} else {
			if (mode == ACCOUNT_DB_OPEN_READWRITE)
				rc = db_util_open(account_db_path, &g_hAccountGlobalDB, DB_UTIL_REGISTER_HOOK_METHOD);
			else
				return ACCOUNT_ERROR_DB_NOT_OPENED;
		}

		if (_account_db_err_code(g_hAccountGlobalDB) == SQLITE_PERM) {
			ACCOUNT_ERROR("Access failed(%s)", _account_db_err_msg(g_hAccountGlobalDB));
			return ACCOUNT_ERROR_PERMISSION_DENIED;
		}

		if (rc == SQLITE_BUSY) {
			ACCOUNT_ERROR("busy handler fail.");
			return ACCOUNT_ERROR_DATABASE_BUSY;
		}

		ACCOUNT_RETURN_VAL((rc != SQLITE_PERM), {}, ACCOUNT_ERROR_PERMISSION_DENIED, ("Account permission denied rc : %d", rc));
		ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", rc));

		rc = _account_check_is_all_table_exists(g_hAccountGlobalDB);

		if (rc < 0) {
			_ERR("_account_check_is_all_table_exists rc=[%d]", rc);
			return rc;
		} else if (rc == ACCOUNT_TABLE_TOTAL_COUNT) {
			_INFO("Tables OK rc=[%d]", rc);
		} else {
			int ret = _account_create_all_tables(g_hAccountGlobalDB);
			if (ret != ACCOUNT_ERROR_NONE) {
				_ERR("_account_create_all_tables fail ret=[%d]", ret);
				return ret;
			}
		}

		g_refCntDB++;
	} else {
		g_refCntDB++;
	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_global_db_close(void)
{
	int rc = 0;
	int ret = -1;

	if (g_hAccountGlobalDB) {
		if (g_refCntDB > 0)
			g_refCntDB--;
		if (g_refCntDB == 0) {
			rc = db_util_close(g_hAccountGlobalDB);
			if (rc == SQLITE_PERM) {
				ACCOUNT_ERROR("Access failed(SQLITE_PERM)");
				return ACCOUNT_ERROR_PERMISSION_DENIED;
			} else if (rc == SQLITE_BUSY) {
				ACCOUNT_ERROR("database busy");
				return ACCOUNT_ERROR_DATABASE_BUSY;
			}
			ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_FAILED, ("The database isn't connected. rc : %d", rc));
			g_hAccountGlobalDB = NULL;
		}
		ret = ACCOUNT_ERROR_NONE;
	} else {
		ACCOUNT_ERROR("_account_svc_db_close: No handle(). refcnt=%d ", g_refCntDB);
		ret = ACCOUNT_ERROR_DB_FAILED;
	}

	return ret;
}


ACCOUNT_INTERNAL_API int account_type_insert_to_db_offline(account_type_h account_type, int *account_type_id)
{
	_INFO("account_type_insert_to_db starting");

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((account_type_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE ID POINTER IS NULL"));

	int db_id = -1;
	int return_code = ACCOUNT_ERROR_NONE;
	uid_t uid = -1;
	account_type_s *account_type_data = NULL;

	_INFO("account_manager_account_type_add start");

	guint pid = getpid();
	_INFO("client Id = [%u]", pid);

	return_code = _account_global_db_open(1);
	if (return_code != ACCOUNT_ERROR_NONE) {
		_ERR("_account_global_db_open() error, ret = %d", return_code);
		goto RETURN;
	}

	if (pkgmgr_installer_info_get_target_uid(&uid) < 0) {
		ACCOUNT_ERROR("pkgmgr_installer_info_get_target_uid() fail");
		return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	if (uid != OWNER_ROOT && uid != GLOBAL_USER) {
		_ERR("current process is not root user nor global user, uid=%d", uid);
		goto RETURN;
	}

	account_type_data = (account_type_s *)account_type;
	if (account_type_data->app_id == NULL) {
		_ERR("app id of account_type_h is null");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}
	if (_account_type_check_duplicated(g_hAccountGlobalDB, account_type_data->app_id)) {
		_ERR("account_type duplicated, app_id=[%s]", account_type_data->app_id);
		return ACCOUNT_ERROR_DUPLICATED;
	}

	_INFO("before _account_type_insert_to_db");
	return_code = _account_type_insert_to_db(g_hAccountGlobalDB, account_type_data, &db_id);
	_INFO("after _account_type_insert_to_db");
	if (return_code != ACCOUNT_ERROR_NONE) {
		_ERR("_account_type_insert_to_db error");
		goto RETURN;
	}

	*account_type_id = db_id;
	account_type_data->id = db_id;

RETURN:
	_INFO("account_manager_account_type_add end");

	if (g_hAccountUserDB == NULL)
		return return_code;

	return_code = _account_global_db_close();
	if (return_code != ACCOUNT_ERROR_NONE)
		ACCOUNT_DEBUG("_account_global_db_close() fail[%d]", return_code);

	return return_code;
}

ACCOUNT_INTERNAL_API int account_type_delete_by_app_id_offline(const char *app_id)
{
	_INFO("account_type_delete_by_app_id starting");

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));

	guint pid = getpid();

	_INFO("client Id = [%u]", pid);

	int return_code = _account_global_db_open(1);
	if (return_code != ACCOUNT_ERROR_NONE) {
		_ERR("_account_global_db_open() error, ret = %d", return_code);
		goto RETURN;
	}

	uid_t uid;
	if (pkgmgr_installer_info_get_target_uid(&uid) < 0) {
		ACCOUNT_ERROR("pkgmgr_installer_info_get_target_uid() fail");
		return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	if (uid != OWNER_ROOT && uid != GLOBAL_USER) {
		_ERR("current daemon is not root user, uid=%d", uid);
		goto RETURN;
	}

	pthread_mutex_lock(&account_mutex);
	_INFO("before _account_type_delete_by_app_id");
	return_code = _account_type_delete_by_app_id(g_hAccountGlobalDB, app_id);
	_INFO("after _account_type_delete_by_app_id=[%d]", return_code);
	pthread_mutex_unlock(&account_mutex);

	if (return_code != ACCOUNT_ERROR_NONE) {
		_ERR("_account_type_delete_by_app_id error");
		goto RETURN;
	}

RETURN:
	if (g_hAccountGlobalDB == NULL)
		return return_code;

	return_code = _account_global_db_close();
	if (return_code != ACCOUNT_ERROR_NONE)
		ACCOUNT_DEBUG("_account_global_db_close() fail[%d]", return_code);

	_INFO("account_type_delete_by_app_id_offline end");
	return return_code;
}

ACCOUNT_INTERNAL_API int account_delete_from_db_by_package_name_offline(const char *package_name)
{
	_INFO("_account_delete_from_db_by_package_name_offline");
	uid_t uid = -1;
	uid_t gid = -1;
	int return_code = ACCOUNT_ERROR_NONE;
	struct passwd pw;
	struct passwd *user_pw = NULL;
	char buf[PW_BUF_LEN];

	ACCOUNT_RETURN_VAL((package_name != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("package_name is null!"));

	if (pkgmgr_installer_info_get_target_uid(&uid) < 0) {
		ACCOUNT_ERROR("pkgmgr_installer_info_get_target_uid() fail");
		return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	if (uid != 0) {
		_ERR("current process user is not root, uid=%d", uid);
		return_code = ACCOUNT_ERROR_PERMISSION_DENIED;
		goto RETURN;
	}

	setpwent();
	while (!getpwent_r(&pw, buf, PW_BUF_LEN, &user_pw)) {
		uid = user_pw->pw_uid;
		gid = user_pw->pw_gid;
		_INFO("user_pw->pw_uid=[%d], user_pw->pw_gid", uid, gid);
		if (uid > MIN_USER_UID && gid == APP_GID) {
			sqlite3 *hAccountDB = NULL;
			return_code = _account_user_db_open(&hAccountDB, 1, uid);

			if (return_code != ACCOUNT_ERROR_NONE)
				ACCOUNT_DEBUG("_account_user_db_open() error, ret=[%d]", return_code);

			return_code = _account_delete_account_by_package_name(hAccountDB, package_name, false, getpid(), uid);
			if (return_code != ACCOUNT_ERROR_NONE)
				ACCOUNT_DEBUG("_account_delete_account_by_package_name error=[%d]", return_code);

			return_code = _account_user_db_close(hAccountDB);
			if (return_code != ACCOUNT_ERROR_NONE)
				ACCOUNT_DEBUG("_account_user_db_close() fail[%d]", return_code);
		}
	}
	endpwent();

	return_code = ACCOUNT_ERROR_NONE;
RETURN:
	_INFO("account_delete_from_db_by_package_name_offline end");

	return return_code;
}
