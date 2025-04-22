#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>
#include "proj4.h"


/*
 * Helper function for main. 
 * Check for some errors and print error messages 
 * for command line arguments passed to main.
 * If an error is found, the program terminates.
 */
void errorCheck(int argc, char ** argv){
  bool errorFound = false;
  if(argc != 5){
    printf("Usage error: ./proj4.out inputFile outputFile s t\n");
    errorFound = true;
  }
  else if( access(argv[1], F_OK) != 0 ){
    printf("Error accessing %s in the present working directory\n", argv[1]);
    errorFound = true;
  }
  else {
    int t = atoi(argv[4]);
    if(t < 1 || 3 < t){
      printf("Error: t must be between 1 and 3 (inclusive)\n");
      errorFound = true;
    }
  }
  if(errorFound) exit(0);
}

// Main function to execute the diagonal sums computations and print to standard output
int main(int argc, char ** argv){
  errorCheck(argc, argv);
  char * inputFile = argv[1];
  char * outputFile = argv[2];
  unsigned long sum = (unsigned long) atol(argv[3]);
  int t = atoi(argv[4]);
  grid g, diagonalSumsOutput;
  initializeGrid(&g, inputFile);

  printf("Computing the diagonal sums equal to %ld in a %d-by-%d grid using %d thread(s).\n",
         sum, g.n, g.n, t);
  struct timeval start, end;    // start and end time
  unsigned long e_usec; // elapsed microseconds
  gettimeofday(&start, 0); // mark the start time
  diagonalSums(&g, sum, &diagonalSumsOutput, t);
  gettimeofday(&end, 0);        // mark the end time
  e_usec = ((end.tv_sec * 1000000) + end.tv_usec) -
    ((start.tv_sec * 1000000) + start.tv_usec);
  printf("Elapsed time for computing the diagonal sums using %d thread(s): %lf seconds.\n",
         t, e_usec / 1000000.0);

  printf("Writing the diagonal sums equal to %ld to the file %s.\n",
         sum, outputFile);
  writeGrid(&diagonalSumsOutput, outputFile);
  freeGrid(&g);
  freeGrid(&diagonalSumsOutput);
  printf("Program is complete. Goodbye!\n");
  return 0;
}
