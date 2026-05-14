/* 
 * This file will be read path and sort 
 * and then return res to run_launcher.c
 *
 */
#include <sys/types.h>
#include <dirent.h>
#include "search.h"

static App *filter_path(App *apps, size_t sizetot, const char *patt, size_t *out_count) 
{
	size_t cap, cnt;
	App *res, *tmp;

	cap = 16;
	cnt = 0;
	res = malloc(sizeof(App) * cap);
	if(!res)
		return NULL;
	
	for(size_t i = 0; i < sizetot; ++i) {
		if(!strstr(apps[i].name, patt))
			continue;
		if(cnt >= cap) {
			cap *= 2;
			tmp = realloc(res, sizeof(App) * cap);
			if(!tmp) {
				free(res);
				return NULL;
			}
			res = tmp;
		}
		res[cnt].name = strdup(apps[i].name);
		res[cnt].path = strdup(apps[i].path);
		cnt++;
	}
	*out_count = cnt;
	return res;
}

static App *scanapps(char **dir, size_t *out_size) 
{ 
	static char *default_dirs[] = {
		"/bin", "/usr/bin", "/usr/sbin", 
		"/usr/local/bin", "/usr/local/sbin", NULL
	};
	char 		fullpath[1024];
	struct 		dirent *dp;
	size_t 		cnt, cap;
	App 		*apps;
	DIR 		*d;

	if(!dir) 
		dir = default_dirs;
	cap = 128;
	apps = malloc(sizeof(App) * cap);
	if(!apps)
		return NULL;
	cnt = 0;
	for(int i = 0; dir[i]; i++) {
		if((d = opendir(dir[i])) == NULL) {
			continue;
		}
		while((dp = readdir(d)) != NULL) {
			if(!strcmp(dp->d_name, ".") || 
					!strcmp(dp->d_name, "..")) 
				continue;
			memset(fullpath, 0, sizeof(fullpath));
			snprintf(fullpath, sizeof(fullpath), "%s/%s", dir[i], dp->d_name);

			if(cnt >= cap) {
				cap *= 2;
				App *tmp = realloc(apps, sizeof(App) * cap);
				if(!tmp) {
					closedir(d);
					free(apps);
					return NULL;
				}
				apps = tmp;
			}
			apps[cnt].name = strdup(dp->d_name);
			apps[cnt].path = strdup(fullpath);
			cnt++;
		}
		closedir(d);
	}
	*out_size = cnt;
	return apps;
}
static void apps_free(App *f, size_t f_count) 
{
	if(!f) 
		return;
	for(size_t i = 0; i < f_count; ++i) {
		free(f[i].name);
		free(f[i].path);
	}
	free(f);
}
void find_free(Find *fr)
{
	if(!fr || !fr->item)
		return;
	for(size_t i = 0; i < fr->count; ++i) {
		free(fr->item[i].name);
		free(fr->item[i].path);
	}
	free(fr->item);
	fr->item = NULL;
	fr->count = 0;
}

Find findfile(const char *pattern, char **dirs) 
{
	App *filtered, *lookdir;
	size_t fcount, total;
	Find res;

	res.count =  0;
	res.item  =  NULL;
	filtered  =  NULL;
	lookdir   =	 NULL;
	fcount    =  0;
	total 	  =  0;

	lookdir = scanapps(dirs, &total);
	if(!total) {
		return res;
	}

	filtered = filter_path(lookdir, total, pattern, &fcount);
	apps_free(lookdir, total);

	if(!filtered || !fcount) {
		free(filtered);
		return res;
	}
	res.item = filtered;
	res.count = fcount;
	return res;
}
