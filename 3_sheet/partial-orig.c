#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void initcond(int nx, double y0[nx], double x[nx]);
void timestep(double t,double dt,double y0);
void RuKu3(double t0, double dt, int nx, double y0[nx], double x[nx], int nghost);
void get_rhs(double t, int nx, double f[nx], double x[nx], double rhs[nx], int nghost );
void der2(int nx, double fx[nx], double x[nx], double der2f[nx], int nghost);

int main(int argc, const char * argv[]){
  
  int bc_type=1;
  double wait=0.001;
  double dt=1e-5;
  int nsteps=1000000000;
  int nx=10000;

  nx = atoi(argv[1]);
  dt = atof(argv[2]);

  int nghost=1;
  double verbose=0;
  double y0[nx];
  double x[nx];
  double t=0.0;
  int i=0;
  double ymax,ymean,ymin;
  double tmax=1000.;
  
  initcond(nx,y0,x);
 
  //-------------------------------------------------------------------------
  //  Starting loop over time
  //-------------------------------------------------------------------------
  // printf("\n Starting:\n");
  double start = omp_get_wtime();
  while (i<nsteps && t<tmax) {
    RuKu3(t, dt,nx,y0,x,nghost);
    t = t+dt;
    if (i%1000 == 0) {
      ymin = ymax = ymean = y0[0];
      for (int j=1;j<nx;j++) {
	if (y0[j] > ymax) {ymax = y0[j];}
	if (y0[j] < ymin) {ymin = y0[j];}
	ymean += y0[j];
      }
    }  
    i+=1;
  }
  printf("%f\n", omp_get_wtime() - start);
  printf(" Timestep, time %i %f \n",i,t);
  printf(" ymin ymax ymean %f %f %f \n",ymin,ymax,ymean/nx);
  return 0;
}
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
void get_rhs(double t, int nx, double f[nx], double x[nx], double rhs[nx], int nghost ) {
  // 
  //  This function provides the right hand side (RHS) of the differential equations
  // 
  double der2f[nx];  // Second derivative of f
  double derf[nx];   // First derivative of f
  // #########################################
  // advection equation with constent velocity
  //
  //der2(nx,f,x,derf,nghost);
  //for (int i=0;i<nx;i++) {
  //  rhs[i] = -0.5 * derf[i];
  //}
  //
  // ##################
  // diffusion equation 
  //
  der2(nx,f,x,der2f,nghost);
  //

  for (int i=0;i<nx;i++) {
    rhs[i] = 0.001 * der2f[i];
  }
}
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
void RuKu3(double t,double dt,int nx, double y0[nx], double x[nx], int nghost) {
  //
  double t1,t2;
  double k2[nx],y2[nx];
  double k3[nx],y[nx];
  double rhs[nx];
  double k1[nx],y1[nx];
  //
  get_rhs(t,nx,y0,x,rhs,nghost );

  for (int i=0;i<nx; i++) {
    k1[i] = rhs[i] * dt;
    y1[i] = y0[i] + k1[i]*8/15;
  }
  t1 = t + dt*8/15;
  //
  get_rhs(t1,nx,y1,x,rhs,nghost );
  for (int i=0;i<nx; i++) {
    k2[i] = rhs[i] * dt;
    y2[i] = y0[i] + k1[i]/4 + k2[i]*5/12;
  }
  t2 = t + dt*2/3;
  //
  get_rhs(t2,nx,y2,x,rhs,nghost );
  for (int i=0;i<nx; i++) {
    k3[i] = rhs[i] * dt;
    y0[i] =  y0[i] + k1[i]/4 + k3[i]*3/4;
  }
}
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
void initcond(int nx, double y0[nx], double x[nx]) {
  // Initialize with a gaussian blob
  //
  double width=0.2;
  double ampl=1.;
  double x0=0.;
  //
  for (int i=0; i<nx; i++) {
    x[i] = (double)2.*i/(nx-1.)-1.;
    y0[i] = ampl * exp(-pow(x[i]-x0,2.)/width);
  }
}
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
void der2(int nx, double fx[nx], double x[nx], double der2f[nx], int nghost) {
  //
  double dx;
  //
  dx =x[1]-x[0];
  switch (nghost) {
    // -----------------------------------
  case 1: 
    for (int i=nghost;i < nx-nghost; i++) {
      der2f[i] =				\
	+1.*fx[i-nghost]			\
	-2.*fx[i]				\
	+1.*fx[i+nghost];  
    }
    // Periodic boundary
    der2f[0] =					\
      +1.*fx[nx-1]				\
      -2.*fx[0]					\
      +1.*fx[nghost];  
    der2f[nx-1] =					\
      +1.*fx[nx-2]					\
      -2.*fx[nx-1]					\
      +1.*fx[0];  
    
    for (int i=0;i<nx;i++) {
      der2f[i]=der2f[i]/dx/dx;
    }
    break;
  default: 
    printf("nhgost must be 1");
    break;
  }
}
