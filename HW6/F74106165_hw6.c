#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include <math.h>

double c0, decay;
int alpha, beta;
int n, m, t, answer;
int *best_tour;
int **adj, **path;
double **record_adj;
double **prob;
int **visited;

int main(int argc, char *argv[]){
    //set data
    time_t tr;
    srand((unsigned) time(&tr));
    FILE* fp;
    char buffer[256];
    scanf("%s",buffer);
    fp = fopen(buffer, "r");
    fscanf(fp,"%d",&n);
    fscanf(fp,"%d",&m);
    fscanf(fp,"%d",&t);
    answer = 2147483647;
    c0 = (double)1/n;
    decay = 0.5;
    alpha = 1;
    beta = 2;
    adj = (int **)malloc(n * sizeof(int*));                 //path length
    visited = (int **)malloc(m * sizeof(int*));     
    path = (int **)malloc(m * sizeof(int*));                //ants' path
    record_adj = (double **)malloc(n * sizeof(double*));    //Pheromone
    prob = (double **)malloc(m * sizeof(double*));    
    best_tour = (int *)malloc((n+1) * sizeof(int));         //answer
    for(int i=0;i<m;i++){
        path[i] = (int *)malloc(n * sizeof(int));
        prob[i] = (double *)malloc(n * sizeof(double));
        visited[i] = (int *)malloc(n * sizeof(int));
    }
    
    for(int i=0;i<n;i++){
        adj[i] = (int *)malloc(n * sizeof(int));
        record_adj[i] = (double *)malloc(n * sizeof(double));
        for(int j=0;j<n;j++){
            fscanf(fp, "%d", &adj[i][j]);
            if(i == j){
                record_adj[i][j] = 0;
            }else{
                record_adj[i][j] = c0;
            }
        }
    }

    // t iterations
    for(int tt=0;tt<t;tt++){
        //parallel m ants' computation
        //get each ant's path
        #pragma omp parallel for
        for(int i=0;i<m;i++){
            //printf("ants %d done by %d\n",i ,omp_get_thread_num()); //check parallel success
            for(int j=0;j<n;j++){
                visited[i][j] = 0;
                prob[i][j] = 0;
            }
            path[i][0] = rand() % n;
            visited[i][path[i][0]] = 1;
            int last = path[i][0];
            for(int j=1;j<n;j++){
                double probsum = 0;
                for(int k=0;k<n;k++){
                    if(visited[i][k] == 0){
                        prob[i][k] = pow(record_adj[last][k],alpha)*pow(((double)1/adj[last][k]),beta);
                        probsum += prob[i][k];
                    }
                }
                int choose = -1;
                double randprob = (double) rand() / (RAND_MAX + 1.0);
           
                for(int k=0;k<n;k++){
                    if(visited[i][k] == 0){
                        prob[i][k] /= probsum;
                        randprob -= prob[i][k];
                        if(choose == -1){
                            choose = k;
                        }
                        if(randprob <= 0){
                            choose = k;
                            break;
                        }
                    }
                }
                path[i][j] = choose;
                visited[i][choose] = 1;
                last = choose;
            }
         
        }
        //decay Pheromone
        #pragma omp parallel for
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                record_adj[i][j] *= decay;
            }
        }
        //update new Pheromone information and choose shortest path
        #pragma omp parallel for
        for(int i=0;i<m;i++){
            int pathlen = 0;
            for(int j=1;j<n;j++){
                pathlen += adj[path[i][j-1]][path[i][j]];
            }
            pathlen += adj[path[i][n-1]][path[i][0]];
            for(int j=1;j<n;j++){
                record_adj[path[i][j-1]][path[i][j]] += (double)1/pathlen;
            }
            record_adj[path[i][n-1]][path[i][0]] += (double)1/pathlen;
            #pragma omp critical
            if(pathlen < answer){
                for(int j=0;j<n;j++){
                    best_tour[j] = path[i][j];
                }
                answer = pathlen;
            }
        }
    }
    
    best_tour[n] = best_tour[0];
    printf("best_tour : ");
    for(int i=0;i<=n;i++){
        printf("%d ",best_tour[i]);
    }
    printf("\n");
    printf("length : %d",answer);
    return 0;
}