#ifndef ASM_TOKEN_H
#define ASM_TOKEN_H

#include <stdint.h>
#include <glib.h>

typedef enum tokentype {
    TK_DIRECTIVE,
    TK_LABEL,
    TK_MNEM,
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
        uint32_t number;
    };
} token_t;

static void free_token(gpointer t) {
    token_t* token = (token_t*) t;
    switch (token->type) {
        case TK_DIRECTIVE:
        case TK_LABEL:
        case TK_MNEM:
        case TK_STRING:
            g_free((gpointer) token->str);
            break;
        default:
            break;
    }
}

static void print_token(gpointer t, gpointer d)  {
    token_t* token = (token_t*) t;
    switch (token->type) {
        case TK_DIRECTIVE:
            printf("DIRECTIVE(%s)\n", token->str);
            break;
        case TK_LABEL:
            printf("LABEL(%s:)\n", token->str);
            break;
        case TK_MNEM:
            printf("MNEMONIC(%s)\n", token->str);
            break;
        case TK_REGISTER:
            printf("REGISTER($%d)\n", token->number);
            break;
        case TK_NUMBER:
            printf("NUMBER(0x%08x)\n", token->number);
            break;
        case TK_STRING: {
            gchar *str = g_strescape(token->str, NULL);
            printf("STRING(%s)\n", str);
            g_free(str);
            break;
        }
        case TK_COMMA:
            printf("COMMA(,)\n");
            break;
        case TK_NEWLINE:
            printf("NEWLINE(\\n)\n");
            break;
    }
}

#endif // ASM_TOKEN_H
