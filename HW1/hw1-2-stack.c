#include <stdio.h>
#include <stdlib.h>
#define MAXSTACK 10000 /*定義最大堆疊容量*/
int pos;
struct Coord
{
    int x;
    int y;
    int id;
};
struct Coord stack[MAXSTACK];
int isEmpty(){
	if(pos==-1){
		return 1; 
	}else{
		return 0;
	}
} 
void push(struct Coord data){
	if(pos>=MAXSTACK){
		printf("full\n");	
	}else{
		pos++;
		stack[pos]=data;
	}
} 
struct Coord pop(){
	struct Coord data;
	if(isEmpty()){
		printf("empty\n");
	}else{
		data=stack[pos];
		pos--;
		return data; 
	}
}
struct Coord top(){
	struct Coord data;
	if(isEmpty()){
		printf("empty\n");
	}else{
		data=stack[pos];
		return data; 
	}
}
int stsize(){
	return pos+1;
}

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

// 快速排序函數
void quickSort(struct Coord arr[],  int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
int main( int argc, char *argv[]){
    pos = -1;
    FILE* fp = fopen("hw1-2data\\student_input\\input\\a.txt", "r");
    char contents[255];
    int n;
    fscanf(fp, "%s", contents);
    sscanf(contents, "%d", &n);
    printf("%d ",n);
    struct Coord coord[n];
    int idx[n];
    for(int i=0;i<n;i++){
        fscanf(fp, "%s", contents);
        sscanf(contents, "%d", &coord[i].x);
        fscanf(fp, "%s", contents);
        sscanf(contents, "%d", &coord[i].y);
        coord[i].id = i+1;
    }

    fclose(fp);
    quickSort(coord,0,n-1);
    
    int count = 0;
    push(coord[0]);
    push(coord[1]);
    for(int i=2;i<n;i++){
        struct Coord c3 = coord[i];
        struct Coord c2 = pop();
        struct Coord c1 = top();
        int ck = 0;
        while(angle(c1.x,c1.y,c2.x,c2.y,c3.x,c3.y) >= 0){
            if(stsize() == 1){
                ck = 1;
                break;
            }else{
                c2 = pop();
                c1 = top();
            }
        }
        if(ck == 0){
            push(c2);
        }
        push(c3);
    }
    count = stsize();
    struct Coord down[count];
    for(int i=0;i<count;i++){
        down[i] = pop();
    }
    push(coord[n-1]);
    push(coord[n-2]);
    for(int i=n-3;i>=0;i--){
        int ck = 0;
        struct Coord c3 = coord[i];
        struct Coord c2 = pop();
        struct Coord c1 = top();
        while(angle(c1.x,c1.y,c2.x,c2.y,c3.x,c3.y) >= 0){
            if(stsize() == 1){
                ck = 1;
                break;
            }else{
                c2 = pop();
                c1 = top();
            }
        }
        if(ck == 0){
            push(c2);
        }
        push(c3);
    }

    int tmpsz = stsize();
    count += tmpsz;
    struct Coord up[tmpsz];

    for(int i=0;i<tmpsz;i++){
        up[i] = pop();
    }
    for(int i=0;i<sizeof(up)/sizeof(up[0]);i++){
        printf("%d ",up[i].id);
    }
    printf("\n");
    for(int i=1;i<sizeof(down)/sizeof(down[0])-1;i++){
        printf("%d ",down[i].id);
    }
    return 0;
}