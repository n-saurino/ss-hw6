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
    std::atomic_bool is_active_{true};
    std::vector<std::thread> threads_;
    std::deque<std::function<void()>> pending_jobs_;
    std::mutex mtx_;
    std::condition_variable cv_;

    void Run(){
        // perpetual while loop
        while(true){

            // declare a job
            std::function<void()> job;

            // create a new scope with {}
            {
                // grab the lock
                std::unique_lock<std::mutex> uniq_lock(mtx_);
                
                // set this thread to wait on the lock and make sure that the q is not empty 
                // and that the threadpool is still active
                cv_.wait(uniq_lock, [this]{return !pending_jobs_.empty() || !is_active_;});

                // if !is_active_ -> null
                if(!is_active_ && pending_jobs_.empty()){
                    return;
                }

                // set job = front of pending_jobs
                job = std::move(pending_jobs_.front());

                // pop pending_jobs
                pending_jobs_.pop_front();
            }
            job();
        }
    }

public:
    ThreadPool(){
        Start();
    }

    ~ThreadPool(){
        Stop();
    }
    
    bool Busy(){
        bool pool_busy;
        {
            std::unique_lock<std::mutex> uniq_lock(mtx_);
            pool_busy = !pending_jobs_.empty();
        }
        return pool_busy;
    }

    void Start(){
        const uint32_t num_threads = std::thread::hardware_concurrency(); // gets the number of threads supported by system
        for(uint32_t i = 0; i < num_threads; ++i){
            threads_.emplace_back(std::thread(&ThreadPool::Run, this));
        }
    }

    void Stop(){
        {
            std::unique_lock<std::mutex> uniq_lock(mtx_);
            is_active_ = false;
        }
        // need to notify all threads that are waiting
        cv_.notify_all();

        // need to set each thread in thread pool to join
        for(std::thread& t : threads_){
            t.join();
        }

        threads_.clear();
    }

    // add a new job to the queue
    // use a lock to make sure there isn't a data race
    // then notify a waiting thread with the conditional variable
    
    void QueueJob(const std::function<void()>& job){
        {
            std::unique_lock<std::mutex> uniq_lock(mtx_);
            pending_jobs_.push_back(job);
        }
        cv_.notify_one();
    }
    
    

    template <typename F, typename... Args>
    auto QueueJob(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> uniq_lock(mtx_);
            pending_jobs_.emplace_back([task]() { (*task)(); });
        }
        cv_.notify_one();
        return res;
    }
};
