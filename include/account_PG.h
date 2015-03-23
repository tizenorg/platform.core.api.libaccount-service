/*
 * Account
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

/**
 *
 * @ingroup   SLP_PG
 * @defgroup  Account_PG Account


<h1 class="pg">Introduction</h1>
	<h2 class="pg">Purpose of this document</h2>

The purpose of this document is to describe how applications can use account APIs for handling Account's information. This document gives programming guidelines to application engineers and examples of using Account data.

	<h2 class="pg">Scope</h2>

The scope of this document is limited to Account API usage.


<h1 class="pg">Account Architecture</h1>
	<h2 class="pg"> Overview</h2>

Account is responsible for inserting, deleting, and updating Account data in order to accommodate the needs for application's Account data.
Users can access Account data without knowing DB schema, SQLite, relations of data

<h1 class="pg">Account Features</h1>
	- Similar to Sqlite3
	- Handle information of Account

	<h2 class="pg">Similar to Sqlite3</h2>
Account API is similar to Sqlite3.

	<h2 class="pg">Handle information of Account</h2>
Account supports to insert/update/delete/get/set/query information of Account.
The Information of Account includes user name, package name, email, access token, icon path, source, display name, capability informatio etc.

<h1 class="pg">Account API Description</h1>

you can refer @ref ACCOUNT_SVC

<h1 class="pg">Sample Code</h1>

	<h2 class="pg">Connect to Account Database</h2>

Before using Account information from Account API, caller module should connect to the Account database, and after finishing with the account information, should disconnect from the Account database

@code
int account_connect(void);

int account_disconnect(void);
@endcode

	<h2 class="pg">Insert information of account</h2>

@code
void insert_test(void)
{
   int ret = -1;
   account_h account;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_create(&account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_user_name(account, "tarun.kr");

   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_display_name(account, "Tarun Kumar");

   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_domain_name(account, "Samsung electronics");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_email_address(account, "tarun@gmail.com");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_package_name(account, "com.samsung.twiter");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_icon_path(account, "icon-path");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_access_token(account, "ACCESSTOKEN");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE );
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }
   ret = account_set_source(account, "URUSA inc.");

   int i;

   for(i=0;i<USER_TXT_CNT;i++)
   {
       ret = account_set_user_text(account, i, "user txt");
       if(ret != ACCOUNT_ERROR_NONE)
       {
           SECURE_LOGD("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
       }

   }

   for(i=0;i<USER_INT_CNT;i++)
   {
       ret = account_set_user_int(account, i, 888);
       if(ret != ACCOUNT_ERROR_NONE)
       {
           SECURE_LOGD("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
       }
   }

   ret = account_set_capability(account, ACCOUNT_CAPABILITY_CONTACT, ACCOUNT_CAPABILITY_ENABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_CALENDAR, ACCOUNT_CAPABILITY_ENABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_ENABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_VIDEO, ACCOUNT_CAPABILITY_ENABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_EMAIL, ACCOUNT_CAPABILITY_ENABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_ENABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_VOIP, ACCOUNT_CAPABILITY_ENABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_SAMSUNG_APPS, ACCOUNT_CAPABILITY_ENABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_MOBILE_TRACKER, ACCOUNT_CAPABILITY_ENABLED);

   int account_id = -1;
   ret = account_insert_to_db(account, &account_id);
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
       return;
   }

   ret = account_destroy(account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   return;
}
@endcode

	<h2 class="pg">Get account by id</h2>

@code
bool _capability_get_callback(const char* capability_type, account_capability_state_e capability_value, void* user_data)
{
   char* test_type = capability_type;
   int test_value = capability_value;

   SECURE_LOGD("_capability_get_callback test_type = : %s\n", test_type);
   SECURE_LOGD("_capability_get_callback test_value = : %d\n", test_value);
   return TRUE;
}

static void _account_free_text(char *text)
{
	if (text) {
		free(text);
		text = NULL;
	}
}

void get_account_by_id(int account_id)
{
   int ret = - 1;
   account_h account;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_create(&account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_query_account_by_account_id(account_id, &account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   char *test_text = NULL;

   account_get_user_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_display_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_email_address(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_icon_path(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_source(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_package_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_domain_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_access_token(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   int i;

   for(i=0;i<USER_TXT_CNT;i++)
   {
	   _account_free_text(test_text);

	   account_get_user_text(handle, i, &test_text);
	   SECURE_LOGD("_account_get_callback : %s\n", test_text);
   }

   int test_int  = -1;

   account_auth_type_e auth_type;
   account_get_auth_type(handle, &auth_type);
   SECURE_LOGD("account_get_auth_type : %d\n", auth_type);

   account_secrecy_state_e secret;
   account_get_secret(handle, &secret);
   SECURE_LOGD("account_get_secret : %d\n", secret);

   account_sync_state_e sync_support;
   account_get_sync_support(handle, &sync_support);
   SECURE_LOGD("account_get_sync_support : %d\n", sync_support);

   for(i=0;i<USER_INT_CNT;i++)
   {
	   test_int  = -1;
	   account_get_user_int(handle, i, &test_int);
	   SECURE_LOGD("_account_get_callback : %d\n", test_int);
   }

   account_get_capability(handle, _capability_get_callback, NULL);

   ret = account_destroy(account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
}
@endcode


	<h2 class="pg">Get account list by user name</h2>

@code
static void _account_free_text(char *text)
{
	if (text) {
		free(text);
		text = NULL;
	}
}

bool _account_get_callback(account_h handle, void* user_data)
{
   SECURE_LOGD("_account_get_callback\n");

   char *test_text = NULL;

   account_get_user_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_display_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_email_address(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_icon_path(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_source(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_package_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_domain_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_access_token(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   int i;

   for(i=0;i<USER_TXT_CNT;i++)
   {
	_account_free_text(test_text);

	account_get_user_text(handle, i, &test_text);
	SECURE_LOGD("_account_get_callback : %s\n", test_text);
   }

   int test_int  = -1;

   account_auth_type_e auth_type;
   account_get_auth_type(handle, &auth_type);
   SECURE_LOGD("account_get_auth_type : %d\n", auth_type);

   account_secrecy_state_e secret;
   account_get_secret(handle, &secret);
   SECURE_LOGD("account_get_secret : %d\n", secret);

   account_sync_state_e sync_support;
   account_get_sync_support(handle, &sync_support);
   SECURE_LOGD("account_get_sync_support : %d\n", sync_support);

   for(i=0;i<USER_INT_CNT;i++)
   {
	test_int  = -1;
	account_get_user_int(handle, i, &test_int);
	SECURE_LOGD("_account_get_callback : %d\n", test_int);
   }

   account_get_capability(handle, _capability_get_callback, NULL);

   return TRUE;
}


void get_account_list_by_user_name(char * user_name)
{
   int ret = - 1;
   account_h account;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_query_account_by_user_name(_account_get_callback, user_name, NULL);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
}
@endcode

	<h2 class="pg">Get account list by package name</h2>

@code
static void _account_free_text(char *text)
{
	if (text) {
		free(text);
		text = NULL;
	}
}

bool _account_get_callback(account_h handle, void* user_data)
{
   SECURE_LOGD("_account_get_callback\n");

   char *test_text = NULL;

   account_get_user_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_display_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_email_address(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_icon_path(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_source(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_package_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_domain_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_access_token(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   int i;

   for(i=0;i<USER_TXT_CNT;i++)
   {
	   _account_free_text(test_text);

   	account_get_user_text(handle, i, &test_text);
   	SECURE_LOGD("_account_get_callback : %s\n", test_text);
   }

   int test_int  = -1;

   account_auth_type_e auth_type;
   account_get_auth_type(handle, &auth_type);
   SECURE_LOGD("account_get_auth_type : %d\n", auth_type);

   account_secrecy_state_e secret;
   account_get_secret(handle, &secret);
   SECURE_LOGD("account_get_secret : %d\n", secret);

   account_sync_state_e sync_support;
   account_get_sync_support(handle, &sync_support);
   SECURE_LOGD("account_get_sync_support : %d\n", sync_support);

   for(i=0;i<USER_INT_CNT;i++)
   {
   	test_int  = -1;
   	account_get_user_int(handle, i, &test_int);
   	SECURE_LOGD("_account_get_callback : %d\n", test_int);
   }

   account_get_capability(handle, _capability_get_callback, NULL);

   return TRUE;
}

void get_account_list_by_package_name(char * package_name)
{
   int ret = - 1;
   account_h account;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_query_account_by_package_name(_account_get_callback, package_name, NULL);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
}
@endcode

	<h2 class="pg">Get all account list</h2>

@code
static void _account_free_text(char *text)
{
	if (text) {
		free(text);
		text = NULL;
	}
}

bool _account_get_callback(account_h handle, void* user_data)
{
   SECURE_LOGD("_account_get_callback\n");

   char *test_text = NULL;

   account_get_user_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_display_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_email_address(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_icon_path(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_source(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_package_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_domain_name(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   _account_free_text(test_text);

   account_get_access_token(handle, &test_text);
   SECURE_LOGD("_account_get_callback : %s\n", test_text);

   int i;

   for(i=0;i<USER_TXT_CNT;i++)
   {
	_account_free_text(test_text);

   	account_get_user_text(handle, i, &test_text);
   	SECURE_LOGD("_account_get_callback : %s\n", test_text);
   }

   int test_int  = -1;

   account_auth_type_e auth_type;
   account_get_auth_type(handle, &auth_type);
   SECURE_LOGD("account_get_auth_type : %d\n", auth_type);

   account_secrecy_state_e secret;
   account_get_secret(handle, &secret);
   SECURE_LOGD("account_get_secret : %d\n", secret);

   account_sync_state_e sync_support;
   account_get_sync_support(handle, &sync_support);
   SECURE_LOGD("account_get_sync_support : %d\n", sync_support);

   for(i=0;i<USER_INT_CNT;i++)
   {
   	test_int  = -1;
   	account_get_user_int(handle, i, &test_int);
   	SECURE_LOGD("_account_get_callback : %d\n", test_int);
   }

   account_get_capability(handle, _capability_get_callback, NULL);

   return TRUE;
}

void get_all_account()
{
   int ret = - 1;
   account_h account;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_foreach_account_from_db(_account_get_callback, NULL);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
}
@endcode

	<h2 class="pg">Update information of account by account id</h2>

@code
void update_accont_by_id(int account_id)
{
   int ret = -1;
   account_h account;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_create(&account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_user_name(account, "update-tarun.kr");

   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_display_name(account, "update-Tarun Kumar");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_domain_name(account, "update-Samsung electronics");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_email_address(account, "update-tarun@gmail.com");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_package_name(account, "update-com.samsung.twiter");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_icon_path(account, "update-icon-path");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_access_token(account, "update-ACCESSTOKEN");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }
   ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE );
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }
   ret = account_set_source(account, "update-URUSA inc.");

   int i;

   for(i=0;i<USER_TXT_CNT;i++)
   {
       ret = account_set_user_text(account, i, "update-user txt");
       if(ret != ACCOUNT_ERROR_NONE)
       {
           SECURE_LOGD("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
       }

   }

   for(i=0;i<USER_INT_CNT;i++)
   {
       ret = account_set_user_int(account, i, 999);
       if(ret != ACCOUNT_ERROR_NONE)
       {
           SECURE_LOGD("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
       }
   }

   ret = account_set_capability(account, ACCOUNT_CAPABILITY_CONTACT, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_CALENDAR, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_VIDEO, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_EMAIL, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_VOIP, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_SAMSUNG_APPS, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_MOBILE_TRACKER, ACCOUNT_CAPABILITY_DISABLED);

   ret = account_update_to_db_by_id(account, account_id);
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
       return;
   }

   ret = account_destroy(account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }
}
@endcode

	<h2 class="pg">Update information of account by user name</h2>

@code
void update_accont_by_user_name(char *user_name, char *package_name)
{
   int ret = -1;
   account_h account;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_create(&account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_user_name(account, "update-tarun.kr");
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_display_name(account, "update-Tarun Kumar");
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_domain_name(account, "update-Samsung electronics");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_email_address(account, "update-tarun@gmail.com");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_package_name(account, "update-com.samsung.twiter");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_icon_path(account, "update-icon-path");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_access_token(account, "update-ACCESSTOKEN");
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_auth_type( account, ACCOUNT_AUTH_TYPE_OAUTH );
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }
   ret = account_set_secret( account, ACCOUNT_SECRECY_INVISIBLE );
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_set_source(account, "update-URUSA inc.");
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   int i;

   for(i=0;i<USER_TXT_CNT;i++)
   {
       ret = account_set_user_text(account, i, "update-user txt");
       if(ret != ACCOUNT_ERROR_NONE)
       {
           SECURE_LOGD("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
       }

   }

   for(i=0;i<USER_INT_CNT;i++)
   {
       ret = account_set_user_int(account, i, 999);
       if(ret != ACCOUNT_ERROR_NONE)
       {
           SECURE_LOGD("(%d)-[Account] ret = %d, account=%p, i=%d\n", __LINE__, ret, account, i);
       }
   }

   ret = account_set_capability(account, ACCOUNT_CAPABILITY_CONTACT, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_CALENDAR, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_PHOTO, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_VIDEO, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_EMAIL, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_STATUS_POST, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_VOIP, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_SAMSUNG_APPS, ACCOUNT_CAPABILITY_DISABLED);
   ret = account_set_capability(account, ACCOUNT_CAPABILITY_MOBILE_TRACKER, ACCOUNT_CAPABILITY_DISABLED);

   ret = account_update_to_db_by_user_name(account, user_name, package_name);
   if(ret != ACCOUNT_ERROR_NONE)
   {
       SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
       return;
   }

   ret = account_destroy(account);
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
      SECURE_LOGD("(%d)-[Account] ret = %d, account=%p\n", __LINE__, ret, account);
   }
}
@endcode

	<h2 class="pg">Delete account by id </h2>

@code
void delete_account_by_id(int account_id)
{
   int ret = -1;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_delete(account_id);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
   return;
}
@endcode

	<h2 class="pg">Update sync status by id </h2>

@code
void update_sync_status_by_id(int account_id, const account_sync_state_e sync_status)
{
   int ret = -1;

   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_update_sync_status_by_id(account_id, sync_status);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
   return;
}
@endcode

	<h2 class="pg">Delete account by user name </h2>

@code
void delete_account_by_user_name(char *user_name, char *package_name)
{
   int ret = -1;
   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_delete_from_db_by_user_name(user_name, package_name);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d \n", __LINE__, ret);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
   return;
}
@endcode

	<h2 class="pg">Delete account by package name </h2>

@code
void delete_account_by_package_name(char *package_name)
{
   int ret = -1;
   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_delete_from_db_by_package_name(package_name);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d \n", __LINE__, ret);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
   return;
}
@endcode

	<h2 class="pg">Get account insert dbus notification </h2>

@code
void notify_account_insert()
{
    DBusMessage* msg;
    DBusConnection* conn;
    DBusError err;

    SECURE_LOGD("Listening for signals\n");

    dbus_error_init(&err);

    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if  (dbus_error_is_set(&err)) {
            dbus_error_free(&err);
    }
    if (NULL == conn) {
            exit(1);
    }

    dbus_bus_add_match(conn, "type='signal',interface='account.signal.Type'", &err);
    dbus_connection_flush(conn);
    if  (dbus_error_is_set(&err)) {
            exit(1);
    }
    SECURE_LOGD("Match rule sent\n");

    while (true) {

            dbus_connection_read_write(conn, 0);
            msg = dbus_connection_pop_message(conn);

            if  (NULL == msg) {
                sleep(1);
                continue;
            }

            if (dbus_message_is_signal(msg, ACCOUNT_DBUS_SIGNAL_INTERFACE, ACCOUNT_DBUS_NOTI_NAME_INSERT)) {
            	  SECURE_LOGD("Got Signal with name insert\n");
            }

            dbus_message_unref(msg);
    }
    dbus_connection_close(conn);
}
@endcode

	<h2 class="pg">Get account update dbus notification </h2>

@code
void notify_account_update()
{
    DBusMessage* msg;
    DBusConnection* conn;
    DBusError err;

    SECURE_LOGD("Listening for signals\n");


    dbus_error_init(&err);

    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if  (dbus_error_is_set(&err)) {
            dbus_error_free(&err);
    }
    if (NULL == conn) {
            exit(1);
    }

    dbus_bus_add_match(conn, "type='signal',interface='account.signal.Type'", &err);
    dbus_connection_flush(conn);
    if  (dbus_error_is_set(&err)) {
            exit(1);
    }
    SECURE_LOGD("Match rule sent\n");

    while (true) {

            dbus_connection_read_write(conn, 0);
            msg = dbus_connection_pop_message(conn);

            if  (NULL == msg) {
                sleep(1);
                continue;
            }

            if (dbus_message_is_signal(msg, ACCOUNT_DBUS_SIGNAL_INTERFACE, ACCOUNT_DBUS_NOTI_NAME_UPDATE)) {
                SECURE_LOGD("Got Signal with name update\n");
            }

            dbus_message_unref(msg);
    }
    dbus_connection_close(conn);
}
@endcode

	<h2 class="pg">Get account delete dbus notification </h2>

@code
void notify_account_delete()
{
    DBusMessage* msg;
    DBusConnection* conn;
    DBusError err;

    SECURE_LOGD("Listening for signals\n");


    dbus_error_init(&err);

    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if  (dbus_error_is_set(&err)) {
            dbus_error_free(&err);
    }
    if (NULL == conn) {
            exit(1);
    }

    dbus_bus_add_match(conn, "type='signal',interface='account.signal.Type'", &err);
    dbus_connection_flush(conn);
    if  (dbus_error_is_set(&err)) {
            exit(1);
    }
    SECURE_LOGD("Match rule sent\n");

    while (true) {

            dbus_connection_read_write(conn, 0);
            msg = dbus_connection_pop_message(conn);

            if  (NULL == msg) {
                sleep(1);
                continue;
            }

            if (dbus_message_is_signal(msg, ACCOUNT_DBUS_SIGNAL_INTERFACE, ACCOUNT_DBUS_NOTI_NAME_DELETE)) {
                SECURE_LOGD("Got Signal with name delete\n");
            }

            dbus_message_unref(msg);
    }
    dbus_connection_close(conn);
}
@endcode

	<h2 class="pg">Get capability by acccount id</h2>

@code
bool _capability_get_callback(const char* capability_type, account_capability_state_e capability_value, void* user_data)
{
   char* test_type = capability_type;
   int test_value = capability_value;

   SECURE_LOGD("_capability_get_callback test_type = : %s\n", test_type);
   SECURE_LOGD("_capability_get_callback test_value = : %d\n", test_value);
   return TRUE;
}

void capability_by_account_id(int account_id)
{
   int ret = -1;
   ret = account_connect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }

   ret = account_query_capability_by_account_id(_capability_get_callback, account_id, NULL);
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d \n", __LINE__, ret);
   }

   ret = account_disconnect();
   if(ret != ACCOUNT_ERROR_NONE)
   {
	   SECURE_LOGD("(%d)-[Account] ret = %d, \n", __LINE__, ret);
   }
   return;
}
@endcode
 * @}
 */

