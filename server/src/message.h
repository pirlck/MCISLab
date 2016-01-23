// 
// this file is about converted data into json
// 
// or parse json data into char

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "common.h"

void json_to_msg_client_info(char *text, struct client_info *client);

void json_to_msg_queryres(char *text, struct queryres *qres);

void json_to_download_req(char *text, struct download_req *req);

void json_to_resource_share(char *text,struct resource_share *res);

void server_info_to_json(char *text, struct server_info *server);

char *res_list_to_json(struct resource_type *res, int len);

char *peer_info_to_json(struct peer_info *peers, int len);

#endif
