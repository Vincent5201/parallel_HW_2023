#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>
struct Coord
{
    int x;
    int y;
    int id;
};

int angle(int x1, int y1, int x2, int y2, int x3, int y3){
    int vx1 = x2-x1;
    int vy1 = y2-y1;
    int vx2 = x2-x3;
    int vy2 = y2-y3;
    return vx1*vy2 - vx2*vy1;
}

void swapc(struct Coord* a, struct Coord* b) {
    struct Coord t = *a;
    *a = *b;
    *b = t;
}

int comp(struct Coord a, struct Coord b){
    if (a.x < b.x) {
        return 1;
    }else if(a.x == b.x){
        if(a.y < b.y){
            return 1;
        }
    }
    return 0;
}

int partition(struct Coord arr[], int low, int high) {
    struct Coord pivot = arr[high]; 
    int i = (low - 1); 

    for (int j = low; j <= high - 1; j++) {
        if(comp(arr[j],pivot) == 1){
            i++;
            swapc(&arr[i], &arr[j]);
        }
    }
    swapc(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(struct Coord arr[],  int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main( int argc, char *argv[]){
    int finish=1, start, myid, numprocs, turn = 2;
    int n;
    struct Coord *coord;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    start = 0; 
    int part_num = (n-2+numprocs-2)/(numprocs-1);
    while(finish > 0){
        if(myid == 0 && start == 0){   
            char contents[255];
            scanf("%s",contents);
            FILE* fp = fopen(contents, "r");
            fseek(fp,0,SEEK_END);
            int size = ftell(fp);
            rewind(fp);
            char *ar = (char *)malloc(sizeof(char)*size);
            fread(ar,1,size,fp);
            fclose(fp);
            char* ptr = ar;
            int count;
            sscanf(ptr, "%d%n", &n, &count);
            ptr += count;
            coord = (struct Coord *)malloc(n * sizeof(struct Coord));
            for(int i=0;i<n;i++){
                sscanf(ptr, "%d %d%n", &coord[i].x, &coord[i].y, &count);
                coord[i].id = i+1;
                ptr += count;
            }
            free(ar);
	        start = 1;
            //quickSort(coord,0,n-1);
	    }
        MPI_Bcast(&start, 1, MPI_INT, 0, MPI_COMM_WORLD);
        

    if(start > 0){
	    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	    if(myid != 0){
   	        coord = (struct Coord *)malloc(n * sizeof(struct Coord));
	    }
	    MPI_Bcast(coord, n*sizeof(struct Coord), MPI_BYTE, 0, MPI_COMM_WORLD);
        int num_sort = (n+numprocs-1)/(numprocs);
	if(myid < numprocs-1){
            quickSort(coord,myid*num_sort,(myid+1)*num_sort-1);
	}else{
            quickSort(coord,myid*num_sort,n-1);
	}
        struct Coord *tmpcoord;
        tmpcoord = (struct Coord *)malloc(n * sizeof(struct Coord));
        while(turn <= numprocs){
            if(myid%turn == 0){
                MPI_Recv(tmpcoord,n*sizeof(struct Coord),MPI_BYTE,myid+(turn/2),0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);                int a1 = myid*num_sort;
                int a2 = (myid+(turn/2))*num_sort;
                int p =  myid*num_sort;
                while(a1 < (myid+(turn/2))*num_sort && a2 < (myid+turn)*num_sort && a2 < n){
                    if(comp(coord[a1],tmpcoord[a2])==1){
                        tmpcoord[p] = coord[a1];
                        a1++;
                    }else{
                        tmpcoord[p] = tmpcoord[a2];
                        a2++;
                    }
                    p++;
                }
                if(a2 == (myid+turn)*num_sort || a2 == n){
                    while(a1 < (myid+(turn/2))*num_sort){
                        tmpcoord[p] = coord[a1];
                        a1++;
                        p++;
                    }
		}
		for(int i=0;i<n;i++){
	            coord[i] = tmpcoord[i];
           	}	
            }else if(myid%(turn/2) == 0){
                MPI_Send(coord,n*sizeof(struct Coord),MPI_BYTE,myid-(turn/2),0,MPI_COMM_WORLD);
            }
            turn *= 2;
        }

        MPI_Bcast(coord, n*sizeof(struct Coord), MPI_BYTE, 0, MPI_COMM_WORLD);
	
            int down[n];
            memset(down, 0, n*sizeof(down[0]));
            int cont = 1;
            if(myid == 0){
                while (cont == 1){
                    MPI_Bcast(down, n, MPI_INT, 0, MPI_COMM_WORLD);
                    cont = 0;
                    for(int i=1;i<n-1;i++){
                        int rcv;
			        if(down[i] == 0){
                            MPI_Recv(&rcv, 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                            down[i] = rcv;
                            if(rcv == 1){
                                cont = 1;
                            }
			        }
                    }
                    MPI_Bcast(&cont, 1, MPI_INT, 0, MPI_COMM_WORLD);
                }
            }else{
                while(cont == 1){
                    MPI_Bcast(down, n, MPI_INT, 0, MPI_COMM_WORLD);
                    for(int i=myid;i<n-1;i+=(numprocs-1)){
                        if(down[i] == 0){
                            int l = i-1;
                            int r = i+1;
                            while(down[l] == 1){
                                l--;
                            }
                            while(down[r] == 1){
                                r++;
                            }
                            int ang = angle(coord[l].x, coord[l].y, coord[i].x, coord[i].y, coord[r].x, coord[r].y);
                            int send = 0;
                            if(ang > 0){
                                send = 1;
                            }
                            MPI_Send(&send, 1, MPI_INT, 0, i, MPI_COMM_WORLD);
                        }
                    }
                    MPI_Bcast(&cont, 1, MPI_INT, 0, MPI_COMM_WORLD);
                }
            }

            int up[n];
            memset(up, 0, n*sizeof(up[0]));
            cont = 1;
            if(myid == 0){
                while (cont == 1){
                    MPI_Bcast(up, n, MPI_INT, 0, MPI_COMM_WORLD);
                    cont = 0;
                    for(int i=n-2;i>0;i--){
                        int rcv;
			if(up[i]==0){
                            MPI_Recv(&rcv, 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                            up[i] = rcv;
                            if(rcv == 1){
                                cont = 1;
                            }
			}
                    }
                    MPI_Bcast(&cont, 1, MPI_INT, 0, MPI_COMM_WORLD);
                }
                finish = 0;
                MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
            }else{
                while(cont == 1){
                    MPI_Bcast(up, n, MPI_INT, 0, MPI_COMM_WORLD);
                    for(int i=n-1-myid;i>0;i-=(numprocs-1)){
                        if(up[i] == 0){
                            int l = i-1;
                            int r = i+1;
                            while(up[l] == 1){
                                l--;
                            }
                            while(up[r] == 1){
                                r++;
                            }
                            int ang = angle(coord[l].x, coord[l].y, coord[i].x, coord[i].y, coord[r].x, coord[r].y);
                            int send = 0;
                            if(ang <= 0){
                                send = 1;
                            }
                            MPI_Send(&send, 1, MPI_INT, 0, i, MPI_COMM_WORLD);
                        }
                    }
                    MPI_Bcast(&cont, 1, MPI_INT, 0, MPI_COMM_WORLD);
                }
                MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);
            }
            if(myid == 0){
                for(int i=0;i<n;i++){
                    if(up[i] == 0){
                        printf("%d ", coord[i].id);
                    }
                }
                for(int i=n-2;i>0;i--){
                    if(down[i] == 0){
                        printf("%d ", coord[i].id);
                    }
                }
            }
        }
	}
	MPI_Finalize();
    return 0;
	       
}
    
