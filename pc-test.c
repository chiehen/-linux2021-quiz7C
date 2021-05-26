#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#include "ringbuffer.h"

#define RING_SIZE (1 << 6)

static void test_basic_operations()
{
    ringbuf_t *r = ringbuf_create(RING_SIZE);
    assert(r);

    assert(ringbuf_is_empty(r) ==  true);
    
    int result;
    /* enqueue on item and then pop out */
    {
        int in = 1;
        void *out;
        {
            result = ringbuf_sp_enqueue(r, *(void **) &in);
            assert(result == 0);
        }
        assert(ringbuf_is_empty(r) == false);
        {
            result = ringbuf_sc_dequeue(r, &out);
            assert(result == 0);
        }
        assert(*(int *) &out == in);
    }
    assert(ringbuf_is_empty(r) ==  true);

    /* push many items */
    for (int i = 0; i < RING_SIZE - 1; ++i) {
        int in = i;
        result = ringbuf_sp_enqueue(r, *(void **) &in); 
        assert(result == 0);
    }
    assert(ringbuf_is_full(r) == true);

    /* pop many items */
    for (int i = 0; i < RING_SIZE - 1; ++i) {
        void *out;
        result = ringbuf_sc_dequeue(r, &out);
        assert(result == 0);
        assert(*(int *) &out == i);
    }
    assert(ringbuf_is_empty(r) == true);

    ringbuf_free(r);
}

# define THREAD_COUNT (50 * 1.5 * 1000 * 100)
# define PRODUCER_COUNT 1

typedef struct __RingTest {
    atomic_int consumer_count;
    atomic_int producer_count;
    ringbuf_t *r;
} RingTest;

static void *test_consumer(void *arg)
{
    RingTest *test = (RingTest *)arg;
    while (atomic_load(&test->consumer_count) < THREAD_COUNT) {
        if (!ringbuf_is_empty(test->r)) {
            void *out;
            atomic_fetch_add(&test->consumer_count, 1);
            int result = ringbuf_sc_dequeue(test->r, &out);
            assert(result == 0);
        }
    }
    return NULL;
}

static void *test_producer(void *arg)
{
    RingTest *test = (RingTest *) arg;
    assert(test->r);
    while (atomic_load(&test->producer_count) < THREAD_COUNT) {
        if(!ringbuf_is_full(test->r)) {
            int in = atomic_fetch_add(&test->producer_count, 1);
            // if (in >= THREAD_COUNT) break;
            int result = ringbuf_sp_enqueue(test->r, *(void **) &in);
            assert(result == 0);
        }
    }
    return NULL;
}

static void stress_test()
{
    RingTest test;
    atomic_init(&test.consumer_count, 0);
    atomic_init(&test.producer_count, 0);

    test.r = ringbuf_create(RING_SIZE);
    assert(test.r);

    /* thread creation */
    pthread_t consumer;
    pthread_t producers[PRODUCER_COUNT];
    {
        int p_result = 
            pthread_create(&consumer, NULL, test_consumer, &test);
        assert(p_result == 0);
    }
    for (int i = 0; i < PRODUCER_COUNT; ++i) {
        int p_result = pthread_create(&producers[i], NULL, 
                test_producer, &test);
        assert(p_result == 0);
    }

    for (int i = 0; i < PRODUCER_COUNT; ++i) {
        int p_result = pthread_join(producers[i], NULL);
        assert(p_result == 0);
    }
    {
        int p_result = pthread_join(consumer, NULL);
        assert(p_result == 0);
    }

    assert(ringbuf_is_empty(test.r) == true);

    ringbuf_free(test.r);
}
    
    
int main(void)
{
    printf("** Basic operations **\n");
    test_basic_operations();
    printf("Verified OK!\n\n");

    printf("** Stress test **\n");
    stress_test();
    printf("Verified OK!\n\n");

    return 0;
}
