#include <stdbool.h>

typedef struct __ringbuffer ringbuf_t;



ringbuf_t *ringbuf_create(const unsigned count);


void ringbuf_free(ringbuf_t *r);

int ringbuf_sp_enqueue(ringbuf_t *r, void *obj);

int ringbuf_sc_dequeue(ringbuf_t *r, void **obj_p);

int ringbuf_mc_dequeue(ringbuf_t *r, void **obj_p);

bool ringbuf_is_full(const ringbuf_t *r);

bool ringbuf_is_empty(const ringbuf_t *r);
