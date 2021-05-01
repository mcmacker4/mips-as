#include <mips-as/prelude.h>

#include "parse/tokenizer.h"

int main(int argc, const char** argv) {

    if (argc < 2) {
        g_printerr("Missing file name.");
        return 1;
    }

    gchar* src;
    GError* err = NULL;
    if (g_file_get_contents(argv[1], &src, NULL, &err)) {
        GQueue* tokens = tokenize(src);
        g_queue_foreach(tokens, print_token, NULL);
        g_free(src);
        g_queue_free_full(tokens, free_token);
    } else {
        g_printerr("%s\n", err->message);
        g_error_free(err);
    }

    return 0;
}
