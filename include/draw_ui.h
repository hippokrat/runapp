#pragma once

#include<stddef.h>
#include "launch.h"
#include "search.h"
#include "include.h"
#include "config.h"


typedef struct {
	char **d;
	int mode;
	size_t max_result;
} Draw;

char *draw(Draw *cfg);
