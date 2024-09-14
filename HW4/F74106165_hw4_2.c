#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long thread_count;
int n, part_min;
int rcv_num, turn, start;
int *arr;
int **count;
pthread_mutex_t mutex;

void *one_thread(void* args);

int main( int argc, char *argv[]){
    long thread;
    thread_count = strtol(argv[1], NULL, 10);
    pthread_t* thread_handles = malloc(thread_count * sizeof(thread_count));
    pthread_mutex_init(&mutex, NULL);   
    FILE* fp;
    char buffer[256];
    scanf("%s",buffer);
    fp = fopen(buffer, "r");
    fscanf(fp,"%d",&n);
    arr = (int *)malloc((n+1) * sizeof(int));
    for(int i=1;i<=n;i++){
        fscanf(fp, "%d", &arr[i]);
    }
    fclose(fp);
    
    arr[0] = 1;
    count = (int **)malloc((n+1) * sizeof(int*));
    for(int i=0;i<=n;i++){
        count[i] = (int *)malloc((n+1) * sizeof(int));
    }
    for(int i=0;i<=n;i++){
        for(int j=0;j<=n;j++){
            count[i][j] = 0;
        }
    }
    
    turn = 2;
    while(turn <= n){    
        for(int i=1;i<=n-turn+1;i++){
            rcv_num = (turn-1+thread_count-1)/thread_count;
            start = i;
            part_min = 200000000;
            for(thread = 0; thread < thread_count; thread++){
                pthread_create(&thread_handles[thread], NULL, one_thread, (void*)thread);
            }
            for(thread = 0; thread < thread_count; thread++){
                pthread_join(thread_handles[thread], NULL);
            }
            count[i][start+turn-1] = part_min;
        }
        turn++;
    }
    pthread_mutex_destroy(&mutex);

    printf("%d", count[1][n]);
    return 0;
}

void *one_thread(void* args){
    long my_rank = (long)args;
    long add = my_rank*rcv_num;
    
    for(int i=start+add;i<start+add+rcv_num && i < start+turn-1;i++){	
        int tmp = arr[start-1]*arr[i]*arr[start+turn-1] + count[start][i] + count[i+1][start+turn-1];
        pthread_mutex_lock(&mutex);
        if(tmp < part_min){
            part_min = tmp;
        }
        pthread_mutex_unlock(&mutex);
    }
    
    
    return NULL;
}