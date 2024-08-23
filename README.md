# BasicThreadPool
This is just my first attempt at implementing a C-style thread pool using POSIX threads (pthread.h). 
The point is to prep for turning my 'PuzzleSolver' project multithreaded.

Design details:
Main thread creates the first task.  
Thread pool takes tasks, executes them, and returns the result to the queue for other threads to work on.  
Once all tasks are complete, the program stops.
