#include "buffer.h"

#include <memory.h>
#include <glib.h>

#define BUFF_INITIAL_SIZE (4 * 4)

buffer_t buffer_create() {
    buffer_t buff;
    buff.data = calloc(BUFF_INITIAL_SIZE, 1);
    buff.size = 0;
    buff.capacity = BUFF_INITIAL_SIZE;
    return buff;
}

#define ALIGN(x, n) ((x) % (n) == 0 ? (x) : (x) + ((n) - (x) % (n)))

static inline void buffer_resize(buffer_t* buff, uint32_t min) {
    uint32_t newsize1 = ALIGN(buff->size * 3 / 2, 4);
    uint32_t newsize2 = ALIGN(min, 4);
    buff->capacity = MAX(newsize1, newsize2);
    buff->data = realloc(buff->data, buff->capacity);
}

uint32_t buffer_push_aligned(buffer_t* buff, uint8_t* data, uint32_t len) {
    uint32_t addr = ALIGN(buff->size, 4);
    if (buff->capacity <= addr + len) {
        buffer_resize(buff, addr + len);
    }
    memcpy(buff->data, data, len);
    return addr + buff->base;
}

void buffer_fit(buffer_t* buff) {
    // make size multiple of 4 rounding up
    buff->capacity = ALIGN(buff->size, 4);
    buff->data = realloc(buff->data, buff->capacity);
}
