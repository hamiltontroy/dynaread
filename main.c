#include <stdio.h>  // printf
#include <unistd.h> // sleep
#include "thpool.h"

void task(void *arg)
{
    printf("%d\n", *(int*)arg);
    sleep(1);
}

int main()
{
    threadpool
        thpool;
    int
        a = 1111;

    // Making threadpool with 2 threads
    thpool = thpool_init(2);

    // Adding 20 tasks to threadpool
    for (int i = 0; i < 20; i++)
    {
        thpool_add_work(thpool, task, &a);
    }

    // (blocking) Waits for threads residing within pool to finish tasks
    thpool_wait(thpool);

    // Deallocates all threads to free memory
    thpool_destroy(thpool);
}
