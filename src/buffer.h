#ifndef ASM_BUFFER_H
#define ASM_BUFFER_H

#include <mips-as/prelude.h>

typedef struct buffer {
    uint8_t* data;
    uint32_t size;
    uint32_t capacity;
    uint32_t base;
} buffer_t;

buffer_t buffer_create();
uint32_t buffer_push_aligned(buffer_t* buff, uint8_t* data, uint32_t len);
void buffer_fit(buffer_t* buff);

#endif //ASM_BUFFER_H
