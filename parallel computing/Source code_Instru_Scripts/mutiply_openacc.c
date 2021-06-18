#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openacc.h>
#include "timer.h"

#define NN 1024
#define NM 1024

double A[NN][NM];
double B[NN][NM];
double C[NN][NM];

int main(int argc, char** argv)
{
  const int n = NN;
  const int m = NM;
  const int max = 1000;

  printf("Matrix Multiplication: %d x %d mesh\n", n, m);

StartTimer();
int iter=0;
#pragma acc data copyin (A,B), copy(C)
    for(int i = 0; i < m; ++i){
      for(int j = 0; j < n; ++j){
         A[i][j] = rand() % max;
      }      //matrix A
    }
    for(int p = 0; p < m; ++p){
      for(int q = 0; q < n; ++q){
         B[p][q] = rand() % max; // run on the cpu
      }      //matrix B
    }
       
   while (iter<500) {
   #pragma acc kernels
    for(int s = 0; s < m; ++s)
      {
        for(int t = 0; t < n; ++t)
          {
            C[s][t]=0;
            for(int k = 0; k < m; ++k)
            {
              C[s][t] += A[s][k] * B[k][t];
            }
          }
      }
       if(iter%50==0){printf("%5d, %0.6f\n", iter, C[0][0]);}
       iter++;
   }
    double runtime = GetTimer();
    //printf(" %0.6f\n", C[0][0]);
    printf(" total: %f s\n", runtime / 1000);
}
