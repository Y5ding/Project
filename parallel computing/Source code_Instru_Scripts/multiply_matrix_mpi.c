#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <malloc.h>
#include "timer.h"

#define NN 1024
#define NM 1024

double *A, *B, *C, *buff, *sub;
int rank, numprocs, numline;

int main(int argc, char** argv)
{

  const int n = NN;
  const int m = NM;
  //const int nummax = 1000;

    MPI_Init(NULL,NULL);//MPI Initialize
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);//obtain number of processor

    numline = n/numprocs;
    A = (double*)malloc(sizeof(double)*n*m);
    B = (double*)malloc(sizeof(double)*n*m);
    C = (double*)malloc(sizeof(double)*n*m);
    
    buff = (double*)malloc(sizeof(double)*n*numline);//size of subsets
    sub = (double*)malloc(sizeof(double)*n*numline);// result of subsets

if (rank==0)
    {
        printf("Matrix Multiplication: %d x %d mesh\n", n, m);
        StartTimer();
      for(int i = 0; i < m; ++i){
        for(int j = 0; j < n; ++j){
            A[i*m+j] = (rand() % 9) + 1;
        }      //matrix A
      }
      for(int p = 0; p < m; ++p){
        for(int q = 0; q < n; ++q){
            B[p*m+q] = (rand() % 9) + 1;
        }      //matrix B
      }

        for (int e=1;e<numprocs;e++){//send A
            MPI_Send(A,n*m,MPI_DOUBLE,e,0,MPI_COMM_WORLD);
        }
        for (int f=1; f<numprocs; f++){//send B
            MPI_Send(B+(f-1)*numline*m,m*numline,MPI_DOUBLE,f,1,MPI_COMM_WORLD);
        }
        //recv subset
        for (int k=1;k<numprocs;k++)
        {
            MPI_Recv(sub,n*numline,MPI_DOUBLE,k,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);//from any source without order
            for (int i=0;i<numline;i++)
            {
                for (int j=0;j<m;j++)
                {
                    C[((k-1)*numline+i)*m+j] = sub[i*m+j];// block: k-1 th numprocs
                                                    //location: i*m+j
                }
            }
        }
        for (int i=(numprocs-1)*numline;i<m;i++){
            for (int j = 0; j < n; j++) {
             int temp = 0;
             for (int k = 0; k<m; k++) {
                 temp +=A[i*m+k]*B[k*m+j];
              }
            C[i*m+j] = temp;
            }
        }

    double runtime = GetTimer();

    printf(" runtime: %f s\n", runtime / 1000);
    printf(" ans: %f \n",C[0]);
    //printf(" ans: %f \n",&C[1]);
        free(A);
        free(B);
        free(C);
        free(buff);
        free(sub);
    }
else
   {
       MPI_Recv(A,n*m,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

       MPI_Recv(buff,m*numline,MPI_DOUBLE,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

       for (int i=0;i<numline;i++)
       {
           for (int j=0;j<n;j++)
           {
               int temp=0;
               for(int k=0;k<m;k++){
                   temp += buff[i*m+k]*A[k*m+j];}
               sub[i*m+j]=temp;
           }
       }
       MPI_Send(sub,n*numline,MPI_DOUBLE,0,3,MPI_COMM_WORLD);
   }

   MPI_Finalize();
}
