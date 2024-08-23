# BasicThreadPool
This is just my first attempt at implementing a C-style thread pool using POSIX threads (pthread.h). 
The point is to prepare for turning my 'PuzzleSolver' project multithreaded.

Design details:  
- Threads are both consumers and producers
- The main thread creates the first task
- The program ends when all tasks are done
