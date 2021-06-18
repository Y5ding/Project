#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
  const int iter_max = 1000;

    printf("Matrix Multiplication: %d x %d mesh\n", n, m);

    StartTimer();
  for(i = 0; i < m; ++i){
    for(j = 0; j < n; ++j){
       A[i][j] = rand() % iter_max;
    }      //matrix A
  }
  for(p = 0; p < m; ++p){
    for(q = 0; q < n; ++q){
       B[p][q] = rand() % iter_max;
    }      //matrix B
  }

  for(s = 0; s < m; ++s)
	{
		for(t = 0; t < n; ++t)
		{
			C[s][t]=0;
			for(k = 0; k < m; ++k)
			C[s][t] += A[s][k] * B[k][t];
		}
	}

    double runtime = GetTimer();

    printf(" total: %f s\n", runtime / 1000);
}
