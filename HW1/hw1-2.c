#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    struct Coord pivot = arr[high]; // 選擇最後一個元素作為基準
    int i = (low - 1); // 初始化小於基準的區域的索引

    for (int j = low; j <= high - 1; j++) {
        // 如果當前元素小於或等於基準
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
int main( int argc, char *argv[]){
    clock_t start_time = clock();
    clock_t end_time;
    double elapsed_time;
    int n;
    FILE* fp = fopen("hw1-2data\\student_input\\input\\c.txt", "r");
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
    struct Coord coord[n];
    for(int i=0;i<n;i++){
        sscanf(ptr, "%d %d%n", &coord[i].x, &coord[i].y, &count);
        coord[i].id = i+1;
        ptr += count;
    }
    free(ar);
    
    quickSort(coord,0,n-1);
    end_time = clock();
    elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("time: %f sec\n", elapsed_time);
    int down[n];
    memset(down, 0, n*sizeof(down[0]));
    int cont = 1;
    while (cont == 1)
    {
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
    int up[n];
    memset(up, 0, n*sizeof(up[0]));
    cont = 1;
    while (cont == 1)
    {
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
    
    
    end_time = clock();
    elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("time: %f sec\n", elapsed_time);
    return 0;
}