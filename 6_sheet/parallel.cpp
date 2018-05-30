#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <mpich/mpi.h>
#include <cstddef>
#include "common.h"

MPI_Datatype particletype;
MPI_Datatype postype;



//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    int numtasks, rank;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int array_of_blocklengths_part[] = {1,1,1,1,1,1};
    MPI_Aint array_of_displacements_part[6];
    array_of_displacements_part[0] = offsetof(particle_t, x);
    array_of_displacements_part[1] = offsetof(particle_t, y);
    array_of_displacements_part[2] = offsetof(particle_t, vx);
    array_of_displacements_part[3] = offsetof(particle_t, vy);
    array_of_displacements_part[4] = offsetof(particle_t, ax);
    array_of_displacements_part[5] = offsetof(particle_t, ay);

    MPI_Datatype array_of_types_part[] = {MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE};

    MPI_Type_create_struct(
        6,
        array_of_blocklengths_part,
        array_of_displacements_part,
        array_of_types_part,
        &particletype);
    MPI_Type_commit(&particletype);

    const int array_of_blocklengths_pos[] = {1,1};
    MPI_Aint array_of_displacements_pos[2];
    array_of_displacements_pos[0] = offsetof(pos_t, x);
    array_of_displacements_pos[1] = offsetof(pos_t, y);

    MPI_Datatype array_of_types_pos[] = {MPI_DOUBLE,MPI_DOUBLE};

    MPI_Type_create_struct(
        2,
        array_of_blocklengths_pos,
        array_of_displacements_pos,
        array_of_types_pos,
        &postype);
    MPI_Type_commit(&postype);

    if(rank == 0 && find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    int n = read_int( argc, argv, "-n", 1000 );

    if(n % numtasks != 0) {
        printf("Number of particles must be a multiple of num. of threads: %d",
                numtasks);
        MPI_Abort(MPI_COMM_WORLD, 1);
    } 
    int partitionSize = n / numtasks;    

    char *savename = read_string( argc, argv, "-o", NULL );
    
    FILE *fsave;
    if (rank == 0)
        fsave = savename ? fopen( savename, "w" ) : NULL;
    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    // stores all positions of particles additionally to the particles themselves
    // this way only the position array needs to be exchanged
    pos_t *positions = (pos_t *)malloc(n*sizeof(pos_t));
    set_size( n );
    // only init particles on master thread and broadcast later
    // this is needed as the seed for the random function in init_particles()
    // is dependant on the time
    if (rank == 0)
        init_particles( n, particles );
    

    // simulate a number of time steps in which each durign each step the forces
    // exhibited by all particles are applied to each other. Then the particles
    // are moved according to their accelaration
    //
    double simulation_time = read_timer( );
    // broadcast initial properties of particles
    MPI_Bcast((void *) particles, n, particletype, 0, MPI_COMM_WORLD);
    // initialize position array which will later be exchanged
    for(int i=0; i<n; i++) {
        positions[i].x = particles[i].x;
        positions[i].y = particles[i].y;       
    }

    for( int step = 0; step < NSTEPS; step++ )
    {
        //
        // compute forces based on the distance of each particle to 
        // all other particles 
        //        
        int startIndex = rank * partitionSize;
        int endIndex = (rank+1)*partitionSize;
        for( int i = startIndex; i < endIndex; i++ )
        {
            particles[i].ax = particles[i].ay = 0;
            for (int j = 0; j < n; j++ )
                apply_force( particles[i], positions[j]);
        }
        
        // print_particle_acc(particles, n);              
        //
        //  move particles according to the accelaration calculated beforehand
        //
        for( int i = startIndex; i < endIndex; i++ ) 
            move( particles[i] );
        for(int i=startIndex; i<endIndex; i++) {
            positions[i].x = particles[i].x;
            positions[i].y = particles[i].y;            
        }

        int rc = MPI_Allgather(MPI_IN_PLACE, 0,
                                MPI_DATATYPE_NULL, positions, partitionSize,
                                postype, MPI_COMM_WORLD);
        
        if(rc != MPI_SUCCESS) {
            printf("Allgather failed: %d\n", rc);
        }
        
        //
        // save every SAVEFREQ step to reduce output file size 
        // bigger save freq. will result in less smooth movement of particles
        //
        // barriers ensure that particles array is not modified during saving
        if(rank == 0 && fsave && (step%SAVEFREQ) == 0 )
            save(fsave, n, positions);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    simulation_time = read_timer( ) - simulation_time;
    
    if(rank == 0)
        printf( "n = %d, simulation time = %g seconds\n", n, simulation_time );
    
    free(particles);
    free(positions);    
    if( fsave && rank == 0)
        fclose( fsave );
    
    MPI_Finalize();
    return 0;
}

/** c)
 * This program can be further parallelized by using vectorization, especially 
 * the computation of the forces exhibited by all particles unto one (line 87-88).
 **/ 
