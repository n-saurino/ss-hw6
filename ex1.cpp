#include "ThreadPool.h"
#include <iostream>
#include <assert.h>

int Ex1(){
    int sum = 0;
    std::mutex sum_mtx;
    ThreadPool* thread_pool = new ThreadPool;

    // Queue 10 jobs that will increment the sum variable
    // sum should equal 10 at the end of the task queue
    for(int i = 0; i < 10; ++i){
        thread_pool->QueueJob([&sum, &sum_mtx] {std::unique_lock<std::mutex> uniq_lock(sum_mtx); sum++;});
    }

    // Submit a simple job that returns a value
    auto future1 = thread_pool->QueueJob([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 1;
    });

    // Submit another job
    auto future2 = thread_pool->QueueJob([](int a, int b) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return a + b;
    }, 2, 3);

    // Submit a job that has no return value
    auto future3 = thread_pool->QueueJob([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << "Hello from the thread pool!" << std::endl;
    });

    // Test the results
    assert(future1.get() == 1);
    assert(future2.get() == 5);
    future3.get(); // This will block until the task is finished

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::unique_lock<std::mutex> uniq_lock(sum_mtx);
        assert(sum == 10);
    }

    std::cout << "All tests passed!" << std::endl;
    
    // call ThreadPool destructor
    delete thread_pool;

    // print to stdout
    return 0;
}