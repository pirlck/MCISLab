//
//
//
//

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <time.h>

void doit(char *text);

time_t str_to_time(const char *datetime);

char *get_curr_time(void);

int get_tag_num(const char *key,char sp);

int get_tag_by_index(const char *key,char *tag, int index);

#endif
