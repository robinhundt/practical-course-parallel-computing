#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_GRIDSIZE 128

// Save/ Print only every nth step:
#define PRINTSTEP 1

// Run for that many time steps
#define DEFAULT_TIMESTEPS 1000

void init_cells(double* grid, int gridsize);
void print(double* grid, int padded_grid_size, int time);
void save(FILE *f, double* grid, int padded_grid_size, int time);
int find_option( int argc, char **argv, const char *option );
int read_int( int argc, char **argv, const char *option, int default_value );
char *read_string( int argc, char **argv, const char *option, char *default_value );



int main(int argc, char** argv) {
  
  if( find_option( argc, argv, "-h" ) >= 0 )
  {
      printf( "Options:\n" );
      printf( "-h to see this help\n" );
      printf( "-n <int> to set the grid size\n" );
      printf( "-t <int> to set the number of time steps\n" );
      printf( "-o <filename> to specify the output file name\n" );
      return 0;
  }
  
  int GRIDSIZE = read_int( argc, argv, "-n", DEFAULT_GRIDSIZE );
  // Check gridsize for some basic assumptions
  if(GRIDSIZE%2) {
    printf("Only even Gridsize allowed!\n");
    return 1;
  }
  
  int TIMESTEPS = read_int( argc, argv, "-t", DEFAULT_TIMESTEPS );


  char *savename = read_string( argc, argv, "-o", "sample_conduct.txt" );
  FILE *f = savename ? fopen( savename, "w" ) : NULL;
  if( f == NULL )
  {
      printf( "failed to open %s\n", savename );
      return 1;
  }
  
  
  int t,i,j;
  double *T, *Tn;
  
  // Allocate Grid with a padding on every side for convenience
  int padded_grid_size = GRIDSIZE+2;
  T=(double *) malloc((padded_grid_size)*(padded_grid_size)*sizeof(double));
  
  // temp grid to hold the calculated data for the next time step
  Tn=(double *) malloc((padded_grid_size)*(padded_grid_size)*sizeof(double));

  
  // remember -- our grid has a border around it!
  init_cells(T,padded_grid_size);
  
  
  for(t=0;t<TIMESTEPS;t++) { // Loop for the time steps
    
    // Calculate grid cells for next timestep
    for(FIXME: loop over total grid without(!) padding, might use 2 loops for convenience) {
        FIXME:
        Tn[cell] = average from neighborcells T[neighbors]
    }

    // copy new grid into old one
    for(FIXME: LOOP over total grid without padding,might use 2 loops for convenience) {
        FIXME:
        T[] = Tn[];
    }
    
    if(!(t % PRINTSTEP)) {
      // print(T,padded_grid_size,t);
        save(f,T,padded_grid_size,t);
    }
    
  }
  
  fclose(f);
  
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

void save( FILE *f, double* grid, int padded_grid_size,int time)
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
