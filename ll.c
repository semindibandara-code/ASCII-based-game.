#include "ll.h"
#include <stdlib.h>
#include <string.h>

void ll_init(struct LLStack *s)
{
    (*s).head = NULL;
    (*s).count = 0;
}

void ll_clear(struct LLStack *s)
{
    struct LLNode *n = (*s).head;
    while (n != NULL) {
        struct LLNode *next = (*n).next;
        if ((*n).data != NULL) free((*n).data);
        free(n);
        n = next;
    }
    (*s).head = NULL;
    (*s).count = 0;
}

int ll_push(struct LLStack *s, const void *data, size_t size)
{
    struct LLNode *n;
    void *buf;

    n = (struct LLNode*)malloc(sizeof(struct LLNode));
    if (n == NULL) return 1;
    buf = malloc(size);
    if (buf == NULL) { free(n); return 2; }
    memcpy(buf, data, size);

    (*n).data = buf;
    (*n).size = size;
    (*n).next = (*s).head;
    (*s).head = n;
    (*s).count += 1;
    return 0;
}

int ll_pop_take(struct LLStack *s, void **out_data, size_t *out_size)
{
    struct LLNode *n;
    if ((*s).head == NULL) return 1;

    n = (*s).head;
    (*s).head = (*n).next;
    (*s).count -= 1;

    *out_data = (*n).data;  /* caller will free */
    *out_size = (*n).size;
    free(n);
    return 0;
}

size_t ll_count(const struct LLStack *s)
{
    return s->count;
}

