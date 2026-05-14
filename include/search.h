#pragma once
#include <stddef.h>
#include "include.h"
#include "launch.h"

typedef struct {
    char *name;
    char *path;
} App;

typedef struct {
    App    *item;
    size_t  count;
} Find;

Find findfile(const char *pattern, char **dirs);
void find_free(Find *fr);


