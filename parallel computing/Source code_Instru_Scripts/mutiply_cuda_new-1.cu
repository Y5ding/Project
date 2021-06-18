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
//__managed__ double  checkiter=0.0;
double *d_A;  //device copy
double *d_B; //device copy
double *d_C; //device copy

#define BLOCK_SIZE 16 //2,4,8

__global__ void matrixMul( double* A, double* B, double* C, int nn) 
{
  //for A[m][n] B[m][n] wa=n, wb=n
  // only conduct mutiplication on the GPU
    __shared__ double AS[BLOCK_SIZE][BLOCK_SIZE]; //subset
    __shared__ double BS[BLOCK_SIZE][BLOCK_SIZE];
        
        int bx = blockIdx.x; // Block index
        int by = blockIdx.y;
        int tx = threadIdx.x;
        int ty = threadIdx.y;
        
        int Col = bx * BLOCK_SIZE + tx; // 1-D array m*m matrix  m*n
        int Row = by * BLOCK_SIZE + ty;

       double sub = 0;
       //printf("answer: %f \n", sub);

       for(int a = 0;a < nn / BLOCK_SIZE;a++)  //for each bloack
        {
            AS[ty][tx] = A[Row * nn + (a * BLOCK_SIZE + tx)]; // WA*ty
            BS[ty][tx] = B[Col + (a * BLOCK_SIZE + ty) * nn];
            __syncthreads();
            
            
            for(int k = 0;k < BLOCK_SIZE;k++){
               sub += AS[ty][k] * BS[k][tx];}
            __syncthreads();
        }
        C[Row * nn + Col] = sub; //by * BLOCK_SIZE * wB + ty * wB +bx * BLOCK_SIZE + tx
                                       //by * BLOCK_SIZE * wB + ty * wB +bx * BLOCK_SIZE + tx
        //checkiter=C[0];

}

int main(void)
{
  const int n = NN;
  const int m = NM;
  const int max = 1000;
    int width=m/64;  //16

  int size = n*m*sizeof(double);

  printf("Matrix Multiplication: %d x %d mesh\n", n, m);

  StartTimer();
  //Set up the initial values of A and B here.
        for(int i = 0; i < m; ++i){
      for(int j = 0; j < n; ++j){
         A[i][j] = rand() % max;
      }      //matrix A
    }
    for(int p = 0; p < m; ++p){
      for(int q = 0; q < n; ++q){
         B[p][q] = rand() % max;
      }      //matrix B
    }
    double sque = GetTimer();
    printf(" sque: %f s\n", sque / 1000);
    cudaMalloc((void **)&d_A, size);
    cudaMalloc((void **)&d_B, size);
    cudaMalloc((void **)&d_C, size);

    cudaMemcpy(d_A, &A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, &B, size, cudaMemcpyHostToDevice);
    //cudaError_t  error_check ;
//gpu CUDA
// setup execution parameters
    dim3 gridSize(m/width,n/width);
    dim3 blockSize(width,width); 
    int iter=0;
    /*while (iter<500) { 
    checkiter=0.0;*/
    matrixMul<<<gridSize,blockSize>>>(d_A, d_B, d_C, m);
    cudaDeviceSynchronize();
    /*if(iter%50==0){printf("%5d, %0.6f\n", iter, checkiter);}
   iter++;
}*/
    /*error_check = cudaGetLastError();
   if( error_check != cudaSuccess ){
    printf("%s\n" , cudaGetErrorString( error_check ) );
    system("pause") ;
    return 0 ;
    }*/
   cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);

   cudaFree(d_A);
   cudaFree(d_B);
   cudaFree(d_C);
    double runtime = GetTimer();
    printf(" total: %f s\n", runtime / 1000);
    //verify correctness
    double ans=0.0;
    double ansl=0.0;
            for(int k = 0; k < m; ++k)
            {
              ans += A[0][k] * B[k][0];
              ansl += A[2][k] * B[k][2];
            }
            if(ans !=C[0][0] || ansl !=C[2][2])
            {
              printf("Wrong: %f , %f \n", C[0][0], ans);
              printf("Wrong: %f , %f \n", C[2][2], ansl);
            }
     else{
     printf("Correct: %f , %f \n", C[0][0], ans);
     printf("Correct: %f , %f \n", C[2][2], ansl);}
}
