#ifndef PROJ4_H
#define PROJ4_H

/*
 * The struct grid_t contains a pointer p to a 2D array of 
 * unsigned chars with n rows and n columns stored on
 * the heap of this process. Once this is initialized properly,
 * p[i][j] should be a valid unsigned char for all i = 0..(n-1)
 * and j = 0..(n-1).
 */
typedef struct grid_t {
  unsigned int n;
  unsigned char ** p;
} grid;


/*
 * Initialize g based on fileName, where fileName
 * is a name of file in the present working directory
 * that contains a valid n-by-n grid of digits, where each
 * digit in the grid is between 1 and 9 (inclusive).
 */
void initializeGrid(grid * g, char * fileName);


/*
 * This function will compute all diagonal sums in input that equal s using
 * t threads, where 1 <= t <= 3, and store all of the resulting
 * diagonal sums in output. Each thread should do
 * roughly (doesn't have to be exactly) (100 / t) percent of the 
 * computations involved in calculating the diagonal sums. 
 * This function should call (or call another one of your functions that calls)
 * pthread_create and pthread_join when 2 <= t <= 3 to create additional POSIX
 * thread(s) to compute all diagonal sums.
 */
void diagonalSums(grid * input, unsigned long s, grid * output, int t);


/*
 * Write the contents of g to fileName in the present
 * working directory. If fileName exists in the present working directory, 
 * then this function should overwrite the contents in fileName.
 * If fileName does not exist in the present working directory,
 * then this function should create a new file named fileName
 * and assign read and write permissions to the owner. 
 */
void writeGrid(grid * g, char * fileName);


/*
 * Free up all dynamically allocated memory used by g.
 * This function should be called when the program is finished using g.
 */
void freeGrid(grid * g);


/*
 * Function to compute the diagonal sums of a threads assigned
 * rows. This function iterates through each row, column, and left and right
 * diagonal at each input[row, column] digit
 */
void * compute_diagonal_sums(void *arg);


/*
 * This struct thread_args contains information of each thread.
 * Contains the input grid, output grid, sum value (s), and thread id.
 * Also contains a variable holding the total # of threads. 
 */
typedef struct {
  grid * input;
  grid * output;
  unsigned long s;
  int thread_id;
  int total_threads;
} thread_args;




#endif

