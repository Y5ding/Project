#include <math.h>
#include <string.h>
#include <stdio.h>
#include "timer.h"

#define NN 2
#define NM 2

double A[NN][NM];
double B[NN][NM];
double C[NN][NM];
double D[NN][NM];
double *d_A;  //device copy
double *d_B; //device copy
double *d_C; //device copy

__global__ void matrixMul( double* A, double* B, double* C, int m, int n) {
  //for A[m][n] B[m][n] wa=n, wb=n
  // only conduct mutiplication on the GPU
  //here we consider only a square matrix
  int site,sitea,siteb;
  int rowstride = gridDim.x * blockDim.x; //intial stride Nx is a multiple of blockDim.x
  int colstride = gridDim.y * blockDim.y;
  int rowinit = threadIdx.x + blockDim.x * blockIdx.x;
  int colinit = threadIdx.y + blockDim.y * blockIdx.y; //intial index

  // required to compute the block sub-matrix for (int a = aBegin, b = bBegin;
  for(int s = rowinit; s < m; s+=rowstride)
    {
      for(int t = colinit; t < n; t+=colstride)
        {
          site = s*n + t; // 1 D memory location
          C[site]=0.0;
          for(int k = rowinit; k < m; k+=rowstride)
          {
            sitea=s*m+k;
            siteb=k*m+t;
            C[site]+= A[sitea]*B[siteb];
            printf("site: %d , %d, %d, %d \n", site, s, k, t);
          }
        printf("answer: %d , %f \n", site, C[site]);
        }
    }
}

int main(int argc, char** argv)
{
  const int n = NN;
  const int m = NM;
  //const int max = 1000;
  int size = n*m*sizeof(double);

  printf("Matrix Multiplication: %d x %d mesh\n", n, m);

  StartTimer();
  //Set up the initial values of A and B here.
    for(int i = 0; i < m; ++i){
      for(int j = 0; j < n; ++j){
         A[i][j] = 2;
      }      //matrix A
    }
    for(int p = 0; p < m; ++p){
      for(int q = 0; q < n; ++q){
         B[p][q] = 2;
      }      //matrix B
    }
    cudaMalloc((void **)&d_A, size);
    cudaMalloc((void **)&d_B, size);
    cudaMalloc((void **)&d_C, size);

    cudaMemcpy(d_A, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, B, size, cudaMemcpyHostToDevice);
//gpu CUDA
// setup execution parameters
    dim3 gridSize(16,16);
    dim3 blockSize(16,16);
    matrixMul<<<gridSize,blockSize>>>(d_A, d_B, d_C, m, n);

   cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);

   cudaFree(d_A);
   cudaFree(d_B);
   cudaFree(d_C);
    double runtime = GetTimer();
    printf(" total: %f s\n", runtime / 1000);
    //verify correctness
    for(int s = 0; s < m; ++s)
      {
        for(int t = 0; t < n; ++t)
          {
            D[s][t]=0.0;
            for(int k = 0; k < m; ++k)
            {
              D[s][t] += A[s][k] * B[k][t];
            }
            if(D[s][t] !=C[s][t])
            {
              printf("answer: %f , %f \n", C[s][t], D[s][t]);
            }
          }
      }
}
