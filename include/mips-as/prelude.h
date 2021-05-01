#ifndef ASM_PRELUDE_H
#define ASM_PRELUDE_H

#include <malloc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <glib.h>

#define FATAL(...) {                \
    g_printerr(__VA_ARGS__);        \
    exit(-1);                       \
}

#endif //ASM_PRELUDE_H
