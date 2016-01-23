//
//  this file is about mysql connection 
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/usr/include/mysql/mysql.h"
#include "common.h"
#include "dbconnect.h"
#include "tools.h"

const char *host = HOST;
const char *user = USER;
const char *password = PASSWORD;
const char *database = DATABASE;
const char *tbl_rescource = TBL_RESOURCE;
const char *tbl_rescource_owner = TBL_RESOURCE_OWNER;
const char *tbl_rescource_tags = TBL_RESOURCE_TAGS;

//

// open mysql 
// connected mysql
// initial database
//
MYSQL *open()
{
	MYSQL *mysql;

	mysql = mysql_init(NULL);
	mysql_options(mysql,MYSQL_SET_CHARSET_NAME,"utf8");
	if(mysql == NULL)
	{
		printf("mysql_init failed!\n");
		return NULL;
	}

	mysql = mysql_real_connect(mysql,host,user,password,database,0,NULL,0);
	if(mysql == 0)
	{
		printf("mysql_real connect failed!\n");
		return NULL;
	}
	return mysql;
}


// add share resource
//
// success 0;failed -1
int add_share_resource(const struct resource_share *res)
{
	
	MYSQL *mysql = NULL;
	char query_str[512];
	int affect_rows;
	if(mysql == NULL) {
		mysql = open();
	}
	
	// insert into tbl_resoure 
	sprintf(query_str,"INSERT INTO tbl_resource(f_res_name,\
		f_res_size,f_res_md5,f_res_piececount) VALUES('%s',\
		'%s','%s','%d')", res->name,res->size,res->md5,res->piececount);
	if(mysql_query(mysql,query_str)) {
		fprintf(stderr,"Error: %s\n",mysql_error(mysql));
	}
	
	// get sharetime
	char *sharetime;
	sharetime = get_curr_time();

	// insert into tbl_resource_owner 
	if(strlen(res->mac) <= 0) {

		mysql_close(mysql);
		return 0;
	}
	memset(query_str,'\0',sizeof(query_str));
	sprintf(query_str,"INSERT INTO tbl_resource_owner(f_res_md5,\
		f_res_owner,f_res_sharetime) VALUES('%s','%s','%s')",res->md5
		,res->mac,sharetime);
	if(mysql_query(mysql,query_str)) {
		fprintf(stderr,"Error: %s\n",mysql_error(mysql));
	}


	// insert into tbl_resource_tags 
	if(strlen(res->tag) <= 0) {

		mysql_close(mysql);
		return 0;
	}
	const char *sp = TAG_SEPERATOR;
	char *tag = NULL;
	char key[512];

	int tag_failed = 0;
	strcpy(key,res->tag);
	tag = strtok(key,sp);
	while(tag) {
		memset(query_str,'\0',sizeof(query_str));
		sprintf(query_str,"INSERT INTO tbl_resource_tags(f_res_md5,\
			f_res_tags) VALUES('%s','%s')",res->md5,tag);
		if(mysql_query(mysql,query_str)) {
			fprintf(stderr,"Error: %s\n",mysql_error(mysql));

			mysql_close(mysql);
			return -1;
		}
		affect_rows = mysql_affected_rows(mysql);
		if(affect_rows < 0) {
			tag_failed = 1;
		}

		tag = strtok(NULL,sp);
	}

	mysql_close(mysql);

	return tag_failed == 1 ? -1:0;
}

// query res_md5 by res_key
// result stored into res_md5[][]
//success 0;failed -1
int query_res_md5(const char *key, char (*res_md5)[33], int *len)
{
	MYSQL *mysql = NULL;
	char query_str[256];
	int cnt;
	MYSQL_RES *result;
	MYSQL_ROW query_rows;

	if(mysql == NULL) {
		mysql = open();
	}

	cnt = 10;
	if(*len > 0 || *len <=10) {
		cnt = *len;
	}

	sprintf(query_str,"SELECT f_res_md5 FROM tbl_resource_tags WHERE f_res_tags like '%s%%' limit %d",key,cnt);
	if(mysql_query(mysql,query_str)) {
		fprintf(stderr,"Error: %s\n",mysql_error(mysql));
		return -1;
	}

	result = mysql_use_result(mysql);

	//can query many rows
	cnt = 0;
	while((query_rows = mysql_fetch_row(result))) {
		strcpy(res_md5[cnt++],query_rows[0]);
	}
	*len = cnt;
	mysql_close(mysql);

	return 0;
}

