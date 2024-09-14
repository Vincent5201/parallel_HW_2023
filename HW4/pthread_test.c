#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

long thread_count;

void *Hello(void *rank);

int main( int argc, char *argv[]){
    long thread;
    thread_count = strtol(argv[1], NULL, 10);
    pthread_t* thread_handles = malloc(thread_count * sizeof(thread_count));
    for(thread = 0; thread < thread_count; thread++){
        int *args = malloc(2 * sizeof(thread_count));
        args[0] = thread;
        args[1] = thread*thread;
        pthread_create(&thread_handles[thread], NULL, Hello, (void*)(intptr_t)args);
    }

    for(long thread = 0; thread < thread_count; thread++){
        pthread_join(thread_handles[thread], NULL);
    }

    free(thread_handles);

    return 0;
}

void *Hello(void* rank){
    int *my_args = (int*)(intptr_t)rank;
    printf("%d ",my_args[0]);
    return NULL;
}

