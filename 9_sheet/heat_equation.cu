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

__global__ void init_matrix(float *A, int rank, int n_block = 512, n = 1024) {
  int i = threadIdx.y + blockIdx.y * (n_block / gridDim.y);
  int end_i = i + (n_block / gridDim.y / blockDim.y);
  if(rank == 2 || rank == 3) {
    i += n_block;
    end_i += n_block;
  }
  int j = threadIdx.x + blockIdx.x * (n_block / gridDim.x);
  int end_j = j + (n_block / gridDim.x / blockDim.x);
  if(rank == 0 || rank == 3) {
    j += n_block;
    end_j += n_block;
  }

  for(; i < end_i; i++) {
    for(; j < end_j; j++) {
      if(i < n/2 && j < n/2 || n/2 >= i && n/2 >= j)
        A[i*n+j] = 1;
      else  
        A[i*n+j] = 0;
    }
  }

}

__global__ void jacobi_iteration(float *A_in, float *A_out,
                                bool *gt_eps, int rank, float eps = 0.01,
                                int n_block = 512, int n = 1024) {
  __shared__ gt_eps_block = false;
  int i = threadIdx.y + blockIdx.y * (n_block / gridDim.y);
  int end_i = i + (n_block / gridDim.y / blockDim.y);
  if(rank == 2 || rank == 3) {
    i += n_block;
    end_i += n_block;
  }
  int j = threadIdx.x + blockIdx.x * (n_block / gridDim.x);
  int end_j = j + (n_block / gridDim.x / blockDim.x);
  if(rank == 0 || rank == 3) {
    j += n_block;
    end_j += n_block;
  }

  for(; i < end_i; i++) {
    for(; j < end_j; j++) {
      if (0 < i && i < n - 1.0 && 0 < j && n - 1.0)
        A_out[i*n+j] = (A_in[(i-1)*n+j] + A_in[(i+1)*n+j] \ 
                        + A_in[i*n+j-1] + A_in[i*n+j+1]) / 4.0;
      if(fdimf(A_out[i*n+j], A_in[i*n+j]) > eps)
        gt_eps_block = true;
    }
  }
  gt_eps[blockIdx.y * gridDim.x + blockIdx.x] = gt_eps_block;
}

void exchange_borders(float *A, int rank, int n = 1024, int processes = 4) {
  int before = abs((rank - 1) % 4);
  int next = (rank + 1) % 4;

  MPI_Sendrecv()
}


int main(int argc, char** argv) {

  int numtasks, rank;
    
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Status status;
  cudaError_t err = cudaGetLastError();  
  
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
  bool *gt_eps;
  float *A;

  cudaMallocManaged(&A_block, sizeof *A_block * 1024 * 1024);
  cudaMallocManaged(&A_block_tmp, sizeof *A_block_tmp * 1024 * 1024);
  init_matrix<<<1, 256>>>(A_block, rank);
  if (err != cudaSuccess) 
    fprintf(stderr, "Error: %s\n", cudaGetErrorString(err));
  
  cudaDeviceSynchronize();


  dim3 nb(4,4);
  dim3 nt(1, 128)
  cudaMallocManaged(&gt_eps, sizeof *gt_eps * 4 * 4);
  while(**something**) {
    jacobi_iteration<<<nb, nt>>>(A_block, A_block_tmp, gt_eps, rank);
  }


  double *T, *T_block, *Tn_block;
  
  // Allocate Grid with a padding on every side for convenience
  int padded_grid_size = GRIDSIZE+2;
  int blocksize =   GRIDSIZE / numtasks;
  if(rank == 0) {
    T=(double *) malloc((padded_grid_size)*(padded_grid_size)*sizeof(double));
    // temp grid to hold the calculated data for the next time step
    init_cells(T,padded_grid_size);
  }

  T_block = (double *) malloc(padded_grid_size * (blocksize+2) * sizeof(double));
  Tn_block = (double *) malloc(padded_grid_size * (blocksize+2) * sizeof(double));

  MPI_Scatter(&T[padded_grid_size], padded_grid_size * blocksize, MPI_DOUBLE, &T_block[padded_grid_size],
              padded_grid_size * blocksize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  // remember -- our grid has a border around it!
  
  int last_row = padded_grid_size * (blocksize + 1);
  int next_to_last_row = last_row - padded_grid_size;

  for(int t=0;t<TIMESTEPS;t++) { // Loop for the time steps
    if(rank > 0 && rank < numtasks-1) {
      MPI_Sendrecv(&T_block[padded_grid_size], padded_grid_size, MPI_DOUBLE, rank-1, 42, 
                    T_block, padded_grid_size, MPI_DOUBLE, rank-1, 42, MPI_COMM_WORLD, NULL);
      MPI_Sendrecv(&T_block[next_to_last_row], padded_grid_size, MPI_DOUBLE, rank+1, 42, 
                    &T_block[last_row], padded_grid_size, MPI_DOUBLE, rank+1, 42, MPI_COMM_WORLD, NULL);
    } else if(rank == 0) {
      MPI_Sendrecv(&T_block[next_to_last_row], padded_grid_size, MPI_DOUBLE, 1, 42, &T_block[last_row], padded_grid_size,
                  MPI_DOUBLE, 1, 42, MPI_COMM_WORLD, &status);
    } else if(rank == numtasks-1) {
      MPI_Sendrecv(&T_block[padded_grid_size], padded_grid_size, MPI_DOUBLE, rank-1, 42, T_block, padded_grid_size,
                  MPI_DOUBLE, rank-1, 42, MPI_COMM_WORLD, &status);
    }
    // Calculate grid cells for next timestep
    for(int i=1; i<blocksize+1; i++) {
      for(int j=1; j<padded_grid_size-1; j++) {
        Tn_block[i*padded_grid_size + j] = (T_block[(i-1)*padded_grid_size+j] + T_block[i*padded_grid_size + (j-1)] \
          + T_block[i*padded_grid_size+(j+1)] + T_block[(i+1)*padded_grid_size+j]) / 4.0;
      }
    }
    // copy new grid into old one
    for(int i=1; i<blocksize+1; i++) {
      for(int j=1; j<padded_grid_size-1; j++) {
        T_block[i*padded_grid_size+j] = Tn_block[i*padded_grid_size+j];
      }
    }
    if(!(t % PRINTSTEP)) {
      MPI_Barrier(MPI_COMM_WORLD);    
      MPI_Gather(&T_block[padded_grid_size], blocksize*padded_grid_size, MPI_DOUBLE, &T[padded_grid_size],
                blocksize*padded_grid_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      if(rank == 0)
        save(f,T,padded_grid_size,TIMESTEPS);
    }
  }
  
  fclose(f);
  
  MPI_Finalize();

  return 0;
}


void init_cells(double* grid, int gridsize) {
  int i,j;
  
  // set everything to zero, even the border
  for(i=0;i<gridsize;i++) {
    for(j=0;j<gridsize;j++) {
      grid[i*gridsize + j]=0;
    }
  }
  
  // but the most inner 4 cells
  for(i=gridsize/2-1;i<=gridsize/2;i++) {
    for(j=gridsize/2-1;j<=gridsize/2;j++) {
      grid[i*gridsize + j]=1;
    }
  }
  
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
