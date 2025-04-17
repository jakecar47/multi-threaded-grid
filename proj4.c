#include <pthread.h>
#include <stdio.h>
#include "proj4.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


void initializeGrid(grid * g, char * fileName) {
  // Open the file
  int fd = open(fileName, O_RDONLY);
  if (fd < 0) {
    perror("open failed");
    exit(EXIT_FAILURE);
  }

  // Find the size of the file
  struct stat st;
  if (fstat(fd, &st) < 0) {
    perror("Error finding file size.\n");
    close(fd);
    exit(EXIT_FAILURE);
  }

  // Allocate memory for a buffer to store the file contents
  char * buffer = malloc(st.st_size + 1);
  if (!buffer) {
    perror("The call to malloc failed.\n");
    close(fd);
    exit(EXIT_FAILURE);
  }

  // Read through the entire file and store the values in buffer
  ssize_t bytesRead = read(fd, buffer, st.st_size);
  if (bytesRead < 0) {
    perror("Error reading the file.\n");
    free(buffer);
    close(fd);
    exit(EXIT_FAILURE);
  }
  buffer[bytesRead] = '\0'; // Null terminate the last character in the buffer
  close(fd);

  // Count number of rows and columns
  unsigned int n = 0;
  for (ssize_t i = 0; i < bytesRead; i++) {
    if (buffer[i] == '\n') {
      n++;
    }
  }

  // Assign value for n (num rows and cols) and allocate memory for p
  g->n = n;
  g->p = malloc(n * sizeof(unsigned char *));
  for (unsigned int i = 0; i < n; i++) {
    g->p[i] = malloc(n * sizeof(unsigned char));
  }

  // Fill grid from the buffer
  unsigned int row = 0, col = 0;
  for (ssize_t i = 0; i < bytesRead && row < n; i++) {
    if (buffer[i] >= '0' && buffer[i] <= '9') {
      g->p[row][col++] = buffer[i] - '0';
    } else { // If the end of the row is reached, move to next row, first column
      row++;
      col = 0;
    }
  }
  free(buffer); // Free the memory allocated to the buffer
}


void diagonalSums(grid * input, unsigned long s, grid * output, int t) {
  // Initialize n
  unsigned int n = input->n;

  // Initialize output grid
  output->n = n;
  output->p = malloc(n * sizeof(unsigned char *));
  for (unsigned int i = 0; i < n; i++) {
    output->p[i] = calloc(n, sizeof(unsigned char)); // one column
  }

  // If t = 1, only use the programs single thread
  if (t == 1) {
    thread_args args = {input, output, s, 0, 1};
    compute_diagonal_sums(&args);
  } else {
    // Create an array of threads to store up to 2 new threads
    pthread_t threads[2];
    thread_args args[2];

    // Determine if 1 or 2 threads are to be created
    int actual_threads = t > 2 ? 2 : t;

    // Assign properties of each thread
    for (int i = 0; i < actual_threads; i++) {
      args[i].input = input;
      args[i].output = output;
      args[i].s = s;
      args[i].thread_id = i;
      args[i].total_threads = actual_threads;

      // Allow each thread to perform its work on the diagonals
      pthread_create(&threads[i], NULL, compute_diagonal_sums, &args[i]);
    }

    // After each thread is finished, join it to the main thread
    for (int i = 0; i < actual_threads; i++) {
      pthread_join(threads[i], NULL);
    }
  }  
}


void writeGrid(grid * g, char * fileName) {
  // Determine if the file exists or not
  int file = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Attempt to open the file
  if (file < 0) {
    printf("Error opening the file.\n");
    exit(EXIT_FAILURE);
  }

  // Read through the grid and add each digit to the file
  char digit;
  for (int i = 0; i < g->n; i++) {
    for (int j = 0; j < g->n; j++) {
      digit = g->p[i][j] + '0';
      write(file, &digit, 1);
    }
    write(file, "\n", 1); // End of current row
  }
}


void freeGrid(grid * g) {
  // Free each value in the grid
  for (unsigned int i = 0; i < g->n; i++) {
    free(g->p[i]);
  }
  // Free the struct
  free(g->p);
}


void * compute_diagonal_sums(void *arg) {
  thread_args *args = (thread_args *)arg;
  unsigned int n = args->input->n;
  unsigned int thread_id = args->thread_id;
  unsigned int total = args->total_threads;
  unsigned long target = args->s;

  // Outer loop to iterate through the rows
  for (unsigned int start_row = thread_id; start_row < n; start_row += total) {
    // Inner loop to iterate through all the columns of each row
    for (unsigned int start_col = 0; start_col < n; start_col++) {

      // Initialize variables for traversing the right diagonals at each value
      unsigned long rightSum = 0;
      unsigned int row = start_row;
      unsigned int col = start_col;
      bool rightMatched = false;
      unsigned int end_row = start_row;
      unsigned int end_col = start_col;

      // Traverse the right diagonals at each value until target is reached or exceeded
      while (row < n && col < n && !rightMatched && rightSum <= target) {
        rightSum += args->input->p[row][col];
        if (rightSum == target) { // Check for diagonal sums == target
          rightMatched = true;
          end_row = row;
          end_col = col;
        }
	// Go to next value in the diagonal
        row++;
        col++;
      }

      // If match is found, retrace values and add them to the output grid
      if (rightMatched) {
	// Return to the start of the diagonal
        row = start_row;
        col = start_col;
        while (row <= end_row && col <= end_col) { // Loop until the end of the identified diagonal sum
          args->output->p[row][col] = args->input->p[row][col]; // Add value
          row++; 
          col++;
        }
      }

      // Initialize variables for traversing the left diagonals at each value
      unsigned long leftSum = 0;
      row = start_row;
      col = start_col;
      bool leftMatched = false;
      end_row = start_row;
      end_col = start_col;

      // Traverse the left diagonals at each value until target is reached or exceeded
      while (row < n && col < n && !leftMatched && leftSum <= target) {
        leftSum += args->input->p[row][col];
        if (leftSum == target) { // Check for diagonal sums == target
          leftMatched = true;
          end_row = row;
          end_col = col;
        }

	// Go to next value in the diagonal
        row++;
        if (col > 0) {
          col--;
        } else {
          row = n;  // safely exit loop
        }
      }

      // If match is found, retrace values and add them to the output grid
      if (leftMatched) {
	// Return to the start of the diagonal
        row = start_row;
        col = start_col;
	while (row <= end_row && col >= end_col && col < n) { // Loop until the end of the identified diagonal sum
          args->output->p[row][col] = args->input->p[row][col]; // Add value
          row++;
          if (col > 0) {
            col--;
          } else {
            row = end_row + 1;
          }
        }
      }
    }
  }
  // Return a null pointer
  return NULL;
}
