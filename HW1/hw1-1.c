#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int deal(int *tgt, int p, int* count, int* parts, int n, int m, int get);

int main( int argc, char *argv[]){
    int tmp [10];

    FILE* fp = fopen("hw1-1data\\student_input\\input\\b.txt", "r");
    char contents[255];
    int n, m;
    fscanf(fp, "%s", contents);
    sscanf(contents, "%d", &n);
    fscanf(fp, "%s", contents);
    sscanf(contents, "%d", &m);
    int count[m];
    int *parts = (int *)malloc((m*n) * sizeof(int));
    memset(parts, 0, (n*m)*sizeof(parts[0]));
    for(int i=0;i<m;i++){
        fscanf(fp, "%s", contents);
        sscanf(contents, "%d", &count[i]);
        fscanf(fp, "%s", contents);
        for(int j=0;j<count[i];j++){
            fscanf(fp, "%s", contents);
            sscanf(contents, "%d", &parts[i*n+j]);
            
        }
    }
    
    fclose(fp);
    
    int ans = 0;
    
    for(int i=0;i<m;i++){
        int tgt[n];
        memset(tgt, 0, n*sizeof(tgt[0]));
        int tmp = deal(tgt,i,count,parts,n, m, 1);
      
        ans += tmp;
    }
    printf("%d", ans);
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
        int i = 0;
        while(i<n && parts[p*n+i] != 0){
            tgt[parts[p*n+i]-1]++;
            i++;
        }
        /*for(int i=0;i<count[p];i++){
            tgt[parts[p*m+i]-1]++;
        }*/
        if(check(tgt, n) == 1){
            i = 0;
            while(i<n && parts[p*n+i] != 0){
                tgt[parts[p*n+i]-1]--;
                i++;
            }
            /*for(int i=0;i<count[p];i++){
                tgt[parts[p*m+i]-1]--;
            }*/
            return mypow(2, (m-p-1));
        }
        ans += deal(tgt,p+1,count,parts,n,m,1);
        ans += deal(tgt,p+1,count,parts,n,m,0);
        i = 0;
        while(i<n && parts[p*n+i] != 0){
            tgt[parts[p*n+i]-1]--;
            i++;
        }
    }else{
        ans += deal(tgt,p+1,count,parts,n,m,1);
        ans += deal(tgt,p+1,count,parts,n,m,0);
    }
    return ans;
}
