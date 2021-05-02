#include "tokenizer.h"

#include <string.h>
#include <ctype.h>

#include <assert.h>

#define TK_REQUIRE(expr, ...) {                          \
    if (!(expr)) {                                      \
        fprintf(stderr, "Token Error: " __VA_ARGS__);   \
        exit(-1);                                       \
    }                                                   \
}

void tk_tokenize(tokenizer_t* tk);
void tk_skip_spaces(tokenizer_t* tk);

void tk_read_token(tokenizer_t* tk);
void tk_read_directive(tokenizer_t* tk);
void tk_read_label_or_symbol(tokenizer_t* tk);
void tk_read_number(tokenizer_t* tk);
void tk_read_string(tokenizer_t* tk);
void tk_read_register(tokenizer_t* tk);

uint32_t tk_remain(tokenizer_t* tk);

static inline char tk_peek(tokenizer_t* tk) {
    return tk->src[tk->position];
}

static inline char tk_consume(tokenizer_t* tk) {
    if (tk_peek(tk) == '\n') {
        tk->line++;
        tk->column = 1;
    } else {
        tk->column ++;
    }
    return tk->src[tk->position++];
}

void tk_populate_registers(GHashTable* table) {
#define R(k, v) g_hash_table_insert(table, k, (gpointer) v)
    R("zero", 0);
    R("at", 1);

    R("v0", 2);
    R("v1", 3);

    R("a0", 4);
    R("a1", 5);
    R("a2", 6);
    R("a3", 7);

    R("t0", 8);
    R("t1", 9);
    R("t2", 10);
    R("t3", 11);
    R("t4", 12);
    R("t5", 13);
    R("t6", 14);
    R("t7", 15);

    R("s0", 16);
    R("s1", 17);
    R("s2", 18);
    R("s3", 19);
    R("s4", 20);
    R("s5", 21);
    R("s6", 22);
    R("s7", 23);

    R("t8", 24);
    R("t9", 25);

    R("k0", 26);
    R("k1", 27);

    R("gp", 28);
    R("sp", 29);
    R("fp", 30);
    R("ra", 31);
#undef R
}

GQueue* tokenize(const char* src) {
    tokenizer_t tk;
    tk.src = src;
    tk.srclen = strlen(src);
    tk.position = 0;
    tk.line = 1;
    tk.column = 1;
    tk.tokens = g_queue_new();
    tk.registers = g_hash_table_new(g_str_hash, g_str_equal);
    tk_populate_registers(tk.registers);

    tk_tokenize(&tk);

    g_hash_table_destroy(tk.registers);

    return tk.tokens;
}

void tk_skip_spaces(tokenizer_t* tk) {
    char c = tk_peek(tk);
    while (isspace(c) && c != '\n') {
        tk_consume(tk);
        c = tk_peek(tk);
    }
}

void tk_tokenize(tokenizer_t* tk) {
    tk_skip_spaces(tk);
    while (tk_remain(tk)) {
        tk_read_token(tk);
        tk_skip_spaces(tk);
    }
}

void tk_read_token(tokenizer_t* tk) {
    char c = tk_peek(tk);
    if (c == '.') {
        tk_read_directive(tk);
    } else if (c == '$') {
        tk_read_register(tk);
    } else if (isalpha(c)) {
        tk_read_label_or_symbol(tk);
    } else if (isdigit(c) || c == '-' || c == '+') {
        tk_read_number(tk);
    } else if (c == '"') {
        tk_read_string(tk);
    } else if (c == ',') {
        uint32_t line = tk->line, column = tk->column;
        tk_consume(tk);
        token_t* token = g_new(token_t, 1);
        token->type = TK_COMMA;
        token->str = NULL;
        token->line = line;
        token->column = column;
        g_queue_push_tail(tk->tokens, token);
    } else if (c == '\n') {
        uint32_t line = tk->line, column = tk->column;
        tk_consume(tk);
        token_t* token = g_new(token_t, 1);
        token->type = TK_NEWLINE;
        token->str = NULL;
        token->line = line;
        token->column = column;
        g_queue_push_tail(tk->tokens, token);
    } else if (c == ';') {
        while (tk_remain(tk) && tk_peek(tk) != '\n') {
            tk_consume(tk);
        }
    } else {
        FATAL("Unexpected character: %c\n (%d:%d)", c, tk->line, tk->column)
    }
}

