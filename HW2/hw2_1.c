#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int KA(int kx, int ky, int ax, int ay, const int *A, const int *K, int d, int n){
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

int calculate(int x, int y, const int *A, const int *K, int d, int m, int n){
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

int main(int argc, char *argv[]){
    int t, d, n, m;
    int *A, *K; 
    FILE* fp = fopen("2_1data\\b.txt", "r");
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
    
    while(t>0){
        int *Atmp = (int *)malloc((n*m) * sizeof(int));
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                Atmp[i*n+j] = calculate(i,j,A,K,d,m,n);
            }
        }
        //printf("%d\n",Atmp[0]);
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                A[i*n+j] = Atmp[i*n+j];
            }
        }
        free(Atmp);
        t--;
        
    }
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            printf("%d ",A[i*n+j]);
        }
    }
 
    


    return 0;
}