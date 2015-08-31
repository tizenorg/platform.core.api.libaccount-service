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
#include <tzplatform_config.h>

#include "account-private.h"
#include "account_internal.h"
#include "dbg.h"

#ifdef TIZEN_PROFILE_MOBILE
#include "mobile/account.h"
#else
#include "wearable/account.h"
#endif

#define ACCOUNT_DB_OPEN_READONLY 0
#define ACCOUNT_DB_OPEN_READWRITE 1

#define OWNER_ROOT 0
#define GLOBAL_USER tzplatform_getuid(TZ_SYS_GLOBALAPP_USER)

typedef sqlite3_stmt* account_stmt;

static sqlite3* g_hAccountDB = NULL;
static int		g_refCntDB = 0;
pthread_mutex_t account_mutex = PTHREAD_MUTEX_INITIALIZER;

static int _account_get_record_count(char* query);
static int _account_execute_query(const char *query);

static const char *_account_db_err_msg()
{
	return sqlite3_errmsg(g_hAccountDB);
}

static int _account_db_err_code()
{
	return sqlite3_errcode(g_hAccountDB);
}

//TODO: Need to enable creating db on the first connect for
//a) multi-user cases
//b) to ensure db exist in every connect call

static int _account_create_all_tables(void)
{
	int rc = -1;
	int error_code = ACCOUNT_ERROR_NONE;
	char	query[ACCOUNT_SQL_LEN_MAX] = {0, };

	_INFO("create all table - BEGIN");
	ACCOUNT_MEMSET(query, 0, sizeof(query));

	/*Create the account table*/
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", ACCOUNT_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		rc = _account_execute_query(ACCOUNT_SCHEMA);
		if(rc == SQLITE_BUSY) return ACCOUNT_ERROR_DATABASE_BUSY;
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", ACCOUNT_SCHEMA, rc, _account_db_err_msg()));

	}

	/*Create capability table*/
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", CAPABILITY_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		rc = _account_execute_query(CAPABILITY_SCHEMA);
		if(rc == SQLITE_BUSY) return ACCOUNT_ERROR_DATABASE_BUSY;
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", CAPABILITY_SCHEMA, rc, _account_db_err_msg()));
	}

	/* Create account custom table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", ACCOUNT_CUSTOM_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		rc = _account_execute_query(ACCOUNT_CUSTOM_SCHEMA);
		if(rc == SQLITE_BUSY) return ACCOUNT_ERROR_DATABASE_BUSY;
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", query, rc, _account_db_err_msg()));
	}

	/* Create account type table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", ACCOUNT_TYPE_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		rc = _account_execute_query(ACCOUNT_TYPE_SCHEMA);
		if(rc == SQLITE_BUSY) return ACCOUNT_ERROR_DATABASE_BUSY;
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", ACCOUNT_TYPE_SCHEMA, rc, _account_db_err_msg()));
	}

	/* Create label table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", LABEL_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		rc = _account_execute_query(LABEL_SCHEMA);
		if(rc == SQLITE_BUSY) return ACCOUNT_ERROR_DATABASE_BUSY;
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", LABEL_SCHEMA, rc, _account_db_err_msg()));
	}

	/* Create account feature table */
	ACCOUNT_MEMSET(query, 0, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s')", PROVIDER_FEATURE_TABLE);
	rc = _account_get_record_count(query);
	if (rc <= 0) {
		rc = _account_execute_query(PROVIDER_FEATURE_SCHEMA);
		if(rc == SQLITE_BUSY) return ACCOUNT_ERROR_DATABASE_BUSY;
		ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_execute_query(%s) failed(%d, %s).\n", PROVIDER_FEATURE_SCHEMA, rc, _account_db_err_msg()));
	}

	_INFO("create all table - END");
	return error_code;
}

