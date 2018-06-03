### Assignment sheet 06  
Jonas Huegel  
Robin Hundt  

##### Exercise 1:  
The parallelized version is inside the parallel.cpp file. In order to only exchange particle positions we also made changes to the methods inside common.cpp.  
The questions have been answered inside the source code.  
Output files are labelled out-1k-{parallel|serial}.

### Exercise 2:

Versions of the parallelized program are inside {v1|v2|v3}-conduct.c . The questions have been answered in the sourcecode.  
Output of the versions for a gridsize of 128 can be produced by running ./produce-conduct-output (num_taks) and is stored as {v1|v2|v3}-out.  
The comparison between the autovectorized version of v3 and the unvectorized is inside vectorization-data-visualization.pdf.  
