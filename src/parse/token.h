#ifndef ASM_TOKEN_H
#define ASM_TOKEN_H

#include <stdint.h>
#include <glib.h>

typedef enum tokentype {
    TK_DIRECTIVE,
    TK_LABEL,
    TK_SYMBOL,
    TK_REGISTER,
    TK_NUMBER,
    TK_COMMA,
    TK_NEWLINE,
    TK_STRING,
} tokentype_t;

typedef struct token {
    tokentype_t type;
    union {
        const char* str;
        uint32_t num;
        uint32_t reg;
    };

    struct {
        uint32_t line;
        uint32_t column;
    };
} token_t;

static void token_free(gpointer t) {
    token_t* token = (token_t*) t;
    switch (token->type) {
        case TK_DIRECTIVE:
        case TK_LABEL:
        case TK_SYMBOL:
        case TK_STRING:
            g_free((gpointer) token->str);
            break;
        default:
            break;
    }
    g_free(token);
}

static void print_token(gpointer t, gpointer d)  {
    token_t* token = (token_t*) t;
    FILE* file = (FILE*) d;
    switch (token->type) {
        case TK_DIRECTIVE:
            fprintf(file, "DIRECTIVE(%s)", token->str);
            break;
        case TK_LABEL:
            fprintf(file, "LABEL(%s:)", token->str);
            break;
        case TK_SYMBOL:
            fprintf(file, "MNEMONIC(%s)", token->str);
            break;
        case TK_REGISTER:
            fprintf(file, "REGISTER($%d)", token->reg);
            break;
        case TK_NUMBER:
            fprintf(file, "NUMBER(0x%08x)", token->num);
            break;
        case TK_STRING: {
            gchar *str = g_strescape(token->str, NULL);
            fprintf(file, "STRING(%s)", str);
            g_free(str);
            break;
        }
        case TK_COMMA:
            fprintf(file, "COMMA(,)");
            break;
        case TK_NEWLINE:
            fprintf(file, "NEWLINE(\\n)");
            break;
    }
}

#endif // ASM_TOKEN_H
