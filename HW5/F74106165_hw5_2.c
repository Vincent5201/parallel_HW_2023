#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

long thread_count;
int n;
struct Coord *coord;
int *up, *down;

double **adj;
double ans;

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

int partition(struct Coord arr[], int low, int high) {
    struct Coord pivot = arr[high]; 
    int i = (low - 1); 
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].x < pivot.x) {
            i++;
            swapc(&arr[i], &arr[j]);
        }else if(arr[j].x == pivot.x){
            if(arr[j].y < pivot.y){
                i++;
                swapc(&arr[i], &arr[j]);
            }
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

double dist(struct Coord a, struct Coord b){
    double tmp = sqrt(pow(a.x-b.x,2)+ pow(a.y-b.y,2));
    tmp = (int)(tmp * 10000) / 10000.0;
    return tmp;
}

double needed(int a, double *keys, int *from){
    double tmp = 0;
    for(int i=0;i<n;i++){
        if(from[i] == a){
            tmp += keys[i];
            tmp += needed(i, keys, from);
        }
    }
    return tmp;
}

void search(int *choose, int a){
    
    int *visited, *from;
    double *keys;
    visited = (int*)malloc(n * sizeof(int)); 
    from = (int*)malloc(n * sizeof(int));
    keys = (double*)malloc(n * sizeof(double));
    
    memset(visited, 0, n*sizeof(visited[0]));
    visited[0] = 1;
    keys[0] = 0;
    from[0] = -1;
    for(int i=1;i<n;i++){
        if(choose[i] > 0){
            from[i] = 0;
            keys[i] = adj[0][i];
        }else{
            from[i] = -1;
            keys[i] = 2000000000;
        }
    }
    for(int t=0;t<n-1;t++){
        int select = -1;
        for(int i=0;i<n;i++){
            if(visited[i] == 0 && choose[i] > 0){
                if(select == -1){
                    select = i;
                }else{
                    if(keys[i] < keys[select]){
                        select = i;
                    }
                }
            }
        }
        if(select == -1){
            break;
        }
        
        visited[select] = 1;
        for(int i=0;i<n;i++){
            if(visited[i] == 0  && choose[i] > 0){
                if(keys[i] > adj[select][i]){
                    keys[i] = adj[select][i];
                    from[i] = select;
                }
            }
        }
    }
    
    double tmp = needed(0, keys, from);
    #pragma omp critical
    if(tmp < ans){
        for(int i=0;i<n;i++){
            printf("%d ",choose[i]);
        }
        printf("\n");
        ans = tmp;
    }
    free(visited);
    free(from);
    free(keys);
    if(a == -1){
        return;
    }
    for(int i=a+1;i<n;i++){
        if(choose[i] == 0){
            choose[i] = 1;
            search(choose, i);
            choose[i] = 0;
        }
    }

    return;
}

int main( int argc, char *argv[]){
    thread_count = 4;
    FILE* fp;
    char buffer[256];
    scanf("%s",buffer);
    fp = fopen(buffer, "r");
    fscanf(fp,"%d",&n);
    coord = (struct Coord *)malloc(n * sizeof(struct Coord));
    for(int i=0;i<n;i++){
        fscanf(fp, "%d %d", &coord[i].x, &coord[i].y);
        coord[i].id = i;
    }
    fclose(fp);
    
    quickSort(coord,0,n-1);
    down = (int*)malloc(n * sizeof(int));
    memset(down, 0, n*sizeof(down[0]));
    int cont = 1;
    while (cont == 1){
        cont = 0;
        for(int i=1;i<n-1;i++){
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
                if(ang >= 0){
                    cont = 1;
                    down[i] = 1;
                }
            }
        }
    }
    up = (int*)malloc(n * sizeof(int));
    memset(up, 0, n*sizeof(up[0]));
    cont = 1;
    while (cont == 1){
        cont = 0;
        for(int i=n-2;i>0;i--){
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
                if(ang <= 0){
                    cont = 1;
                    up[i] = 1;
                }
            }
        }
    }
    up[0] = 1;
    down[0] = 1;
    ans = 2147364847;

    adj = (double**)malloc(n * sizeof(double*));
    for(int i=0;i<n;i++){
        adj[i] = (double*)malloc(n * sizeof(double));
    }
    for(int i=0;i<n;i++){
        adj[i][i] = 0;
        for(int j=i+1;j<n;j++){
            adj[i][j] = dist(coord[i], coord[j]);
            adj[j][i] = adj[i][j];
        }
    }
    int *choose;
    #pragma omp parallel for num_threads(thread_count) private(choose)
    for(int i=0;i<n;i++){
        choose = (int*)malloc(n * sizeof(int));
        memset(choose, 0, n*sizeof(choose[0]));
        choose[0] = 2;
        for(int i=0;i<n;i++){
            if(up[i] == 0 || down[i] == 0){
                choose[i] = 2;
            }
        }
        if(choose[i] == 0){
            choose[i] = 1;
            search(choose, i);
        }
        free(choose);
    }
    choose = (int*)malloc(n * sizeof(int));
    memset(choose, 0, n*sizeof(choose[0]));
    choose[0] = 2;
    for(int i=0;i<n;i++){
        if(up[i] == 0 || down[i] == 0){
            choose[i] = 2;
        }
    }
    search(choose, -1);
    printf("%.4lf",ans);
    free(choose);
    free(coord);
    free(up);
    free(down);
    free(adj);
    

    return 0;
}