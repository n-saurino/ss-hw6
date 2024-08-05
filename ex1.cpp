#include "ThreadPool.h"
#include <iostream>

int Ex1(){
    int sum = 0;
    ThreadPool* thread_pool = new ThreadPool;

    // Queue 10 jobs that will increment the sum variable
    // sum should equal 10 at the end of the task queue
    for(int i = 0; i < 10; ++i){
        thread_pool->QueueJob([&sum] {sum++;});
    }

    // to make sure that the ThreadPool has completed the entire job queue
    // before we call destructor
    while(thread_pool->Busy()){
        continue;
    }

    // call ThreadPool destructor
    delete thread_pool;
    
    // print to stdout
    std::cout << sum << std::endl;
    return 0;
}