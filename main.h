#ifndef MAIN_H_
#define MAIN_H_

#include "header.h"
#include <shell.h>

#define CLEAR_N_EXIT(status) \
do { \
    _clearHistory(history); \
    exit(status); \
} while (0)

static History *history;
static char *line;

#endif