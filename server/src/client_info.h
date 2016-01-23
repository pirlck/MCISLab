//
//
//
//

#ifndef __CLIENT_INFO__
#define __CLIENT_INFO__

int has_client(const char *mac);

int add_client(struct client_info *newclient);

void print_client_list();

#endif
