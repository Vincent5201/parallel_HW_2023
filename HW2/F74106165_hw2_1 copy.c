#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>

int calculate(int x, int y, int *K, int *rcvA, int *rcvPA, int *rcvNA, int rcv_num, int n, int d, int id, int other_rcvnum, int last_procs){
    int ans = 0;
    int dd = ((d-1)/2);
    for(int i=-dd;i<=dd;i++){
        for(int j=-dd;j<=dd;j++){
            int xx = x+i;
            int yy = y+j;
            if(yy >= n){
                yy-=n;
            }
            if(yy < 0){
                yy+=n;
            }
            if(xx >= rcv_num){
                ans += K[(dd+i)*d+(dd+j)] * rcvNA[(xx-rcv_num)*n+yy];
            }else if(xx < 0){
                ans += K[(dd+i)*d+(dd+j)] * rcvPA[(xx+other_rcvnum)*n+yy];
            }else{
                ans += K[(dd+i)*d+(dd+j)] * rcvA[xx*n+yy];
            }
        }
    }
    ans /= (d*d);
 
    return ans;
}

int main( int argc, char *argv[]){
    int t, d, n, m, rcv_num, last_procs, other_rcvnum;
    int *A, *K, *rcvA, *rcvPA, *rcvNA, *rcv_nums, *disp;
    int myid, numprocs;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    
    if(myid == 0){
        char buffer[256];
        scanf("%s",buffer);
        FILE* fp = fopen(buffer, "r");
        fscanf(fp,"%d",&t);
        fscanf(fp,"%d %d",&n, &m);
        A = (int *)malloc((n*m) * sizeof(int));
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                fscanf(fp, "%d", &A[i*n+j]);
            }
        }
        fscanf(fp,"%d",&d);
        K = (int *)malloc((d*d) * sizeof(int));
        for(int i=0;i<d;i++){
            for(int j=0;j<d;j++){
                fscanf(fp, "%d", &K[i*d+j]);
            }
        }
        fclose(fp);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&d, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(myid != 0){
        K = (int *)malloc((d*d) * sizeof(int));
    }
    MPI_Bcast(K, (d*d), MPI_INT, 0, MPI_COMM_WORLD);
    rcv_num = (m+numprocs-1)/numprocs;
    last_procs = 0;
    if(myid == 0){
        rcv_nums = (int *)malloc(numprocs * sizeof(int));
        disp = (int *)malloc(numprocs * sizeof(int));
        memset(rcv_nums,0,numprocs*sizeof(rcv_nums[0]));
        memset(disp,0,numprocs*sizeof(disp[0]));
    }else{
        rcv_nums = NULL;
        disp = NULL;
    }
    while(rcv_num * last_procs < m){
        if(myid == 0){
            rcv_nums[last_procs] = rcv_num*n;
            disp[last_procs] = last_procs*rcv_num*n;
        }
        last_procs++;
    }
    last_procs--;
    
    if(myid > last_procs){
        rcv_num = 0;
    }
    other_rcvnum = (d-1)/2;
    if(myid == last_procs){
        rcv_num = n-(rcv_num*myid);
    }
    
    rcvA = (int *)malloc((n*rcv_num) * sizeof(int));
    MPI_Scatterv(A, rcv_nums, disp, MPI_INT, rcvA, (rcv_num*n), MPI_INT, 0, MPI_COMM_WORLD);
    
    while(t > 0){
        t--;
        MPI_Barrier(MPI_COMM_WORLD);
        if(rcv_num != 0){
            rcvNA = (int *)malloc((n*other_rcvnum) * sizeof(int));
            rcvPA = (int *)malloc((n*other_rcvnum) * sizeof(int));
            if(myid == 0){
                MPI_Sendrecv(rcvA+(rcv_num-other_rcvnum)*n,(other_rcvnum*n),MPI_INT,myid+1,myid,rcvPA,(other_rcvnum*n),MPI_INT,last_procs,last_procs,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }else if(myid == last_procs){
                MPI_Sendrecv(rcvA+(rcv_num-other_rcvnum)*n,(other_rcvnum*n),MPI_INT,0,myid,rcvPA,(other_rcvnum*n),MPI_INT,myid-1,myid-1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }else{
                MPI_Sendrecv(rcvA+(rcv_num-other_rcvnum)*n,(other_rcvnum*n),MPI_INT,myid+1,myid,rcvPA,(other_rcvnum*n),MPI_INT,myid-1,myid-1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        }                
        MPI_Barrier(MPI_COMM_WORLD);
        if(rcv_num != 0){
            if(myid == 0){
                MPI_Sendrecv(rcvA,(other_rcvnum*n),MPI_INT,last_procs,myid,rcvNA,(other_rcvnum*n),MPI_INT,myid+1,myid+1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }else if(myid == last_procs){
                MPI_Sendrecv(rcvA,(other_rcvnum*n),MPI_INT,myid-1,myid,rcvNA,(other_rcvnum*n),MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }else{
                MPI_Sendrecv(rcvA,(other_rcvnum*n),MPI_INT,myid-1,myid,rcvNA,(other_rcvnum*n),MPI_INT,myid+1,myid+1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        if(rcv_num != 0){
            int *Atmp = (int *)malloc((n*rcv_num) * sizeof(int));
            for(int i=0;i<rcv_num;i++){
                for(int j=0;j<n;j++){
                    Atmp[i*n+j] = calculate(i,j,K,rcvA,rcvPA,rcvNA,rcv_num,n,d,myid,other_rcvnum,last_procs);
                }
            }
            
            for(int i=0;i<rcv_num;i++){
                for(int j=0;j<n;j++){
                    rcvA[i*n+j] = Atmp[i*n+j];
                }
            }
            free(Atmp);
            free(rcvNA);
            free(rcvPA);
        }
    }

    MPI_Gatherv(rcvA, (rcv_num*n), MPI_INT, A, rcv_nums, disp, MPI_INT, 0, MPI_COMM_WORLD);
           
    
    if(myid == 0){
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                printf("%d ",i*n+j);
            }
        }
    }
    
        
    
    free(rcvA);
    free(K);
    if(myid == 0){
        free(A);
        free(rcv_nums);
        free(disp);
    }
    return 0;
}

