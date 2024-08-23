//Problem-defining values:
#define START_NUM 5
#define BRANCHING_FACTOR 4
#define WAIT_TIME 5000
//Multithreading values:
#define MULTITHREADING true
#define NUM_THREADS 64
//Batch processing values:
#define USE_BATCHING false
#define BATCH_SIZE static_cast<size_t>(4)

#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <unistd.h> 
#include <vector>

struct Task {
    int a;
    Task(int a) : a(a) {}
};

std::list<Task> taskQueue;

pthread_t threadPool[NUM_THREADS];

pthread_mutex_t queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueCond = PTHREAD_COND_INITIALIZER;
int activeThreads = 0;

std::list<Task> execute(Task& task) {
    usleep(WAIT_TIME);
    int a = task.a;
    std::cout << std::to_string(a) << std::flush;
    if (a == 0) return {};
    std::list<Task> ret;
    for (int i = 0; i < BRANCHING_FACTOR; ++i) {
        ret.emplace_back(Task(a-1));
    }
    return ret;
}

void* startup(void* args) {
    while (true) {
        ///consumer: get task
        pthread_mutex_lock(&queueLock);
        while (taskQueue.size() == 0 && activeThreads > 0) {
            //if there are tasks, go get some. 
            //if there are no tasks and some threads are active, wait: more tasks might be on the way.
            //if there are no tasks and no active threads, it's time to finish.
            pthread_cond_wait(&queueCond, &queueLock);
        }
        if (taskQueue.size() == 0) {
            //this is the "no tasks and no active threads" case (see above).
            pthread_cond_broadcast(&queueCond); //wake the sleeping threads
            pthread_mutex_unlock(&queueLock); //can't forget about this!
            break; //exit while loop
        }

        //take tasks from the queue
#if USE_BATCHING
        int numTasksToMove = std::min(taskQueue.size(), BATCH_SIZE);
        std::vector<Task> tasksToProcess;
        for (int i = 0; i < numTasksToMove; ++i) {
            tasksToProcess.push_back(std::move(taskQueue.front()));
            taskQueue.pop_front();
        }
#else
        Task task = std::move(taskQueue.front());
        taskQueue.pop_front();
#endif
        activeThreads++; //this thread took a Task or batch of Tasks, so it's active!
        pthread_mutex_unlock(&queueLock);
        
        ///executor: execute task
        std::list<Task> results;
#if USE_BATCHING
        for (int i = 0; i < numTasksToMove; ++i)
            results.splice(results.end(), execute(tasksToProcess[i]));
#else
        results = execute(task);
#endif

        ///producer: add task
        pthread_mutex_lock(&queueLock);
        //add results to the queue
        taskQueue.splice(taskQueue.end(), results);
        activeThreads--; //thread is no longer going to add Tasks (until it picks up new ones) so it is no longer active!
        pthread_cond_broadcast(&queueCond); //wake up the other threads to take what they can      
        pthread_mutex_unlock(&queueLock);
    }
    return NULL;
}

int main() {
    using Time = std::chrono::steady_clock;
    std::chrono::duration<double, std::milli> fp_ms;
    const auto start = Time::now();

#if MULTITHREADING
    taskQueue.emplace_back(START_NUM);

    pthread_mutex_init(&queueLock, NULL);
    pthread_cond_init(&queueCond, NULL);

    for (int i = 0; i < NUM_THREADS; ++i) 
        if (pthread_create(&threadPool[i], NULL, startup, NULL) != 0) 
            std::cerr << "Failed to create thread" << std::endl;      
    for (int i = 0; i < NUM_THREADS; ++i) 
        if (pthread_join(threadPool[i], NULL) != 0) 
            std::cerr << "Failed to join thread" << std::endl;   

    pthread_mutex_destroy(&queueLock);
    pthread_cond_destroy(&queueCond);
#else
    int i = START_NUM;
    double numTimes = 1;
    while (i >= 0) {
        for (double j = 0; j < numTimes; ++j) {
            usleep(5000);
            std::cout << std::to_string(i) << std::flush;
        }
        i--;
        numTimes *= BRANCHING_FACTOR;
    }
#endif
    const auto end = Time::now();
    fp_ms = end - start;
    std::cout << "Solving finished. Time taken (s): " << fp_ms.count()/1000 << std::endl;
}