static int _account_check_is_all_table_exists()
{
	int 	rc = 0;
	char	query[ACCOUNT_SQL_LEN_MAX] = {0,};
	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "select count(*) from sqlite_master where name in ('%s', '%s', '%s', '%s', '%s', '%s')",
			ACCOUNT_TABLE, CAPABILITY_TABLE, ACCOUNT_CUSTOM_TABLE, ACCOUNT_TYPE_TABLE, LABEL_TABLE, PROVIDER_FEATURE_TABLE);
	rc = _account_get_record_count(query);

	if (rc != ACCOUNT_TABLE_TOTAL_COUNT) {
		ACCOUNT_ERROR("Table count is not matched rc=%d\n", rc);
	}

	return rc;
}

static int _account_db_open(int mode)
{
	int  rc = 0;
	char account_db_path[256] = {0, };
	uid_t uid = -1;

	_INFO( "start to get DB path");

	ACCOUNT_MEMSET(account_db_path, 0x00, sizeof(account_db_path));

	uid = getuid();
	if (uid == OWNER_ROOT || uid == GLOBAL_USER)
		ACCOUNT_SNPRINTF(account_db_path, sizeof(account_db_path), "%s", ACCOUNT_GLOBAL_DB_PATH);
	else
		ACCOUNT_SNPRINTF(account_db_path, sizeof(account_db_path), "%s", ACCOUNT_DB_PATH);
	_INFO( "account_db_path canonicalized = %s", account_db_path);

	if (!g_hAccountDB) {
		if (-1 == access (USER_DB_DIR, F_OK)) {
			mkdir(USER_DB_DIR, 755);
		}

		if (mode == ACCOUNT_DB_OPEN_READWRITE)
			rc = db_util_open(account_db_path, &g_hAccountDB, DB_UTIL_REGISTER_HOOK_METHOD);
		else {
			return ACCOUNT_ERROR_DB_NOT_OPENED;
		}

		if (_account_db_err_code() == SQLITE_PERM){
			ACCOUNT_ERROR( "Access failed(%s)", _account_db_err_msg());
			return ACCOUNT_ERROR_PERMISSION_DENIED;
		}

		if (rc == SQLITE_BUSY) {
			ACCOUNT_ERROR( "busy handler fail.");
			return ACCOUNT_ERROR_DATABASE_BUSY;
		}

		ACCOUNT_RETURN_VAL((rc != SQLITE_PERM), {}, ACCOUNT_ERROR_PERMISSION_DENIED, ("Account permission denied rc : %d", rc));
		ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected. rc : %d", rc));

		rc = _account_check_is_all_table_exists();

		if (rc < 0) {
			_ERR("_account_check_is_all_table_exists rc=[%d]", rc);
			return rc;
		} else if (rc == ACCOUNT_TABLE_TOTAL_COUNT) {
			_INFO("Tables OK rc=[%d]", rc);
		} else {
			int ret = _account_create_all_tables();
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

static int _account_db_close(void)
{
	int rc = 0;
	int ret = -1;

	if (g_hAccountDB) {
		if (g_refCntDB > 0) {
			g_refCntDB--;
		}
		if (g_refCntDB == 0) {
			rc = db_util_close(g_hAccountDB);
			if(  rc == SQLITE_PERM ){
				ACCOUNT_ERROR( "Access failed(SQLITE_PERM)");
				return ACCOUNT_ERROR_PERMISSION_DENIED;
			} else if ( rc == SQLITE_BUSY ){
				ACCOUNT_ERROR( "database busy");
				return ACCOUNT_ERROR_DATABASE_BUSY;
			}
			ACCOUNT_RETURN_VAL((rc == SQLITE_OK), {}, ACCOUNT_ERROR_DB_FAILED, ("The database isn't connected. rc : %d", rc));
			g_hAccountDB = NULL;
		}
		ret = ACCOUNT_ERROR_NONE;
	} else {
		ACCOUNT_ERROR( "_account_svc_db_close: No handle(). refcnt=%d ", g_refCntDB);
		ret = ACCOUNT_ERROR_DB_FAILED;
	}

	return ret;
}

static int _account_execute_query(const char *query)
{
	int rc = -1;
	char* pszErrorMsg = NULL;

	if(!query){
		ACCOUNT_ERROR("NULL query\n");
		return ACCOUNT_ERROR_QUERY_SYNTAX_ERROR;
	}

	if(!g_hAccountDB){
		ACCOUNT_ERROR("DB is not opened\n");
		return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	rc = sqlite3_exec(g_hAccountDB, query, NULL, NULL, &pszErrorMsg);
	if (SQLITE_OK != rc) {
		ACCOUNT_ERROR("sqlite3_exec rc(%d) query(%s) failed(%s).", rc, query, pszErrorMsg);
		sqlite3_free(pszErrorMsg);
	}

	return rc;
}

static int _account_begin_transaction(void)
{
	ACCOUNT_DEBUG("_account_begin_transaction start");
	int ret = -1;

	ret = _account_execute_query("BEGIN IMMEDIATE TRANSACTION");

	if (ret == SQLITE_BUSY){
		ACCOUNT_ERROR(" sqlite3 busy = %d", ret);
		return ACCOUNT_ERROR_DATABASE_BUSY;
	} else if(ret != SQLITE_OK) {
		ACCOUNT_ERROR("_account_svc_begin_transaction fail :: %d", ret);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	ACCOUNT_DEBUG("_account_begin_transaction end");
	return ACCOUNT_ERROR_NONE;
}

static int _account_end_transaction(bool is_success)
{
	ACCOUNT_DEBUG("_account_end_transaction start");

	int ret = -1;

	if (is_success == true) {
		ret = _account_execute_query("COMMIT TRANSACTION");
		ACCOUNT_DEBUG("_account_end_transaction COMMIT");
	} else {
		ret = _account_execute_query("ROLLBACK TRANSACTION");
		ACCOUNT_DEBUG("_account_end_transaction ROLLBACK");
	}

	if(ret == SQLITE_PERM){
		ACCOUNT_ERROR("Account permission denied :: %d", ret);
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	if (ret == SQLITE_BUSY){
		ACCOUNT_DEBUG(" sqlite3 busy = %d", ret);
		return ACCOUNT_ERROR_DATABASE_BUSY;
	}

	if (ret != SQLITE_OK) {
		ACCOUNT_ERROR("_account_svc_end_transaction fail :: %d", ret);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	ACCOUNT_DEBUG("_account_end_transaction end");
	return ACCOUNT_ERROR_NONE;
}

static int _account_get_record_count(char* query)
{
	_INFO("_account_get_record_count");

	int rc = -1;
	int ncount = 0;
	account_stmt pStmt = NULL;

	if(!query){
		_ERR("NULL query\n");
		return ACCOUNT_ERROR_QUERY_SYNTAX_ERROR;
	}

	if(!g_hAccountDB){
		_ERR("DB is not opened\n");
		return ACCOUNT_ERROR_DB_NOT_OPENED;
	}

	rc = sqlite3_prepare_v2(g_hAccountDB, query, strlen(query), &pStmt, NULL);

	if (SQLITE_BUSY == rc){
		_ERR("sqlite3_prepare_v2() failed(%d, %s).", rc, _account_db_err_msg());
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_DATABASE_BUSY;
	} else if (SQLITE_OK != rc) {
		_ERR("sqlite3_prepare_v2() failed(%d, %s).", rc, _account_db_err_msg());
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	rc = sqlite3_step(pStmt);
	if (SQLITE_BUSY == rc) {
		_ERR("sqlite3_step() failed(%d, %s).", rc, _account_db_err_msg());
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_DATABASE_BUSY;
	} else if (SQLITE_ROW != rc) {
		_ERR("sqlite3_step() failed(%d, %s).", rc, _account_db_err_msg());
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	ncount = sqlite3_column_int(pStmt, 0);

	_INFO("account record count [%d]", ncount);
	sqlite3_finalize(pStmt);

	return ncount;
}

static int _account_query_bind_int(account_stmt pStmt, int pos, int num)
{
	if(!pStmt){
		ACCOUNT_ERROR("statement is null");
		return -1;
	}

	if(pos < 0){
		ACCOUNT_ERROR("invalid pos");
		return -1;
	}

	return sqlite3_bind_int(pStmt, pos, num);
}

static int _account_query_bind_text(account_stmt pStmt, int pos, const char *str)
{
	_INFO("_account_query_bind_text");

	if(!pStmt)
	{
		_ERR("statement is null");
		return -1;
	}

	if(str)
	{
		_INFO("sqlite3_bind_text");
		return sqlite3_bind_text(pStmt, pos, (const char*)str, strlen(str), SQLITE_STATIC);
	}
	else
	{
		_INFO("sqlite3_bind_null");
		return sqlite3_bind_null(pStmt, pos);
	}
}

static int _account_type_convert_account_to_sql(account_type_s *account_type, account_stmt hstmt, char *sql_value)
{
	_INFO("");

	int count = 1;

	/*Caution : Keep insert query orders.*/

	/* 1. app id*/
	_account_query_bind_text(hstmt, count++, (char*)account_type->app_id);

	/* 2. service provider id*/
	_account_query_bind_text(hstmt, count++, (char*)account_type->service_provider_id);

	/* 3. icon path*/
	_account_query_bind_text(hstmt, count++, (char*)account_type->icon_path);

	/* 4. small icon path*/
	_account_query_bind_text(hstmt, count++, (char*)account_type->small_icon_path);

	/* 5. multiple accont support*/
	_account_query_bind_int(hstmt, count++, account_type->multiple_account_support);

	_INFO("");

	return count;
}

static gboolean _account_type_check_duplicated(account_type_s *data)
{
	char query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int count = 0;

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE AppId='%s'"
			, ACCOUNT_TYPE_TABLE, data->app_id);

	count = _account_get_record_count(query);
	if (count > 0) {
		return TRUE;
	}

	return FALSE;
}

static int _account_query_finalize(account_stmt pStmt)
{
	int rc = -1;

	if (!pStmt) {
		ACCOUNT_ERROR( "pStmt is NULL");
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	rc = sqlite3_finalize(pStmt);
	if (rc == SQLITE_BUSY){
		ACCOUNT_ERROR(" sqlite3 busy = %d", rc);
		return ACCOUNT_ERROR_DATABASE_BUSY;
	} else if (rc != SQLITE_OK) {
		ACCOUNT_ERROR( "sqlite3_finalize fail, rc : %d, db_error : %s\n", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_DB_FAILED;
	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_query_step(account_stmt pStmt)
{
	if(!pStmt){
		ACCOUNT_ERROR( "pStmt is NULL");
		return -1;
	}

	return sqlite3_step(pStmt);
}

static account_stmt _account_prepare_query(char *query)
{
	int 			rc = -1;
	account_stmt 	pStmt = NULL;

	ACCOUNT_RETURN_VAL((query != NULL), {}, NULL, ("query is NULL"));

	rc = sqlite3_prepare_v2(g_hAccountDB, query, strlen(query), &pStmt, NULL);

	ACCOUNT_RETURN_VAL((SQLITE_OK == rc), {}, NULL, ("sqlite3_prepare_v2(%s) failed(%s).", query, _account_db_err_msg()));

	return pStmt;
}

static int _account_get_next_sequence(const char *pszName)
{
	int 			rc = 0;
	account_stmt	pStmt = NULL;
	int 			max_seq = 0;
	char 			szQuery[ACCOUNT_SQL_LEN_MAX] = {0,};

	ACCOUNT_MEMSET(szQuery, 0x00, sizeof(szQuery));
	ACCOUNT_SNPRINTF(szQuery, sizeof(szQuery),  "SELECT max(seq) FROM %s where name = '%s' ", ACCOUNT_SQLITE_SEQ, pszName);
	rc = sqlite3_prepare_v2(g_hAccountDB, szQuery, strlen(szQuery), &pStmt, NULL);
	if (SQLITE_OK != rc) {
		ACCOUNT_SLOGE("sqlite3_prepare_v2() failed(%d, %s).", rc, _account_db_err_msg());
		sqlite3_finalize(pStmt);
		return ACCOUNT_ERROR_DB_FAILED;
	}

	rc = sqlite3_step(pStmt);
	max_seq = sqlite3_column_int(pStmt, 0);
	max_seq++;

	/*Finalize Statement*/
	rc = sqlite3_finalize(pStmt);
	pStmt = NULL;

	return max_seq;
}

static int _account_type_insert_provider_feature(account_type_s *account_type, const char* app_id)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));

	if (g_slist_length( account_type->provider_feature_list)==0) {
		ACCOUNT_ERROR( "no capability\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where AppId='%s'", ACCOUNT_TYPE_TABLE, app_id);

	rc = _account_get_record_count(query);

	if( _account_db_err_code() == SQLITE_PERM ){
		ACCOUNT_ERROR( "Access failed(%s)", _account_db_err_msg());
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	if (rc <= 0) {
		ACCOUNT_SLOGI( "related account type item is not existed rc=%d , %s", rc, _account_db_err_msg());
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* insert query*/

	GSList *iter;

	for (iter = account_type->provider_feature_list; iter != NULL; iter = g_slist_next(iter)) {
		int ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(app_id, key) VALUES "
				"(?, ?) ", PROVIDER_FEATURE_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		provider_feature_s* feature_data = NULL;
		feature_data = (provider_feature_s*)iter->data;

		ret = _account_query_bind_text(hstmt, count++, app_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, feature_data->key);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Integer binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_ERROR( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		rc = _account_query_finalize(hstmt);
		ACCOUNT_RETURN_VAL((rc == ACCOUNT_ERROR_NONE), {}, rc, ("finalize error"));
		hstmt = NULL;

	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_insert_label(account_type_s *account_type)
{
	int 			rc, count = 1;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	account_stmt 	hstmt = NULL;

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT HANDLE IS NULL"));

	if (g_slist_length( account_type->label_list)==0) {
		ACCOUNT_ERROR( "_account_type_insert_label, no label\n");
		return ACCOUNT_ERROR_NONE;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) from %s where AppId = '%s'", ACCOUNT_TYPE_TABLE, account_type->app_id);

	rc = _account_get_record_count(query);

	if( _account_db_err_code() == SQLITE_PERM ){
		ACCOUNT_ERROR( "Access failed(%s)", _account_db_err_msg());
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	if (rc <= 0) {
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* insert query*/
	GSList *iter;

	for (iter = account_type->label_list; iter != NULL; iter = g_slist_next(iter)) {
		int ret;
		count = 1;
		ACCOUNT_MEMSET(query, 0x00, sizeof(query));
		ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s(AppId, Label, Locale) VALUES "
				"(?, ?, ?) ", LABEL_TABLE);

		hstmt = _account_prepare_query(query);

		ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

		label_s* label_data = NULL;
		label_data = (label_s*)iter->data;

		ret = _account_query_bind_text(hstmt, count++, account_type->app_id);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, label_data->label);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));
		ret = _account_query_bind_text(hstmt, count++, (char*)label_data->locale);
		ACCOUNT_RETURN_VAL((ret == ACCOUNT_ERROR_NONE), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("Text binding fail"));

		rc = _account_query_step(hstmt);

		if (rc != SQLITE_DONE) {
			ACCOUNT_ERROR( "_account_query_step() failed(%d, %s)", rc, _account_db_err_msg());
			break;
		}

		rc = _account_query_finalize(hstmt);
		ACCOUNT_RETURN_VAL((rc == ACCOUNT_ERROR_NONE), {}, rc, ("finalize error"));
		hstmt = NULL;

	}

	return ACCOUNT_ERROR_NONE;
}

static int _account_type_execute_insert_query(account_type_s *account_type)
{
	_INFO("");

	int				rc = 0;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int				error_code = ACCOUNT_ERROR_NONE;
	account_stmt 	hstmt = NULL;

	/* check mandatory field */
	// app id & service provider id
	if (!account_type->app_id) {
		return ACCOUNT_ERROR_INVALID_PARAMETER;
	}

	ACCOUNT_MEMSET(query, 0x00, sizeof(query));
	ACCOUNT_SNPRINTF(query, sizeof(query), "INSERT INTO %s( AppId, ServiceProviderId , IconPath , SmallIconPath , MultipleAccountSupport ) values "
			"(?, ?, ?, ?, ?)",	ACCOUNT_TYPE_TABLE);

	_INFO("");
	hstmt = _account_prepare_query(query);
	_INFO("");

	if( _account_db_err_code() == SQLITE_PERM ){
		ACCOUNT_ERROR( "Access failed(%s)", _account_db_err_msg());
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	} else if( _account_db_err_code() == SQLITE_BUSY ){
		ACCOUNT_ERROR( "Database Busy(%s)", _account_db_err_msg());
		return ACCOUNT_ERROR_DATABASE_BUSY;
	}

	ACCOUNT_RETURN_VAL((hstmt != NULL), {}, ACCOUNT_ERROR_DB_FAILED, ("_account_prepare_query() failed(%s).\n", _account_db_err_msg()));

	_INFO("");
	_account_type_convert_account_to_sql(account_type, hstmt, query);
	_INFO("");

	rc = _account_query_step(hstmt);
	if (rc == SQLITE_BUSY) {
		ACCOUNT_ERROR( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		error_code = ACCOUNT_ERROR_DATABASE_BUSY;
	} else if (rc != SQLITE_DONE) {
		ACCOUNT_ERROR( "account_db_query_step() failed(%d, %s)", rc, _account_db_err_msg());
		error_code = ACCOUNT_ERROR_DB_FAILED;
	}

	_INFO("");
	rc = _account_query_finalize(hstmt);
	ACCOUNT_RETURN_VAL((rc == ACCOUNT_ERROR_NONE), {}, rc, ("finalize error"));
	hstmt = NULL;

	_INFO("");
	return error_code;
}

int _account_type_insert_to_db(account_type_s* account_type, int* account_type_id)
{
	_INFO("");

	int		error_code = ACCOUNT_ERROR_NONE, ret_transaction = 0;

	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));
	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((account_type_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE ID POINTER IS NULL"));

	account_type_s *data = (account_type_s*)account_type;

	pthread_mutex_lock(&account_mutex);


	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	_INFO("");

	if( _account_db_err_code() == SQLITE_PERM ){
		pthread_mutex_unlock(&account_mutex);
		ACCOUNT_ERROR( "Access failed(%s)", _account_db_err_msg());
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	_INFO("");
	if( ret_transaction == ACCOUNT_ERROR_DATABASE_BUSY ){
		ACCOUNT_ERROR( "database busy(%s)", _account_db_err_msg());
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_DATABASE_BUSY;
	} else if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	_INFO("");
	if (_account_type_check_duplicated(data)) {
		_INFO("");
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("Duplicated, rollback insert query(%x)!!!!\n", ret_transaction);
		*account_type_id = -1;
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_DUPLICATED;
	} else {
		_INFO("");
		*account_type_id = _account_get_next_sequence(ACCOUNT_TYPE_TABLE);

		error_code = _account_type_execute_insert_query(data);

		if (error_code != ACCOUNT_ERROR_NONE){
			error_code = ACCOUNT_ERROR_DUPLICATED;
			ret_transaction = _account_end_transaction(FALSE);
			ACCOUNT_ERROR("Insert fail, rollback insert query(%x)!!!!\n", ret_transaction);
			*account_type_id = -1;
			pthread_mutex_unlock(&account_mutex);
			return error_code;
		}
	}

	_INFO("");
	error_code = _account_type_insert_provider_feature(data, data->app_id);
	if(error_code != ACCOUNT_ERROR_NONE) {
		_INFO("");
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("Insert provider feature fail(%x), rollback insert query(%x)!!!!\n", error_code, ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return error_code;
	}
	_INFO("");
	error_code = _account_type_insert_label(data);
	if(error_code != ACCOUNT_ERROR_NONE) {
		_INFO("");
		ret_transaction = _account_end_transaction(FALSE);
		ACCOUNT_ERROR("Insert label fail(%x), rollback insert query(%x)!!!!\n", error_code, ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return error_code;
	}

	ret_transaction = _account_end_transaction(TRUE);
	_INFO("");
	pthread_mutex_unlock(&account_mutex);

	_INFO("");
	return ACCOUNT_ERROR_NONE;
}

int _account_type_delete_by_app_id(const char* app_id)
{
	int 			error_code = ACCOUNT_ERROR_NONE;
	account_stmt	hstmt = NULL;
	char			query[ACCOUNT_SQL_LEN_MAX] = {0, };
	int 			rc = 0, count = -1;
	int 			ret_transaction = 0;
	int				binding_count = 1;
	bool			is_success = FALSE;

	ACCOUNT_RETURN_VAL((g_hAccountDB != NULL), {}, ACCOUNT_ERROR_DB_NOT_OPENED, ("The database isn't connected."));
	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("The database isn't connected."));

	/* Check requested ID to delete */
	ACCOUNT_SNPRINTF(query, sizeof(query), "SELECT COUNT(*) FROM %s WHERE AppId = '%s'", ACCOUNT_TYPE_TABLE, app_id);

	count = _account_get_record_count(query);

	if( _account_db_err_code() == SQLITE_PERM ){
		ACCOUNT_ERROR( "Access failed(%s)", _account_db_err_msg());
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	if (count <= 0) {
		ACCOUNT_SLOGE("app id(%s) is not exist. count(%d)\n", app_id, count);
		return ACCOUNT_ERROR_RECORD_NOT_FOUND;
	}

	/* transaction control required*/
	ret_transaction = _account_begin_transaction();

	if( ret_transaction == ACCOUNT_ERROR_DATABASE_BUSY ){
		ACCOUNT_ERROR( "database busy(%s)", _account_db_err_msg());
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_DATABASE_BUSY;
	}else if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("account_delete:_account_begin_transaction fail %d\n", ret_transaction);
		pthread_mutex_unlock(&account_mutex);
		return ret_transaction;
	}

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AppId = ?", LABEL_TABLE);

	hstmt = _account_prepare_query(query);

	if( _account_db_err_code() == SQLITE_PERM ){
		ACCOUNT_ERROR( "Access failed(%s)", _account_db_err_msg());
		pthread_mutex_unlock(&account_mutex);
		return ACCOUNT_ERROR_PERMISSION_DENIED;
	}

	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found.\n"));

	rc = _account_query_finalize(hstmt);
	ACCOUNT_RETURN_VAL((rc == ACCOUNT_ERROR_NONE), {}, rc, ("finalize error"));
	hstmt = NULL;

	binding_count = 1;
	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE app_id = ? ", PROVIDER_FEATURE_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found. AppId=%s, rc=%d\n", app_id, rc));

	rc = _account_query_finalize(hstmt);
	ACCOUNT_RETURN_VAL((rc == ACCOUNT_ERROR_NONE), {}, rc, ("finalize error"));
	is_success = TRUE;

	hstmt = NULL;

	binding_count = 1;
	ACCOUNT_MEMSET(query, 0, sizeof(query));

	ACCOUNT_SNPRINTF(query, sizeof(query), "DELETE FROM %s WHERE AppId = ? ", ACCOUNT_TYPE_TABLE);

	hstmt = _account_prepare_query(query);
	ACCOUNT_CATCH_ERROR(hstmt != NULL, {}, ACCOUNT_ERROR_DB_FAILED,
			("_account_svc_query_prepare(%s) failed(%s).\n", query, _account_db_err_msg()));

	_account_query_bind_text(hstmt, binding_count++, app_id);

	rc = _account_query_step(hstmt);
	ACCOUNT_CATCH_ERROR(rc == SQLITE_DONE, {}, ACCOUNT_ERROR_RECORD_NOT_FOUND, ("The record isn't found. AppId=%s, rc=%d\n", app_id, rc));

	rc = _account_query_finalize(hstmt);
	ACCOUNT_RETURN_VAL((rc == ACCOUNT_ERROR_NONE), {}, rc, ("finalize error"));
	is_success = TRUE;

	hstmt = NULL;

	CATCH:
	if (hstmt != NULL) {
		rc = _account_query_finalize(hstmt);
		ACCOUNT_RETURN_VAL((rc == ACCOUNT_ERROR_NONE), {}, rc, ("finalize error"));
		hstmt = NULL;
	}

	ret_transaction = _account_end_transaction(is_success);

	if (ret_transaction != ACCOUNT_ERROR_NONE) {
		ACCOUNT_ERROR("account_svc_delete:_account_svc_end_transaction fail %d, is_success=%d\n", ret_transaction, is_success);
	}

	pthread_mutex_unlock(&account_mutex);

	return error_code;
}

ACCOUNT_INTERNAL_API int account_type_insert_to_db_offline(account_type_h account_type, int* account_type_id)
{
	_INFO("account_type_insert_to_db starting");

	ACCOUNT_RETURN_VAL((account_type != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE HANDLE IS NULL"));
	ACCOUNT_RETURN_VAL((account_type_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("ACCOUNT TYPE ID POINTER IS NULL"));

	int db_id = -1;

	_INFO("account_manager_account_type_add start");

	guint pid = getpid();
	_INFO("client Id = [%u]", pid);

	int return_code = _account_db_open(1);
	if (return_code != ACCOUNT_ERROR_NONE)
	{
		_ERR("_account_db_open() error, ret = %d", return_code);

		goto RETURN;
	}

	uid_t uid = getuid();
	if (uid != OWNER_ROOT && uid != GLOBAL_USER)
	{
		_ERR("current process is not root user nor global user, uid=%d", uid);
		goto RETURN;
	}

	_INFO("before _account_type_insert_to_db");
	return_code = _account_type_insert_to_db((account_type_s*)account_type, &db_id);
	_INFO("after _account_type_insert_to_db");
	if (return_code != ACCOUNT_ERROR_NONE)
	{
		_ERR("_account_type_insert_to_db error");
		goto RETURN;
	}

	*account_type_id = db_id;

	account_type_s* account_type_data = (account_type_s*)account_type;
	account_type_data->id = db_id;

RETURN:
	_INFO("account_manager_account_type_add end");

	if( g_hAccountDB == NULL )
		return return_code;

	return_code = _account_db_close();
	if (return_code != ACCOUNT_ERROR_NONE)
	{
		ACCOUNT_DEBUG("_account_db_close() fail[%d]", return_code);
		return_code = ACCOUNT_ERROR_DB_FAILED;
	}

	return return_code;
}

ACCOUNT_INTERNAL_API int account_type_delete_by_app_id_offline(const char* app_id)
{
	_INFO("account_type_delete_by_app_id starting");

	ACCOUNT_RETURN_VAL((app_id != NULL), {}, ACCOUNT_ERROR_INVALID_PARAMETER, ("APP ID IS NULL"));

	guint pid = getpid();

	_INFO("client Id = [%u]", pid);

	int return_code = _account_db_open(1);
	if (return_code != ACCOUNT_ERROR_NONE)
	{
		_ERR("_account_db_open() error, ret = %d", return_code);

		goto RETURN;
	}

	uid_t uid = getuid();
	if (uid != OWNER_ROOT && uid != GLOBAL_USER)
	{
		_ERR("current daemon is not root user, uid=%d", uid);
		goto RETURN;
	}

	_INFO("before _account_type_delete_by_app_id");
	return_code = _account_type_delete_by_app_id(app_id);
	_INFO("after _account_type_delete_by_app_id=[%d]", return_code);

	if (return_code != ACCOUNT_ERROR_NONE)
	{
		_ERR("_account_type_delete_by_app_id error");
		goto RETURN;
	}

RETURN:
	_INFO("account_type_delete_by_app_id_offline end");

	if( g_hAccountDB == NULL )
		return return_code;

	return_code = _account_db_close();
	if (return_code != ACCOUNT_ERROR_NONE)
	{
		ACCOUNT_DEBUG("_account_db_close() fail[%d]", return_code);
		return_code = ACCOUNT_ERROR_DB_FAILED;
	}

	return return_code;
}