void tk_read_directive(tokenizer_t* tk) {
    uint32_t line = tk->line, column = tk->column;
    assert(tk_consume(tk) == '.');

    uint32_t pos = tk->position;
    while (isalpha(tk_peek(tk))) {
        tk_consume(tk);
    }

    token_t* token = g_new(token_t, 1);
    token->type = TK_DIRECTIVE;
    token->str = g_strndup(tk->src + pos, tk->position - pos);
    token->line = line;
    token->column = column;

    g_queue_push_tail(tk->tokens, token);
}

void tk_read_label_or_symbol(tokenizer_t* tk) {
    uint32_t line = tk->line, column = tk->column;
    uint32_t pos = tk->position;
    bool label = false;
    while (isalpha(tk_peek(tk))) {
        tk_consume(tk);
    }
    uint32_t len = tk->position - pos;

    if (tk_peek(tk) == ':') {
        tk_consume(tk);
        label = true;
    }

    token_t* token = g_new(token_t, 1);
    token->type = label ? TK_LABEL : TK_SYMBOL;
    token->str = g_strndup(tk->src + pos, len);
    token->line = line;
    token->column = column;

    g_queue_push_tail(tk->tokens, token);
}

void tk_read_number(tokenizer_t* tk) {
    uint32_t line = tk->line, column = tk->column;
    char* end;
    token_t* token = g_new(token_t, 1);
    token->type = TK_NUMBER;
    token->num = strtol(tk->src + tk->position, &end, 0);
    token->line = line;
    token->column = column;
    if (errno == ERANGE) {
        FATAL("Parsed num does not fit a Word (%d:%d)", tk->line, tk->column)
    }
    tk->position = (size_t) end - (size_t) tk->src;
    g_queue_push_tail(tk->tokens, token);
}

void tk_read_string(tokenizer_t* tk) {
    uint32_t line = tk->line, column = tk->column;
    assert(tk_consume(tk) == '"');
    uint32_t pos = tk->position;
    while (tk_remain(tk) && tk_peek(tk) != '"') {
        tk_consume(tk);
    }
    uint32_t len = tk->position - pos;
    if (tk_consume(tk) != '"') {
        FATAL("Token Error: Unexpected end of file parsing string.")
    }
    const gchar* str = g_strndup(tk->src + pos, len);
    const gchar* estr = g_strcompress(str);
    g_free((gpointer) str);
    token_t* token = g_new(token_t, 1);
    token->type = TK_STRING;
    token->str = estr;
    token->line = line;
    token->column = column;
    g_queue_push_tail(tk->tokens, token);
}

void tk_read_register(tokenizer_t* tk) {
    uint32_t line = tk->line, column = tk->column;
    assert(tk_consume(tk) == '$');
    uint32_t pos = tk->position;

    bool number = true;
    while (isalnum(tk_peek(tk))) {
        if (!isdigit(tk_peek(tk)))
            number = false;
        tk_consume(tk);
    }

    token_t* token = g_new(token_t, 1);
    token->type = TK_REGISTER;
    token->line = line;
    token->column = column;

    uint32_t len = tk->position - pos;
    gchar* str = g_strndup(tk->src + pos, len);

    if (number) {
        token->reg = strtol(str, NULL, 10);
    } else {
        if (g_hash_table_contains(tk->registers, str)) {
            token->reg = (gsize) g_hash_table_lookup(tk->registers, str);
        } else {
            FATAL("Invalid Register Name: %s (%d:%d)\n", str, tk->line, tk->column)
        }
    }

    g_free(str);
    g_queue_push_tail(tk->tokens, token);
}


uint32_t tk_remain(tokenizer_t* tk) {
    return tk->srclen - tk->position;
}