//
//
//
int query_res_info(const char *res_md5,struct resource_type *resinfo)
{
	MYSQL *mysql = NULL;
	MYSQL_RES *result;
	MYSQL_ROW query_rows;
	char query_str[512];

	if(mysql == NULL) {
		mysql = open();
	}

	sprintf(query_str,"SELECT f_res_name,f_res_size, f_res_piececount FROM tbl_resource WHERE f_res_md5 = '%s'",res_md5);
	if(mysql_query(mysql,query_str)) {
		fprintf(stderr,"Error: %s\n",mysql_error(mysql));
		return -1;
	}

	result = mysql_use_result(mysql);
	if((query_rows = mysql_fetch_row(result)) != NULL) {
		
		// mysql query
		strcpy(resinfo->name,query_rows[0]);
		strcpy(resinfo->size,query_rows[1]);
		strcpy(resinfo->md5,res_md5);
		resinfo->piececount = atoi(query_rows[2]);

		mysql_close(mysql);
		return 0;
	}

	mysql_close(mysql);
	return -1;
}

// remove duplicat md5 in md5 list
int remove_duplicate_md5(char md5[][33], int *len)
{
	int tlen = *len;

	for(int i=0; i<tlen-1; i++) {
		for(int j=tlen-1; j>i; j--) {
			if(strcmp(md5[i],md5[j]) == 0 ) {
				for(int k=j; k<tlen-1; k++) {
					strcpy(md5[k],md5[k+1]);
				}
				tlen -= 1;
			}		
		}
	}
	int count = *len - tlen;
	*len = tlen;

	return count;
}

//get res by resource key
int get_res_list(struct queryres key, struct resource_type *res, int *len)
{
	char md5_list[10][33];
	int cnt;
	
	cnt = 10;
	if(*len > 0 || *len <=10) {
		cnt = *len;
	}
	// query md5 by mac 
	query_res_md5(key.key,md5_list,&cnt);

	// remove duplicate item 
	remove_duplicate_md5(md5_list,&cnt);

	*len = cnt;
	for(int i=0;i<*len;i++) {
		query_res_info(md5_list[i],&res[i]);
	}

	return 0;
}


// query res mac by md5
// store into mac
// success 0;-1
int query_macs_by_md5(const char *res_md5, char mac[][32], int *len)
{
	MYSQL *mysql = NULL;
	MYSQL_RES *result;
	MYSQL_ROW query_rows;
	char query_str[256];

	if(mysql == NULL) {
		mysql = open();
	}

	sprintf(query_str,"SELECT f_res_owner FROM tbl_resource_owner WHERE f_res_md5 = '%s' limit 10",res_md5);
	if(mysql_query(mysql,query_str)) {
		fprintf(stderr,"Error: %s\n",mysql_error(mysql));
		return -1;
	}

	result = mysql_use_result(mysql);

	int cnt = 0;
	while((query_rows = mysql_fetch_row(result))) {
		strcpy(mac[cnt++],query_rows[0]);
	}
	*len = cnt;
	mysql_close(mysql);

	return 0;
}

// get last share resource,at most 10 issues
int query_last_share_res(char md5[][33], int *len)
{
	MYSQL *mysql = NULL;
	MYSQL_RES *result;
	MYSQL_ROW query_rows;
	char query_str[256];
	int cnt;

	cnt = 10;
	if(*len > 0 && *len <= 10) {
		cnt = *len;
	}

	if(mysql == NULL) {
		mysql = open();
	}

	sprintf(query_str,"SELECT distinct f_res_md5 FROM tbl_resource_owner ORDER BY f_res_sharetime desc limit %d",cnt);
	if(mysql_query(mysql,query_str)) {
		fprintf(stderr,"Error: %s\n",mysql_error(mysql));
		return -1;
	}
	result = mysql_use_result(mysql);

	cnt = 0;
	while((query_rows = mysql_fetch_row(result))) {
		strcpy(md5[cnt++],query_rows[0]);
	}
	*len = cnt;
	mysql_close(mysql);

	return 0;
}

// get push list 
int get_push_list(struct resource_type *res, int *len)
{
	char md5[20][33];
	int len_md5 = *len;

	query_last_share_res(md5,&len_md5);
	for(int i=0;i<len_md5;i++) {
		query_res_info(md5[i],&res[i]);
	}
	
	*len = len_md5;
	
	return 0;
}
