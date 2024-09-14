#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long thread_count;
int t, d1, d2, n, m, rcv_num;
int *A, *K, *Atmp;

int KA(int kx, int ky, int ax, int ay);
int region(int a, int n);
int calculate(int x, int y);
void *one_turn(void* args);
void *copypaste(void* rank);

int main( int argc, char *argv[]){
    
    long thread;
    thread_count = strtol(argv[1], NULL, 10);
    pthread_t* thread_handles = malloc(thread_count * sizeof(thread_count));
    FILE* fp;
    char buffer[256];
    scanf("%s",buffer);
    fp = fopen(buffer, "r");
    fscanf(fp,"%d",&t);
    fscanf(fp,"%d %d",&n, &m);
    A = (int *)malloc((n*m) * sizeof(int));
    Atmp = (int *)malloc((n*m) * sizeof(int));
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            fscanf(fp, "%d", &A[i*n+j]);
        }
    }
    fscanf(fp,"%d %d",&d1, &d2);
    K = (int *)malloc((d1*d2) * sizeof(int));
    for(int i=0;i<d1;i++){
        for(int j=0;j<d2;j++){
            fscanf(fp, "%d", &K[i*d2+j]);
        }
    }
    fclose(fp);
    rcv_num = (m+thread_count-1)/thread_count;
    
    while(t > 0){
        t--;
        for(thread = 0; thread < thread_count; thread++){
            pthread_create(&thread_handles[thread], NULL, one_turn, (void*)thread);
        }
        for(long thread = 0; thread < thread_count; thread++){
            pthread_join(thread_handles[thread], NULL);
        }
        for(thread = 0; thread < thread_count; thread++){
            pthread_create(&thread_handles[thread], NULL, copypaste, (void*)thread);
        }
        for(long thread = 0; thread < thread_count; thread++){
            pthread_join(thread_handles[thread], NULL);
        }

    }
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            printf("%d ",A[i*n+j]);
        }
    }
    
    return 0;
}

void *one_turn(void* rank){
    long my_rank = (long)rank;
    long add = my_rank*rcv_num;
    for(int i=add; i < add+rcv_num && i < m; i++){
        for(int j=0;j<n;j++){
            Atmp[i*n+j] = calculate(i,j);
        }
    }
    return NULL;
}

void *copypaste(void* rank){
    long my_rank = (long)rank;
    long add = my_rank*rcv_num;
    for(int i=add; i < add+rcv_num && i < m; i++){
        for(int j=0;j<n;j++){
            A[i*n+j] = Atmp[i*n+j];
        }
    }
    return NULL;
}

int KA(int kx, int ky, int ax, int ay){
    return K[kx*d2+ky]*A[ax*n+ay];
}
int region(int a, int n){
    if(a < 0){
        return a+n;
    }
    if(a >= n){
        return a-n;
    }
    return a;
}
int calculate(int x, int y){
    int ans = 0;
    int dd1 = ((d1-1)/2);
    int dd2 = ((d2-1)/2);
    for(int i=-dd1;i<=dd1;i++){
        for(int j=-dd2;j<=dd2;j++){
            ans += KA(dd1+i,dd2+j,region(x+i,m),region(y+j,n));
        }
    }
    ans /= (d1*d2);
    return ans;
}