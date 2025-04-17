Jake Caruana 
811664411

To compile in odin: make

To run in odin: ./proj4.out <input file> <output file> <n x n grid size number> <number of threads (1 - 3)>


FINDINGS---------------
Upon creating separate input files and testing execution times using different numbers of threads on different sized grids, I have made some interesting findings. 

SMALL SIZED GRIDS:
With very small input grids of size less than 100, using only 1 thread is faster than using 2 threads while 2 threads is faster than 3 threads. I think this is because the time it takes to create multiple threads is slower than just simply computing the diagonal sums on a small grid.

MEDIUM SIZED GRIDS:
On grids between a size of ~100 to ~3000, Using 1 thread starts to become marginally slower due to the increasing sizes of the grids, in turn requiring much more computation to complete. In these medium sized grids, the programs using 2 threads start to become slightly slower than the programs using 3 threads. The execution time gap slowly increases as grid size increases, however, the difference is still relatively small up until grids start getting larger than 3000 x 3000.

LARGE SIZED GRIDS:
At this point on grids of size over 3000, using 1 thread is atleast twice as slow as using 2 or 3 threads. In addition, I found that as grid sizes climb over ~3000, the execution time difference in using 2 threads vs 3 threads becomes quite noticeable and continues growing as grid size increases. Using 3 threads begins to take atleast seconds less time than 2 threads. I would imagine that with massive grid input sizes of say 10000+, using 3 threads will be much faster than 2 threads. I wish I could test this, but my computer would take forever doing computations on grids of this size   