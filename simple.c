#include <assert.h>
#include <stdio.h>

#include "ringbuffer.h"

int main(void)
{
    ringbuf_t *r = ringbuf_create((1 << 6));
    if (!r) {
        printf("Fail to create ring buffer.\n");
        return -1;
    }

    int in;
    for (in = 0; !ringbuf_is_full(r); in++)
        ringbuf_sp_enqueue(r, (void *) &in);

    for (int i = 0; !ringbuf_is_empty(r); i++) {
        void *obj;
        ringbuf_sc_dequeue(r, &obj);
        printf("%d\n", *(int *) obj);
        //assert(i == *(int *) obj);
    }

    ringbuf_free(r);
    return 0;
}
