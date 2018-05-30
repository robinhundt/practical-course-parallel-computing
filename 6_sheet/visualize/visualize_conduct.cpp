#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>    
#include <vector>
#include <sys/time.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <math.h>

#define DEFAULT_FILENAME "sample_conduct.txt"
#define eps 0.1
#define SCALE 200
#define FPS 1
#define MIN_SIZE 100
#define COLORSCALE 1e3

	
double read_timer( )
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
	
    gettimeofday( &end, NULL );
	
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

void make_surface( int sx, int sy )
{
    if( SDL_SetVideoMode( sx, sy, 16, SDL_OPENGL | SDL_RESIZABLE ) == NULL )
    {
        printf( "SDL_SetVideoMode failed: %s\n", SDL_GetError( ) );
        exit( 1 );
    }
	
    glEnable( GL_POINT_SMOOTH );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
    glPointSize( 2 );
    glClearColor( 1, 1, 1, 1 );	
   
    glViewport (0, 0, sx, sy);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();  
}

int main( int argc, char *argv[] )
{
    const char *filename = argc > 1 ? argv[1] : DEFAULT_FILENAME;
	
    FILE *f = fopen( filename, "r" );
    if( f == NULL )
    {
        printf( "failed to find %s\n", filename );
        return 1;
    }
    
    int nframes,n;
    fscanf( f, "# %d%d", &nframes, &n);
    int size=n;
    int window_size = (int)(1024);
    window_size = window_size > MIN_SIZE ? window_size : MIN_SIZE;
	
    //
    // init graphics system
    //
    if( SDL_Init( SDL_INIT_VIDEO ) != 0 )
    {
        printf( "SDL_Init failed: %s\n", SDL_GetError( ) );
        return 1;
    }
	
    make_surface( window_size, window_size );
    
    int frames=0;
    std::vector<double> T;
	
    for( bool done = false; !done; )
    {
    
        SDL_Event event;
        while( SDL_PollEvent( &event ) )
        {
            if( ( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ) || ( event.type == SDL_QUIT ) )
                done = true;
            else if( event.type == SDL_VIDEORESIZE )
                make_surface( event.resize.w, event.resize.h );
        }
        
        
        
        if(frames < nframes) {
            frames++;
            double temp;
            T.clear();
            for(int i=0;i<n*n;i++) {
                fscanf( f, " %lf ", &temp );
                T.push_back( temp );
            }
        }
    
        
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity( );
        gluOrtho2D( -eps, size+eps, -eps, size+eps );
        
        glColor3f( 0.75, 0.75, 0.75 );
        glBegin( GL_LINE_LOOP );
        glVertex2d( 0, 0 );
        glVertex2d( size, 0 );
        glVertex2d( size, size );
        glVertex2d( 0, size );
        glEnd( );
        
        glColor3f( 0, 0, 0 );

        for( int i = 0; i < n; i++ ) {
            for (int j=0;j<n;j++) {
                glLoadIdentity();
                glTranslatef(2*(i * 1.0/n)-1,2*(j * 1.0/n)-1,0.0f);
                
                glBegin( GL_QUADS );
                
                glColor3f(log(100000*T[i*n+j]+1)/11.5, 0 ,1-(log(100000*T[i*n+j]+1)/11.5));
                glVertex2f(0,0);
                glVertex2f(0,2.0/n);
                glVertex2f(2.0/n,2.0/n);
                glVertex2f(2.0/n,0);
                glEnd( );
            }
        }
        
        
        SDL_GL_SwapBuffers ();
    }

	
    fclose( f );
    SDL_Quit();
	
    return 0;
}
