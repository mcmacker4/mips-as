#include "assembler.h"
#include "parse/token.h"

#include <string.h>
#include <ctype.h>

assembler_t assembler_new(const char* src, size_t len) {
    assembler_t assembler;
    assembler.textbuff = buffer_create();
    assembler.databuff = buffer_create();
    assembler.sector = SECTOR_TEXT;
    return assembler;
}

void assembler_free(assembler_t* as) {
    g_queue_free_full(as->tokens, free_token);
}

static inline token_t* peek(assembler_t* as) {
    return g_queue_peek_head(as->tokens);
}

void assembler_run(assembler_t* as) {

}


