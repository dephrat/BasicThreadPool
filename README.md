# BasicThreadPool
This is just my first attempt at implementing a C-style thread pool using POSIX threads (pthread.h). 
The point is to prepare for turning my 'PuzzleSolver' project multithreaded.

Design details:  
- The main thread creates the first task
- Threads consume and produce tasks
- The program ends when all tasks are done

What it does:
- Start with a number (e.g. 5)
- Print it once, then print num-1 twice, then num-2 four times, then num-3 eight times
- End by printing zero 2^num times
- Note: Play around with the preprocessor value BRANCHING_FACTOR to change how many tasks are created from each previous task

How to run the program:
- `cd` into the repo
- `g++ -std=c++2a main.cpp` (testing was done using c++2a, haven't tested with other versions)
- `./a.out`
