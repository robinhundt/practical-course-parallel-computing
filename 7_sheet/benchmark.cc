#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void save( int argc, char **argv,int numberOfTests, double *t, double *tmin, double *tmax, int nloop);
int find_option( int argc, char **argv, const char *option );
int read_int( int argc, char **argv, const char *option, int default_value );
char *read_string( int argc, char **argv, const char *option, char *default_value );


#define NUMBER_OF_TESTS 21
#define DEFAULT_NLOOP 1000

int main( int argc, char **argv ){
  double       *buf;
  int          rank;
  int          n;
  int          nOld;
  int          numberOfTests;
  double       t1, t2, *tmin, *tmax, *t;
  int          j, nloop;
  int          numberOfRunsPerTest;
  MPI_Status   status;


  if( find_option( argc, argv, "-h" ) >= 0 ){
      printf( "Options:\n" );
      printf( "-h to see this help\n" );
      printf( "-n <int> to set numbers of send messages\n" );
      printf( "-t <int> to set the number of tests (Each test is run with a greater buffersize. Starting with 21 and increasing per run using the fibonacci function) \n" );
      printf( "-o <filename> to specify the output file name\n" );
      printf( "-r <int> to specify how often a test is repeated\n");
      return 0;
  }


  MPI_Init( &argc, &argv );

  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  nOld = 8;
  n=13;
  nloop= read_int( argc, argv, "-n", DEFAULT_NLOOP );
  numberOfTests = read_int(argc,argv,"-t",NUMBER_OF_TESTS);
  numberOfRunsPerTest= read_int(argc,argv,"-r",100);
  tmin = (double *)malloc(numberOfTests*sizeof(double));
  tmax = (double *)malloc(numberOfTests*sizeof(double));
  t = (double *)malloc(numberOfTests*sizeof(double));
  
  //start test
  for (int i=0; i<numberOfTests; i++) {
    //update n and allocate memory
    int tmp= n;
    n = n+nOld;
    nOld = tmp;
    buf = (double *)malloc(n*sizeof(double));
    //printf("Test: %d Buffersize %d\n",i,(n*sizeof(double)));
    //testruns in this test
    for (j=0;j<numberOfRunsPerTest;j++){
        if (rank == 0) {
          MPI_Barrier(MPI_COMM_WORLD);
          /* Make sure both processes are ready */
          t1 = MPI_Wtime();
          /* loop here */
          for(int k=0; k<nloop;k++){
            MPI_Ssend( buf, n, MPI_DOUBLE, 1, k, MPI_COMM_WORLD );
            MPI_Recv( buf, n, MPI_DOUBLE, 1, k, MPI_COMM_WORLD, 
          &status ); 
          }
          t2 = MPI_Wtime();

          //compute needed time for this run and update tmin and tmax
          double time = t2-t1;
          //addup t
          t[i]+=time;

          //if last run for this testcase compute average of t
          if(j+1==numberOfRunsPerTest)
            t[i]=t[i]/10;

          //update tmin and tmax
          if(j==0){
            tmax[i]=tmin[i]=time;
          }else{
	    if(time<tmin[i])
           	 tmin[i]=time;
            if(time>tmax[i])
           	 tmax[i]=time;
	  }  
      } else if (rank == 1) {
          MPI_Barrier(MPI_COMM_WORLD);
          /* Make sure both processes are ready */
          /* loop here*/ 
          for(int j =0; j<nloop;j++){
            MPI_Recv( buf, n, MPI_DOUBLE, 0, j, MPI_COMM_WORLD, &status );
            MPI_Ssend( buf, n, MPI_DOUBLE, 0, j, MPI_COMM_WORLD );
          }
        }else{ //if running with more then 2 nodes also let them enter the barrier
            MPI_Barrier(MPI_COMM_WORLD);            
        }
    }
    free(buf);
  }
  
  /* Convert to half the round-trip time  ???? */
  if(rank == 0)
    save(argc,argv,numberOfTests,t,tmin,tmax, nloop);
  free(tmin);
  free(tmax);
  free(t);
  MPI_Finalize( );
  return 0;
}

/**
 * save function
 * stores the buffersize, average time, minmal time and maximal time needed in the specified savefile
 */
void save( int argc, char **argv, int numberOfTests, double *t, double *tmin, double *tmax, int nloop){
  char *savename = read_string( argc, argv, "-o", "benchmark.csv" );
  FILE *f = savename ? fopen( savename, "w" ) : NULL;
  if( f == NULL ){
      printf( "failed to open %s\n", savename );
      return;
  }

  fprintf( f, "buffersize;t_average;t_min;t_max;send messages\n");
  int buffersize=13, buffersizeOld=8;
  for( int i = 0; i < numberOfTests; i++ ){
    int tmp= buffersize;
    buffersize = buffersize+buffersizeOld;
    buffersizeOld = tmp;
    fprintf( f, "%d;%g;%g;%g;%d\n", buffersize *sizeof(double),t[i], tmin[i],tmax[i],nloop);
  }
  fclose(f);
}

int find_option( int argc, char **argv, const char *option )
{
    for( int i = 1; i < argc; i++ )
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
