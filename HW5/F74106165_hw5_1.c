#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

int find1(int my_rank, int **tasks, int time, long thread_count, int n);
int find2(int my_rank, int **tasks, int time, long thread_count, int n);

int main(int argc, char *argv[]){
    int thread_count = 4;
    int n, time;
    int **tasks;
    int *selects;
    FILE* fp;

    int ans = 0;
    char buffer[256];
    scanf("%s",buffer);
    fp = fopen(buffer, "r");
    fscanf(fp,"%d",&n);
    tasks = (int **)malloc(n * sizeof(int*));
    for(int i=0;i<n;i++){
        tasks[i] = (int *)malloc(3 * sizeof(int));
        fscanf(fp, "%d %d %d", &tasks[i][0], &tasks[i][1], &tasks[i][2]);
    }
    fclose(fp);
    selects = (int *)malloc(thread_count * sizeof(int));
    int select = -1;
    int done = 0;
    time = 0;
    
    while (done < n){
        #pragma omp parallel for num_threads(thread_count)
        for(int i=0;i<thread_count;i++){
            selects[i] = find1(i, tasks, time, thread_count, n);
        }   
        for(int i=0;i<thread_count;i++){
            if(selects[i] == -1){
                continue;
            }
            if(select == -1){
                select = selects[i];
            }else{
                if(tasks[selects[i]][2] * tasks[select][0] > tasks[select][2] * tasks[selects[i]][0]){
                    select = selects[i];
                }  
            }
        }
        
        
        tasks[select][0]--;
        if(tasks[select][0] == 0){
            ans += (time+1)*tasks[select][2];
            select = -1;
            done += 1;
            #pragma omp parallel for num_threads(thread_count)
            for(int i=0;i<thread_count;i++){
                selects[i] = find2(i, tasks, time, thread_count, n);
            }   
            
            for(int i=0;i<thread_count;i++){
                if(selects[i] == -1){
                    continue;
                }
                if(select == -1){
                    select = selects[i];
                }else{
                    if(tasks[selects[i]][2] * tasks[select][0] > tasks[select][2] * tasks[selects[i]][0]){
                        select = selects[i];
                    }  
                }
            }
        }
        time += 1;
    }
    printf("%d",ans);
    return 0;

}

int find2(int my_rank, int **tasks, int time, long thread_count, int n){
    int my_select = -1;
    for(int i=my_rank;i<n;i+=thread_count){
        if(time >= tasks[i][1] && tasks[i][0] > 0){
            if(my_select != -1){
                if(tasks[i][2] * tasks[my_select][0] > tasks[my_select][2] * tasks[i][0]){
                    my_select = i;
                }    
            }else{
                my_select = i;
            }
        }
    }
    return my_select;
}
int find1(int my_rank, int **tasks, int time, long thread_count, int n){
    int my_select = -1;
    for(int i=my_rank;i<n;i+=thread_count){
        if(time == tasks[i][1]){
            if(my_select != -1){
                if(tasks[i][2] * tasks[my_select][0] > tasks[my_select][2] * tasks[i][0]){
                    my_select = i;
                }    
            }else{
                my_select = i;
            }
        }
    }
    return my_select;
}