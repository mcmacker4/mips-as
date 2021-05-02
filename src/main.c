#include <mips-as/prelude.h>

#include "parse/parser.h"

void print_arg(argument_t* arg) {
    switch (arg->type) {
        case ARG_NUMBER:
            printf("%d", arg->num);
            break;
        case ARG_REGISTER:
            printf("$%d", arg->reg);
            break;
        case ARG_SYMBOL:
            printf("%s", arg->sym);
            break;
        case ARG_STRING:
            printf("\"%s\"", arg->str);
            break;
    }
}

void print_stmt(gpointer s, gpointer d) {
    statement_t* stmt = (statement_t*) s;
    if (stmt->type == STMT_DIRECTIVE) {
        printf(".%s", stmt->directive.name);
        if (stmt->directive.argument != NULL) {
            printf(" ");
            print_arg(stmt->directive.argument);
        }
    } else if (stmt->type == STMT_INSTRUCTION) {
        printf("%s", stmt->instruction.name);
        for (size_t i = 0; i < stmt->instruction.arguments->len; i++) {
            printf(" ");
            print_arg(&g_array_index(stmt->instruction.arguments, argument_t, i));
        }
    } else if (stmt->type == STMT_LABEL) {
        printf("%s:", stmt->label.name);
    }
    printf("\n");
}

int main(int argc, const char** argv) {

    if (argc < 2) {
        g_printerr("Missing file name.");
        return 1;
    }

    gchar* src;
    GError* err = NULL;

    if (!g_file_get_contents(argv[1], &src, NULL, &err)) {
        g_printerr("%s\n", err->message);
        g_error_free(err);
        return 1;
    }

    GQueue* statements = parse(src);
    g_queue_foreach(statements, print_stmt, NULL);

    g_queue_free_full(statements, statement_free);
    g_free(src);

    return 0;
}
