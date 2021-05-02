#ifndef ASM_PARSER_H
#define ASM_PARSER_H

#include <mips-as/prelude.h>

typedef enum statement_type {
    STMT_DIRECTIVE,
    STMT_INSTRUCTION,
    STMT_LABEL,
} statement_type_t;

typedef enum argument_type {
    ARG_NUMBER,
    ARG_REGISTER,
    ARG_SYMBOL,
    ARG_STRING,
} argument_type_t;

typedef struct argument {
    argument_type_t type;
    union {
        uint32_t num;
        uint32_t reg;
        const char* sym;
        const char* str;
    };
} argument_t;

void argument_free_content(gpointer a);
void argument_free(gpointer a);

typedef struct statement {
    statement_type_t type;

    union {

        struct {
            const char* name;
            argument_t* argument;
        } directive;

        struct {
            const char* name;
            GArray* arguments;
        } instruction;

        struct {
            const char* name;
        } label;

    };

} statement_t;

void statement_free(gpointer s);

typedef struct parser {
    GQueue* tokens;
    GQueue* statements;
} parser_t;

GQueue* parse(const char* src);

#endif //ASM_PARSER_H
