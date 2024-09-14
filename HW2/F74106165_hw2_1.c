#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>

int KA(int kx, int ky, int ax, int ay, int *A, int *K, int d, int n){
    return K[kx*d+ky]*A[ax*n+ay];
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
int calculate(int x, int y, int *A, int *K, int d, int m, int n){
    int ans = 0;
    int dd = ((d-1)/2);
    for(int i=-dd;i<=dd;i++){
        for(int j=-dd;j<=dd;j++){
            ans += KA(dd+i,dd+j,region(x+i,m),region(y+j,n),A,K,d,n);
        }
    }

    ans /= (d*d);
    return ans;
}

int main( int argc, char *argv[]){
    FILE* fp;
    char buffer[256];
    int t, d, n, m, rcv_num, last_procs;
    int *A, *K, *rcv_nums, *disp;
    int myid, numprocs;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    if(myid == 0){
        scanf("%s",buffer);
        fp = fopen(buffer, "r");
        fscanf(fp,"%d",&t);
        fscanf(fp,"%d %d",&n, &m);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    A = (int *)malloc((n*m) * sizeof(int));
    if(myid == 0){
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                fscanf(fp, "%d", &A[i*n+j]);
            }
        }
        fscanf(fp,"%d",&d);
    }
    MPI_Bcast(&d, 1, MPI_INT, 0, MPI_COMM_WORLD);
    K = (int *)malloc((d*d) * sizeof(int));
    if(myid == 0){
        for(int i=0;i<d;i++){
            for(int j=0;j<d;j++){
                fscanf(fp, "%d", &K[i*d+j]);
            }
        }
        fclose(fp);
    }   
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(K, (d*d), MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(A, (m*n), MPI_INT, 0, MPI_COMM_WORLD);

    rcv_num = (m+numprocs-1)/numprocs;
    last_procs = 0;
    rcv_nums = (int *)malloc(numprocs * sizeof(int));
    disp = (int *)malloc(numprocs * sizeof(int));
    memset(rcv_nums,0,numprocs*sizeof(rcv_nums[0]));
    memset(disp,0,numprocs*sizeof(disp[0]));
    while(rcv_num * last_procs < m){
        rcv_nums[last_procs] = rcv_num*n;
        disp[last_procs] = last_procs*rcv_num*n;
        last_procs++;
    }
    last_procs--;
    rcv_nums[last_procs] = (m-(rcv_num*last_procs))*n;
    int add = myid*rcv_num;
    if(myid == last_procs){
        rcv_num = n-(rcv_num*myid);
    }
    if(myid > last_procs){
        rcv_num = 0;
    }

    while(t > 0){
        t--;
        int *Atmp = (int *)malloc((n*rcv_num) * sizeof(int));
        if(rcv_num != 0){
            for(int i=0;i<rcv_num;i++){
                for(int j=0;j<n;j++){
                    Atmp[i*n+j] = calculate(i+add,j,A,K,d,m,n);
                }
            }
        }
        MPI_Allgatherv(Atmp, (n*rcv_num), MPI_INT, A, rcv_nums, disp, MPI_INT, MPI_COMM_WORLD);
        free(Atmp);
    }

    
    if(myid == 0){
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                printf("%d ",A[i*n+j]);
            }
        }
        
    }
    free(K);
    free(A);
    free(rcv_nums);
    free(disp);
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    return 0;
}

