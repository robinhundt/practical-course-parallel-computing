#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>


#define DEFAULT_GRIDSIZE 1024

// Save/ Print only every nth step:
#define PRINTSTEP 10


void init_cells(double* grid, int gridsize);
void print(double* grid, int padded_grid_size, int time);
void save(FILE *f, double* grid, int padded_grid_size, int time);
int find_option( int argc, char **argv, const char *option );
int read_int( int argc, char **argv, const char *option, int default_value );
char *read_string( int argc, char **argv, const char *option, char *default_value );

__global__ void init_matrix(float *A, int rank, int n = 1024) {
  int i = threadIdx.x;

  int i_global = i + rank * 256;

  for(int j=1; j<1024; j++) {
    if(i_global < n/2 && j < n/2 || n/2 >= i_global && n/2 >= j)
      A[i*n+j] = 1;
    else  
      A[i*n+j] = 0;
  }
}

__global__ void jacobi_iteration(float *A_in, float *A_out,
                                int *gt_eps, int rank, float eps = 0.01,
                                int n = 1024) {
  __shared__ int gt_eps_block;
  int i = threadIdx.x;
  if (i == 0) {
    gt_eps_block = 0
  }

  for(int j=1; j<1023; j++) {
    A_out[i*n+j] = (A_in[(i-1)*n+j] + A_in[(i+1)*n+j] \ 
                    + A_in[i*n+j-1] + A_in[i*n+j+1]) / 4.0;
    if(fdimf(A_out[i*n+j], A_in[i*n+j]) > eps)
      gt_eps_block = 1;
  }

  *gt_eps = gt_eps_block;
}

MPI_Status exchange_borders(float *A, int rank, int n = 1024, int processes = 4) {

  MPI_Status status;

  if(rank > 0 && rank < processes-1) {
    MPI_Sendrecv(&A[n], n, MPI_FLOAT, rank-1, 42, 
                  A, n, MPI_FLOAT, rank-1, 42, MPI_COMM_WORLD, &status);
    MPI_Sendrecv(&A[256*n], n, MPI_FLOAT, rank+1, 42, 
                  &A[257*n], n, MPI_FLOAT, rank+1, 42, MPI_COMM_WORLD, &status);
  } else if(rank == 0) {
    MPI_Sendrecv(&A[256*n], n, MPI_FLOAT, 1, 42, &A[257*n], n,
                MPI_FLOAT, 1, 42, MPI_COMM_WORLD, &status);
  } else if(rank == processes-1) {
    MPI_Sendrecv(&A[n], n, MPI_FLOAT, rank-1, 42, A, n,
                MPI_FLOAT, rank-1, 42, MPI_COMM_WORLD, &status);
  }

  return status;
}


int main(int argc, char** argv) {

  int numtasks, rank;
    
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Status status;
  cudaError_t err;
  
  if( find_option( argc, argv, "-h" ) >= 0 )
  {
      printf( "Options:\n" );
      printf( "-h to see this help\n" );
      printf( "-n <int> to set the grid size\n" );
      printf( "-o <filename> to specify the output file name\n" );
      return 1;
  }
  
  int GRIDSIZE = read_int( argc, argv, "-n", DEFAULT_GRIDSIZE );
  // Check gridsize for some basic assumptions
  if(GRIDSIZE != DEFAULT_GRIDSIZE) {
    printf("Only Gridsize of 1024 is allowed!\n");
    return 1;
  }


  char *savename = read_string( argc, argv, "-o", "sample_conduct.txt" );
  FILE *f = savename ? fopen( savename, "w" ) : NULL;
  if( f == NULL )
  {
      printf( "failed to open %s\n", savename );
      return 1;
  }
  

  float *A_block;
  float *A_block_tmp;

  cudaMallocManaged(&A_block, sizeof *A_block * 258 * 1024);
  cudaMallocManaged(&A_block_tmp, sizeof *A_block_tmp * 258 * 1024);
  init_matrix<<<1, 256>>>(&A_block[1024], rank);

  err = cudaGetLastError();    
  if (err != cudaSuccess) 
    fprintf(stderr, "Error: %s\n", cudaGetErrorString(err));
  cudaDeviceSynchronize();

  int *gt_all_eps;
  cudaMallocManaged(&gt_all_eps, sizeof *gt_all_eps * 4);
  bool gt_eps_global = true;


  while(gt_eps_global) {
    exchange_borders(A_block, rank);
    if(rank == 0)
      jacobi_iteration<<<1, 255>>>(&A_block[2*1024], &A_block_tmp[2*1024],
                                   &gt_all_eps[rank], rank);
    else if(rank == 3)
      jacobi_iteration<<<1, 255>>>(&A_block[1*1024], &A_block_tmp[1*1024],
        &gt_all_eps[rank], rank);
    else 
      jacobi_iteration<<<1, 256>>>(&A_block[1*1024], &A_block_tmp[1*1024],
        &gt_all_eps[rank], rank);

    err = cudaGetLastError();    
    if (err != cudaSuccess) 
      fprintf(stderr, "Error: %s\n", cudaGetErrorString(err));
    cudaDeviceSynchronize();
    
    MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, gt_all_eps, 1, MPI_INT, 0, MPI_COMM_WORLD);
    for(int i=0; i<4; i++)
      gt_all_eps |= gt_all_eps[i];
  }

  MPI_Finalize();
  return 0;
}



void print(double* grid, int padded_grid_size, int time) {
  printf("\n\n\n");
  int i,j;
  
  // we don't want to print the border!
  for(i=1;i<padded_grid_size-1;i++) {
    for(j=1;j<padded_grid_size-1;j++) {
      printf("%.2f ",grid[i*padded_grid_size + j]);
    }
    printf("\n");
  }
}

void save( FILE *f, double* grid, int padded_grid_size,int TIMESTEPS)
{
    int i,j;
    
    static int first = 1;
    if( first )
    {
    fprintf( f, "# %d %d\n", TIMESTEPS, padded_grid_size-2 );
        first = 0;
    }

    for(i = 1; i < padded_grid_size-1; i++ ) {
      for(j=1; j < padded_grid_size-1; j++) {
        fprintf( f, "%.g ", grid[i* padded_grid_size + j] );
      }
      fprintf(f,"\n");
    }

}


//
//  command line option processing
//
int find_option( int argc, char **argv, const char *option )
{
  int i;
  for( i = 1; i < argc; i++ )
    if( strcmp( argv[i], option ) == 0 )
      return i;
  return -1;
}

int read_int( int argc, char **argv, const char *option, int default_value )
{
  int iplace = find_option( argc, argv, option );
  if( iplace >= 0 && iplace < argc-1 )
    return atoi( argv[iplace+1] );
  return default_value;
}

char *read_string( int argc, char **argv, const char *option, char *default_value )
{
  int iplace = find_option( argc, argv, option );
  if( iplace >= 0 && iplace < argc-1 )
    return argv[iplace+1];
  return default_value;
}
