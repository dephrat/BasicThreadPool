# BasicThreadPool
This is just my first attempt at implementing a C-style thread pool using POSIX threads (pthread.h). 
The point is to prepare for turning my 'PuzzleSolver' project multithreaded.

Design details:  
- The main thread creates the first task
- Threads consume and produce tasks
- The program ends when all tasks are done

How to run the program:
- `cd` into the repo
- `g++ -std=c++2a main.cpp` (testing was done using c++2a, haven't tested with other versions)
- `./a.out`
