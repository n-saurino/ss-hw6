# ss-hw6

**Overview**

In Week 6, you will explore advanced threading concepts such as thread pools, futures, promises, and asynchronous tasks. This assignment will help you understand how to manage concurrent tasks more efficiently and prepare you for building high-performance, multithreaded applications.

**Objectives**

1. Understand the concept and implementation of thread pools.
2. Learn to use futures and promises for task synchronization.
3. Implement asynchronous tasks using `std::async`.

**Reading**

- **Primary Texts:**
    - “C++ Concurrency in Action” by Anthony Williams
        - Chapter 6: The C++ memory model and operations
        - Chapter 7: Designing lock-based concurrent data structures
        - Chapter 8: Designing lock-free concurrent data structures
        - Chapter 9: Designing concurrent code
    - “Concurrency in C++: Practical Multithreading” by Cameron Hughes and Tracey Hughes
        - Chapter 10: Concurrent Containers and the STL
        - Chapter 11: Designing Concurrent Algorithms

**Assignment**

**Part 1: Implementing a Thread Pool**

1. **Thread Pool Class:** Implement a thread pool class that:
    - Initializes a fixed number of worker threads.
    - Allows tasks to be submitted to a queue.
    - Worker threads continuously pick tasks from the queue and execute them.
    - Supports task synchronization using futures and promises.

**Part 2: Parallelizing a Computational Task**

1. **Computational Task:** Identify a computational task (e.g., matrix multiplication, large-scale sorting, prime number calculation) that can benefit from parallel execution.
2. **Task Division:** Divide the task into smaller sub-tasks that can be executed concurrently.
3. **Using Thread Pool:** Use your thread pool to execute these sub-tasks in parallel. Collect and combine the results of the sub-tasks to obtain the final result.

**Part 3: Asynchronous Tasks with `std::async`**

1. **Async Function:** Write a function that performs a time-consuming operation (e.g., file I/O, network request) asynchronously using `std::async`.
2. **Future and Promise:** Use futures and promises to synchronize and retrieve the result of the asynchronous operation.

**Deliverables**

1. **Source Code:** Submit the complete source code for the thread pool, the parallelized computational task, and the asynchronous function.
2. **Documentation:** Include comments in the code to explain your approach. Additionally, write a brief document (1-2 pages) explaining:
    - The design and implementation of the thread pool.
    - How the computational task was divided and parallelized.
    - The use of futures, promises, and `std::async` in your solution.

**Submission**

Submit your source code and documentation via the course’s assignment submission portal by the end of Week 6.

**Evaluation Criteria**

- **Correctness:** The program should compile and run without errors.
- **Thread Pool Implementation:** Correct and efficient implementation of the thread pool.
- **Parallel Execution:** Effective parallelization of the computational task.
- **Use of Futures, Promises, and `std::async`:** Proper use of these constructs for synchronization and asynchronous operations.
- **Documentation:** Clarity and completeness of the comments and the accompanying document.
- **Code Quality:** Code should be clean, well-organized, and follow modern C++ best practices.

**Solutions**

**Part 1: Implementing a Thread Pool**

**Solution:**

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool {
public:
    ThreadPool(size_t);
    ~ThreadPool();
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

inline ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    std::function<void()> task;
                    
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            }
        );
}

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });
    }
    condition.notify_one();
    return res;
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}
```

**Part 2: Parallelizing a Computational Task**

**Solution:**

```cpp
#include <iostream>
#include <vector>
#include <numeric> // for std::iota

// Function to multiply matrix
std::vector<std::vector<int>> multiplyMatrix(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B) {
    size_t n = A.size();
    std::vector<std::vector<int>> C(n, std::vector<int>(n, 0));
    ThreadPool pool(4); // 4 threads

    std::vector<std::future<void>> futures;
    for(size_t i = 0; i < n; ++i) {
        futures.emplace_back(
            pool.enqueue([&, i] {
                for(size_t j = 0; j < n; ++j) {
                    for(size_t k = 0; k < n; ++k) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            })
        );
    }

    for(auto &fut : futures) {
        fut.get();
    }

    return C;
}

int main() {
    const int n = 3;
    std::vector<std::vector<int>> A(n, std::vector<int>(n));
    std::vector<std::vector<int>> B(n, std::vector<int>(n));

    std::iota(A[0].begin(), A[0].end(), 1); // Fill with sequential values starting from 1
    std::iota(B[0].begin(), B[0].end(), n + 1); // Fill with sequential values starting from 4

    auto C = multiplyMatrix(A, B);

    std::cout << "Resultant Matrix:" << std::endl;
    for (const auto& row : C) {
        for (const auto& elem : row) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
```

**Part 3: Asynchronous Tasks with std::async**

**Solution:**

```cpp
#include <iostream>
#include <future>
#include <chrono>

// Function to simulate a time-consuming operation
int timeConsumingOperation(int x) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return x * x;
}

int main() {
    std::cout << "Starting async operation..." << std::endl;

    // Using std::async to perform operation asynchronously
    std::future<int> result = std::async(std::launch::async, timeConsumingOperation, 5);

    std::cout << "Doing other work while waiting for async operation..." << std::endl;

    // Get the result from the future
    int value = result.get();
    std::cout << "Async operation result: " << value << std::endl;

    return 0;
}
```

**Deliverables**

1. **Source Code:** Submit the complete source code for the thread pool, the parallelized computational task, and the asynchronous function.
2. **Documentation:** Include comments in the code to explain your approach. Additionally, write a brief document (1-2 pages) explaining:
    - The design and implementation of the thread pool.
    - How the computational task was divided and parallelized.
    - The use of futures, promises, and `std::async` in your solution.

**Submission**

Submit your source code and documentation via the course’s assignment submission portal by the end of Week 6.

**Evaluation Criteria**

- **Correctness:** The program should compile and run without errors.
- **Thread Pool Implementation:** Correct and efficient implementation of the thread pool.
- **Parallel Execution:** Effective parallelization of the computational task.
- **Use of Futures, Promises, and `std::async`:** Proper use of these constructs for synchronization and asynchronous operations.
- **Documentation:** Clarity and completeness of the comments and the accompanying document.