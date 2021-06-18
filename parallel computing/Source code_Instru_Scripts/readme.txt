#compiling OpenMp
module purge 
module load slurm 
module load cpu 
module load aocc 
module load openmpi

clang -fopenmp <filename.c> -o <filename_c>

#compiling MPI
module purge
module load slurm
module load cpu
module load gcc
module load openmpi
mpicc -g -Wall -o <filename> <filename.c>

#compiling CUDA
module purge
module load slurm
module load gpu
module load cuda
nvcc -o <filename.out> <filename_cu>

#compiling OpenACC
module purge 
module load slurm 
module load gpu 
module load pgi
pgcc -o <filename.exe> -fast -Minfo -acc -ta=tesla:cc70 filename.c>
