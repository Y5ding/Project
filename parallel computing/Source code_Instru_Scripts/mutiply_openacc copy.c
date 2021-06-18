if (rank==0){ // main processor-0
        fill out matrices A and B
        for (int e=1;e<numprocs;e++)//send A to other processors
            MPI_Send(A,n*m,MPI_DOUBLE,e,0,MPI_COMM_WORLD);
        for (int f=1; f<numprocs; f++)//send B other processors
            MPI_Send(B+(f-1)*numline*m,m*numline,MPI_DOUBLE,f,1,MPI_COMM_WORLD);
        for (int k=1;k<numprocs;k++){
            //calculating sub blocks in other processors and receive the subset
        MPI_Recv(sub,n*numline,MPI_DOUBLE,k,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);}
            for (int i=0;i<numline;i++){
                for (int j=0;j<m;j++){
                    C[((k-1)*numline+i)*m+j] = sub[i*m+j];}}
        }//calculating render parts of B
        for (int i=(numprocs-1)*numline;i<m;i++){
            for (int j = 0; j < n; j++) {
             int temp = 0;
             for (int k = 0; k<m; k++) {
                 temp +=A[i*m+k]*B[k*m+j];}
            C[i*m+j] = temp;}
        }
else{ // other processors calculate the sub blocks
       MPI_Recv(A,n*m,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
       MPI_Recv(buff,m*numline,MPI_DOUBLE,0,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
       for (int i=0;i<numline;i++){
           for (int j=0;j<n;j++){
               int temp=0;
               for(int k=0;k<m;k++){
                   temp += buff[i*m+k]*A[k*m+j];}
               sub[i*m+j]=temp;}
       }
       MPI_Send(sub,n*numline,MPI_DOUBLE,0,3,MPI_COMM_WORLD);
   }
   MPI_Finalize();
}

__global__ void matrixMul( double* A, double* B, double* C, int nn)
{
    __shared__ double AS[BLOCK_SIZE][BLOCK_SIZE]; //subset
    __shared__ double BS[BLOCK_SIZE][BLOCK_SIZE];
    
    int bx = blockIdx.x; // Block index
    int by = blockIdx.y;
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int Col = bx * BLOCK_SIZE + tx;
    int Row = by * BLOCK_SIZE + ty;
   double sub = 0;
  for(int a = 0;a < nn / BLOCK_SIZE;a++){ //for each bloack
            AS[ty][tx] = A[Row * nn + (a * BLOCK_SIZE + tx)]; // WA*ty
            BS[ty][tx] = B[Col + (a * BLOCK_SIZE + ty) * nn];
            __syncthreads();
            for(int k = 0;k < BLOCK_SIZE;k++){
               sub += AS[ty][k] * BS[k][tx];}
            __syncthreads();
        }
        C[Row * nn + Col] = sub; //by * BLOCK_SIZE * wB + ty * wB +bx * BLOCK_SIZE
}
int main(){
//fill out matrices A and B
//allocate GPU memory
cudaMemcpy(d_A, &A, size, cudaMemcpyHostToDevice);
cudaMemcpy(d_B, &B, size, cudaMemcpyHostToDevice);
dim3 gridSize(m/width,n/width);
dim3 blockSize(width,width);
matrixMul<<<gridSize,blockSize>>>(d_A, d_B, d_C, m);
cudaDeviceSynchronize();
    cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);}
//free GPU memory


#pragma omp parallel for shared(m, n, A, B)
{
    //fill out matrices A and B
}
#pragma omp parallel for
shared(m, n, A, B)private(nthreads)
{
  for(s = 0; s < m; ++s){
    for(t = 0; t < n; ++t){
      C[s][t]=0;
      for(k = 0; k < m; ++k)
      C[s][t] += A[s][k] * B[k][t];
    }
  }
}

    double runtime = GetTimer();

    printf(" total: %f s\n", runtime / 1000);
}
