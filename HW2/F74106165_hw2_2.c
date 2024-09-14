#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>
struct Path
{
    int dest;
    int weight;
};
int main( int argc, char *argv[]){
    FILE* fp;
    int myid, numprocs, last_procs=0;
    int* gathered_dest;
    int* gathered_weight;
    char buffer[128];
    int n, row;
    int maxweight = 9999999;
    int *count, *distance, *rcvcount;
    struct Path *adj, *rcvadj;
    int destnow, weightnow;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    if(myid == 0){
        scanf("%s",buffer);
        fp = fopen(buffer, "r");
        fgets(buffer, 256, fp);
        sscanf(buffer,"%d",&n);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    distance = (int *)malloc(n * sizeof(int));
    for(int i=1;i<n;i++){
        distance[i] = maxweight;
    }
    distance[0] = 0;
    if(n > 2000){
        row = 2000;
    }else{
        row = n;
    }
    int rcv_num = (n+numprocs-1)/numprocs;
    rcvadj = (struct Path*)malloc((rcv_num*row) * sizeof(struct Path));  
    rcvcount = (int *)malloc(rcv_num * sizeof(int));
    if(myid == 0){   
        count = (int *)malloc(n * sizeof(int));
        memset(count,0,n*sizeof(count[0]));
        adj = (struct Path*)malloc((n*row) * sizeof(struct Path));
        int start;
        while (fgets(buffer, 256, fp) != NULL) {
            sscanf(buffer,"%d",&start);
            sscanf(buffer,"%d %d %d",&start,&adj[start*row+count[start]].dest,&adj[start*row+count[start]].weight);
            count[start]++;
        }
        fclose(fp);
        gathered_dest = (int*)malloc(numprocs * sizeof(int));
        gathered_weight = (int*)malloc(numprocs * sizeof(int));
    }

    int add = rcv_num*myid;
    int turn = 1;
    MPI_Scatter(count, (rcv_num), MPI_INT, rcvcount, rcv_num, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(adj, (rcv_num*row)*sizeof(struct Path), MPI_BYTE, rcvadj, (rcv_num*row)*sizeof(struct Path), MPI_BYTE, 0, MPI_COMM_WORLD);
    while(rcv_num * last_procs < n){
        last_procs++;
    }
    last_procs--;
    if(myid > last_procs){
        rcv_num = 0;
    }
    if(myid == last_procs){
        rcv_num = n-(rcv_num*myid);
    }
    
    while(turn < n){
        destnow = 100000;
        weightnow = maxweight;
        for(int i=0;i<rcv_num;i++){
            if(distance[add+i] != maxweight){
                for(int j=0;j<rcvcount[i];j++){
                    struct Path take = rcvadj[i*row+j];
                    if(distance[take.dest] == maxweight){
                        int w = take.weight+distance[add+i];
                        if(w < weightnow){
                            destnow = take.dest;
                            weightnow = w;
                        }
                    }
                }
            }
        }
        turn++;
        MPI_Gather(&destnow, 1, MPI_INT, gathered_dest, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Gather(&weightnow, 1, MPI_INT, gathered_weight, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if(myid == 0){
            for(int i=0;i<numprocs;i++){
                if(gathered_weight[i] < weightnow){
                    weightnow = gathered_weight[i];
                    destnow = gathered_dest[i];
                }
            }
        }
        MPI_Bcast(&destnow, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&weightnow, 1, MPI_INT, 0, MPI_COMM_WORLD);
        distance[destnow] = weightnow;
    }

    if(myid == 0){
        for(int i=0;i<n;i++){
            printf("%d ",distance[i]);
        }
    }
    free(adj);
    free(rcvadj);
    free(count);
    free(rcvcount);
    free(distance);
    
	MPI_Finalize();
    return 0;
	       
}
    
