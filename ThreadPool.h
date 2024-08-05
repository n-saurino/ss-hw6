#pragma once
#include <atomic>
#include <deque>
#include <future>
#include <thread>
#include <vector>

/*
Thread Pool Class: Implement a thread pool class that:
1. Initializes a fixed number of worker threads.
2. Allows tasks to be submitted to a queue.
3. Worker threads continuously pick tasks from the queue and execute them.
4. Supports task synchronization using futures and promises.
*/

/*
I found Oğuzhan KATLI's implementation at https://nixiz.github.io/yazilim-notlari/aboutme 
and https://stackoverflow.com/questions/15752659/thread-pooling-in-c11 helpful to reference
*/

class ThreadPool
{
private:
    /* data */
    std::atomic_bool is_active = false;
    std::vector<std::thread> threads;
    std::deque<std::function<void()>> pending_jobs;
    std::mutex mtx;
    std::condition_variable cv;

public:
    ThreadPool();
    ~ThreadPool();
    
    bool Busy(){

    }

    void Run(){
        
    }

    void Start(){
        const uint32_t num_threads = std::thread::hardware_concurrency(); // gets the number of threads supported by system
        for(uint32_t i = 0; i < num_threads; ++i){
            threads.emplace_back(std::thread(&ThreadPool::Run, this));
        }
    }

    void Stop(){

    }

    void QueueJob(const std::function<void()>& job){

    }

};
