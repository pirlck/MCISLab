//
// use sql script query res source 
//
//


#ifndef dbconnect__h 
#define dbconnect__h 

#include "common.h"
#include "/usr/include/mysql/mysql.h"

#define HOST "localhost"
#define USER "blueclick"
#define PASSWORD "blueclick.com"
#define DATABASE "db_blueclick"
#define TBL_RESOURCE "tbl_resource"
#define TBL_RESOURCE_OWNER "tbl_resource_owner"
#define TBL_RESOURCE_TAGS "tbl_resource_tags"

MYSQL *open();

int add_share_resource(const struct resource_share *res);

int query_res_md5(const char *key, char (*res_md5)[33], int *len);

int query_res_info(const char *res_md5, struct resource_type *resinfo);

int remove_duplicate_md5(char md5[][33], int *len);

int get_res_list(struct queryres key, struct resource_type *res, int *len);

int query_macs_by_md5(const char *res_md5, char mac[][32], int *le);

int query_last_share_res(char md5[][33],int *len);

int get_push_list(struct resource_type *res, int *len);

#endif
