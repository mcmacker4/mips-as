#include "parser.h"

#include "tokenizer.h"
#include <assert.h>

token_t* peek(parser_t* parser) {
    return (token_t*) g_queue_peek_head(parser->tokens);
}

static inline tokentype_t peektype(parser_t* parser) {
    return peek(parser)->type;
}

token_t* consume(parser_t* parser) {
    return g_queue_pop_head(parser->tokens);
}

void ignore(parser_t* parser) {
    token_t* token = g_queue_pop_head(parser->tokens);
    token_free(token);
}

void ignore_expected(parser_t* parser, tokentype_t type) {
    tokentype_t next = peektype(parser);
    if (next == type) {
        ignore(parser);
    } else {
        g_printerr("Unexpected token ");
        print_token(peek(parser), stderr);
        g_printerr(" at %d:%d\n", peek(parser)->line, peek(parser)->column);
        exit(-1);
    }
}

bool remain(parser_t* parser) {
    return !g_queue_is_empty(parser->tokens);
}

void skip_newlines(parser_t* parser) {
    while (remain(parser) && peektype(parser) == TK_NEWLINE) {
        ignore(parser);
    }
}

static inline bool is_valid_directive_arg(tokentype_t type) {
    return type == TK_NUMBER || type == TK_STRING || type == TK_SYMBOL;
}

void read_directive(parser_t* parser) {
    token_t* token = consume(parser);
    assert(token->type == TK_DIRECTIVE);
    statement_t* stmt = g_new(statement_t, 1);
    stmt->type = STMT_DIRECTIVE;
    stmt->directive.name = g_strdup(token->str);

    if (is_valid_directive_arg(peektype(parser))) {
        token_t* argtoken = consume(parser);
        argument_t* arg = g_new(argument_t, 1);
        if (argtoken->type == TK_NUMBER) {
            arg->type = ARG_NUMBER;
            arg->num = argtoken->num;
        } else if (argtoken->type == TK_STRING) {
            arg->type = ARG_STRING;
            arg->str = g_strdup(argtoken->str);
        } else if (argtoken->type == TK_SYMBOL) {
            arg->type = ARG_SYMBOL;
            arg->sym = g_strdup(argtoken->str);
        }
        token_free(argtoken);
        stmt->directive.argument = arg;
    } else {
        stmt->directive.argument = NULL;
    }

    ignore_expected(parser, TK_NEWLINE);
    token_free(token);

    g_queue_push_tail(parser->statements, stmt);
}

void read_label(parser_t* parser) {
    token_t* token = consume(parser);
    assert(token->type == TK_LABEL);

    statement_t* stmt = g_new(statement_t, 1);
    stmt->type = STMT_LABEL;
    stmt->label.name = g_strdup(token->str);

    token_free(token);
    g_queue_push_tail(parser->statements, stmt);
}

static inline bool is_valid_instruction_arg(tokentype_t type) {
    switch (type) {
        case TK_SYMBOL:
        case TK_REGISTER:
        case TK_NUMBER:
            return true;
        default:
            return false;
    }
}


void read_instruction(parser_t* parser) {
    statement_t* stmt = g_new(statement_t, 1);
    stmt->type = STMT_INSTRUCTION;

    token_t* nametoken = consume(parser);
    assert(nametoken->type == TK_SYMBOL);
    stmt->instruction.name = g_strdup(nametoken->str);
    stmt->instruction.arguments = g_array_new(FALSE, FALSE, sizeof(argument_t));
    g_array_set_clear_func(stmt->instruction.arguments, argument_free_content);

    while (is_valid_instruction_arg(peektype(parser))) {

        token_t* token = consume(parser);
        argument_t arg;

        if (token->type == TK_SYMBOL) {
            arg.type = ARG_SYMBOL;
            arg.str = g_strdup(token->str);
        } else if (token->type == TK_NUMBER) {
            arg.type = ARG_NUMBER;
            arg.num = token->num;
        } else if (token->type == TK_REGISTER) {
            arg.type = ARG_REGISTER;
            arg.reg = token->reg;
        } else {
            FATAL("This should never happen.")
        }

        g_array_append_val(stmt->instruction.arguments, arg);
        token_free(token);

        if (!remain(parser) || peektype(parser) == TK_NEWLINE) {
            break;
        }

        if (peektype(parser) == TK_COMMA) {
            ignore(parser);
        } else {
            g_printerr("Unexpected token ");
            print_token(peek(parser), stderr);
            g_printerr(" at %d:%d\n", peek(parser)->line, peek(parser)->column);
        }

    }

    g_queue_push_tail(parser->statements, stmt);

    if (remain(parser)) {
        ignore_expected(parser, TK_NEWLINE);
    }

    token_free(nametoken);
}

GQueue* parse(const char* src) {
    parser_t* parser = g_new(parser_t, 1);

    GQueue* statements = g_queue_new();
    parser->tokens = tokenize(src);
    parser->statements = statements;

    skip_newlines(parser);

    while (remain(parser)) {

        switch (peektype(parser)) {
            case TK_DIRECTIVE:
                read_directive(parser);
                break;
            case TK_LABEL:
                read_label(parser);
                break;
            case TK_SYMBOL:
                read_instruction(parser);
                break;
            default:
                g_printerr("Unexpected token: ");
                print_token(peek(parser), stderr);
                g_printerr(" at %d:%d\n", peek(parser)->line, peek(parser)->column);
                exit(-1);
        }

        skip_newlines(parser);
    }

    g_queue_free(parser->tokens);
    g_free(parser);

    return statements;
}

void argument_free_content(gpointer a) {
    argument_t* arg = (argument_t*) a;
    if (arg->type == ARG_SYMBOL) {
        g_free((gpointer) arg->sym);
    } else if (arg->type == ARG_STRING) {
        g_free((gpointer) arg->str);
    }
}

void argument_free(gpointer a) {
    if (a != NULL) {
        argument_free_content(a);
        g_free(a);
    }
}
void statement_free(gpointer s) {
    statement_t* stmt = (statement_t*) s;
    switch (stmt->type) {
        case STMT_DIRECTIVE:
            g_free((gpointer) stmt->directive.name);
            argument_free(stmt->directive.argument);
            break;
        case STMT_INSTRUCTION:
            g_free((gpointer) stmt->instruction.name);
            g_array_free(stmt->instruction.arguments, TRUE);
            break;
        case STMT_LABEL:
            g_free((gpointer) stmt->label.name);
            break;
    }
    g_free(stmt);
}
