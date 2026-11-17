/* ll.h — simple generic LIFO stack */
#ifndef LL_H
#define LL_H

#include <stddef.h>

struct LLNode {
    void *data;
    size_t size;
    struct LLNode *next;
};

struct LLStack {
    struct LLNode *head;
    size_t count;
};

/* init/clear */
void ll_init(struct LLStack *s);
void ll_clear(struct LLStack *s);

/* push a COPY of [data,size] onto stack; returns 0 on success */
int ll_push(struct LLStack *s, const void *data, size_t size);

/* pop and GIVE BACK the data buffer (caller must free(*out_data)); returns 0 on success */
int ll_pop_take(struct LLStack *s, void **out_data, size_t *out_size);

/* peek size or emptiness helpers */
size_t ll_count(const struct LLStack *s);

#endif

