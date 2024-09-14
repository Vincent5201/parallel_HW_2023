#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>

int deal(int *tgt, int p, int* count, int* parts, int n, int m, int get);

int main( int argc, char *argv[]){
    int finish=0, start, myid, numprocs, myans, ans;
    int n, m;
    int *count;
    int *parts;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    start = 0;  
    while(finish == 0){
        if(myid == 0 && start == 0){   
            char contents[255];
            FILE* fpn = fopen("input/filename/a.txt", "r");
	        fscanf(fpn, "%s", contents);
            FILE* fp = fopen(contents, "r");
            fscanf(fp, "%s", contents);
            sscanf(contents, "%d", &n);
            fscanf(fp, "%s", contents);
            sscanf(contents, "%d", &m);
            
	    count = (int *)malloc(m * sizeof(int));
            parts = (int *)malloc((m*n) * sizeof(int));
            memset(parts, 0, (m*n)*sizeof(parts[0]));
	    
            for(int i=0;i<m;i++){
                fscanf(fp, "%s", contents);
                sscanf(contents, "%d", &count[i]);
                fscanf(fp, "%s", contents);
                for(int j=0;j<count[i];j++){
                    fscanf(fp, "%s", contents);
                    sscanf(contents, "%d", &parts[i*n+j]);
                }
	    }
            fclose(fpn);
	    fclose(fp);  
	    start = 1;
	 
	}
	MPI_Bcast(&start, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
	if(start > 0){
		MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
        	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        	if(myid != 0){
            		count = (int *)malloc(m * sizeof(int));
            		parts = (int *)malloc((m*n) * sizeof(int));
            		memset(parts, 0, (m*n)*sizeof(parts[0]));
			memset(count, 0, m*sizeof(count[0]));
		}
		MPI_Bcast(count, m, MPI_INT, 0, MPI_COMM_WORLD);
		printf("%d %d %d\n",myid, m, n);
        	MPI_Bcast(parts, (m*n), MPI_INT, 0, MPI_COMM_WORLD);
		printf("%dstart",myid);
		finish = 1;
		myans = 0;
        	int group = 0;
        	int tgt[n];
        	while(1){
            	    if(group+myid < m){
                	memset(tgt, 0, n*sizeof(tgt[0]));
               		myans += deal(tgt,group+myid,count,parts,n, m, 1);
            	    }else{
                        break;
                    }
                    group += 2*numprocs;
                    if(group-myid-1 < m){
                        memset(tgt, 0, n*sizeof(tgt[0]));
                   	myans += deal(tgt,group-myid-1,count,parts,n, m, 1);
            	    }else{
                	break;
            	    }
        	}
        	MPI_Reduce(&myans, &ans, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        	if (myid == 0){
               	    printf("ans:%d\n", ans);          
        	} 
	}

    	       
    }
    MPI_Finalize();

    return 0;
}

int check(int *tgt, int n){
    for(int i=0;i<n;i++){
        if(tgt[i] == 0){
            return 0;
        }
    }
    return 1;
}
int mypow(int a, int x){
    if(x == 0){
        return 1;
    }
    int ans = 1;
    for(int i=0;i<x;i++){
        ans *= a;
    }
    return ans;
}
int deal(int *tgt, int p, int* count, int* parts, int n, int m, int get){
    if(p == m){
        return 0;
    }
    int ans = 0;
    if(get == 1){
        for(int i=0;i<count[p];i++){
            tgt[parts[p*n+i]-1]++;
        }
        if(check(tgt, n) == 1){
            for(int i=0;i<count[p];i++){
                tgt[parts[p*n+i]-1]--;
            }
            return mypow(2, (m-p-1));
        }
        ans += deal(tgt,p+1,count,parts,n,m,1);
        ans += deal(tgt,p+1,count,parts,n,m,0);
        for(int i=0;i<count[p];i++){
            tgt[parts[p*n+i]-1]--;
        }
    }else{
        ans += deal(tgt,p+1,count,parts,n,m,1);
        ans += deal(tgt,p+1,count,parts,n,m,0);
    }
    return ans;
}

