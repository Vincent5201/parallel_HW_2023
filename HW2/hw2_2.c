#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Path
{
    int dest;
    int weight;
};

void swapc(struct Path* a, struct Path* b) {
    struct Path t = *a;
    *a = *b;
    *b = t;
}

int partition(struct Path arr[], int low, int high) {
    struct Path pivot = arr[high]; 
    int i = (low - 1); 
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].weight < pivot.weight) {
            i++;
            swapc(&arr[i], &arr[j]);
        }else if(arr[j].weight == pivot.weight){
            if(arr[j].dest < pivot.dest){
                i++;
                swapc(&arr[i], &arr[j]);
            }
        }
    }
    swapc(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(struct Path arr[],  int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main(int argc, char *argv[]){
    clock_t start_time = clock();
    clock_t end_time;
    double elapsed_time;
    int n;
    int maxweight = 9999999;
    int *count;
    int *distance;
    int row;
    struct Path *adj;
    char buffer[32];
    FILE* fp = fopen("2_2data\\neib50000.txt", "r");
    fgets(buffer, 256, fp);
    sscanf(buffer,"%d",&n);
    if(n>1000){
        row = 1000;
    }else{
        row = n;
    }
    adj = (struct Path*)malloc((n*row) * sizeof(struct Path));
    count = (int *)malloc(n * sizeof(int));
    memset(count,0,n*sizeof(count[0]));
    while (fgets(buffer, 256, fp) != NULL) {
        int start, tmp;
        sscanf(buffer,"%d",&start);
        sscanf(buffer,"%d %d %d",&tmp,&adj[start*row+count[start]].dest,&adj[start*row+count[start]].weight);
        count[start]++;
    }
    fclose(fp);
    distance = (int *)malloc(n * sizeof(int));
    for(int i=1;i<n;i++){
        distance[i] = maxweight;
    }
    distance[0] = 0;
    for(int i=0;i<n;i++){
        quickSort(adj,i*row,i*row+count[i]-1);
    }
    
    int turn  = 1;
    
    while(turn < n){
        int destnow;
        int weightnow = maxweight;
        for(int i=0;i<n;i++){
            if(distance[i] != maxweight){
                for(int j=0;j<count[i];j++){
                    if(distance[adj[i*row+j].dest] == maxweight){
                        if(adj[i*row+j].weight+distance[i] < weightnow){
                            destnow = adj[i*row+j].dest;
                            weightnow = adj[i*row+j].weight + distance[i];
                        }
                        break;
                    }
                }
            }
        }
        distance[destnow] = weightnow;
        turn++;
    }
    for(int i=0;i<n;i++){
        printf("%d ",distance[i]);
    }
    end_time = clock();
    elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("time: %f sec\n", elapsed_time);

    
    return 0;
}