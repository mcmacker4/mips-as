#ifndef ASM_TOKENIZER_H
#define ASM_TOKENIZER_H

#include <mips-as/prelude.h>
#include "token.h"

#include <stdint.h>
#include <glib.h>

typedef struct tokenizer {
    GQueue* tokens;

    const char* src;
    uint32_t srclen;
    uint32_t position;

    uint32_t line;
    uint32_t col;

    GHashTable* registers;
} tokenizer_t;

GQueue* tokenize(const char* src);

#endif // ASM_TOKENIZER_H
