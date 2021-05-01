#ifndef ASM_ASSEMBLER_H
#define ASM_ASSEMBLER_H

#include "buffer.h"
#include <stddef.h>
#include <glib.h>

typedef enum sector {
    SECTOR_TEXT,
    SECTOR_DATA
} sector_t;

typedef struct assembler {
    buffer_t textbuff;
    buffer_t databuff;
    sector_t sector;
    GQueue* tokens;
} assembler_t;

assembler_t assembler_new(const char* src, size_t len);
void assembler_run(assembler_t* as);

#endif //ASM_ASSEMBLER_H
