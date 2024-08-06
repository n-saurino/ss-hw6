#include "ThreadPool.h"
#include <iostream>
#include <random>

/*
Part 2: Parallelizing a Computational Task

1. Computational Task: Identify a computational task 
(e.g., matrix multiplication, large-scale sorting, prime number calculation) 
that can benefit from parallel execution.
2. Task Division: Divide the task into smaller sub-tasks that can be executed concurrently.
3. Using Thread Pool: Use your thread pool to execute these sub-tasks in parallel. 
Collect and combine the results of the sub-tasks to obtain the final result.
*/

typedef std::vector<std::vector<int>>* PMatrix;
typedef std::vector<std::vector<std::future<int>>> FutureMatrix;

PMatrix BuildMatrix(int n){
    PMatrix new_matrix = new std::vector<std::vector<int>>;
    new_matrix->resize(n,std::vector<int>(n,0));

    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            (*new_matrix)[i][j] = rand() % 10;
        }
    }

    return new_matrix;
}

void PrintMatrix(PMatrix matrix){
    int n = (*matrix).size();
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            std::cout << (*matrix)[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void TransposeMatrix(PMatrix matrix){
    int n = (*matrix).size();
    for(int i = 0; i < n; ++i){
        for(int j = i; j < n; ++j){
            if(i == j){
                continue;
            }

            int temp = (*matrix)[i][j];
            (*matrix)[i][j] = (*matrix)[j][i];
            (*matrix)[j][i] = temp;
        }
    }
}

int VectorMultiply(const std::vector<int>& vec1, const std::vector<int>& vec2){
    int n = vec1.size();
    int result = 0;
    for(int i = 0; i < n; ++i){
        result += vec1[i] * vec2[i];
    }

    return result;
}

PMatrix MatrixMultiply(const PMatrix pmatrix1, const PMatrix pmatrix2){
    // start threads to handle vector multiplication and then combine into a final matrix
    // probably need a mutex to lock the final matrix and make adjustments to it? Although
    // the indices that are changed will be different for every thread

    int n = (*pmatrix1).size();
    PMatrix new_matrix = new std::vector<std::vector<int>>;
    new_matrix->resize(n,std::vector<int>(n,0));

    TransposeMatrix(pmatrix2);

     auto start = std::chrono::system_clock::now();

    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            (*new_matrix)[i][j] = VectorMultiply((*pmatrix1)[i], (*pmatrix2)[j]);
        }
    }

    // std::this_thread::sleep_for(std::chrono::nanoseconds(10));

    auto end = std::chrono::system_clock::now();
    // this constructs a duration object using milliseconds
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Single thread solution: " << elapsed.count() << " milliseconds\n\n";

    // retransposing matrix to return it to it's original state
    TransposeMatrix(pmatrix2);

    return new_matrix;
}

PMatrix ThreadedMatrixMultiply(const PMatrix pmatrix1, const PMatrix pmatrix2){
    // start threads to handle vector multiplication and then combine into a final matrix
    // probably need a mutex to lock the final matrix and make adjustments to it? Although
    // the indices that are changed will be different for every thread

    int n = (*pmatrix1).size();
    PMatrix new_matrix = new std::vector<std::vector<int>>;
    new_matrix->resize(n,std::vector<int>(n,0));

    TransposeMatrix(pmatrix2);
    ThreadPool* thread_pool = new ThreadPool;
    std::vector<std::future<void>> futures;

     auto start = std::chrono::system_clock::now();

    for (int i = 0; i < n; ++i) {
        futures.push_back(thread_pool->QueueJob([i, pmatrix1, pmatrix2, new_matrix] {
            for (int j = 0; j < new_matrix->size(); ++j) {
                (*new_matrix)[i][j] = VectorMultiply((*pmatrix1)[i], (*pmatrix2)[j]);
            }
        }));
    }
    
    for(auto& future : futures){
        future.get();
    }

    auto end = std::chrono::system_clock::now();
    
    // this constructs a duration object using milliseconds
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "ThreadPool solution: " << elapsed.count() << " milliseconds\n\n";

    // retransposing matrix to return it to its original state
    TransposeMatrix(pmatrix2);
    delete thread_pool;
    return new_matrix;
}

int Ex2(){

    PMatrix pmatrix1 = BuildMatrix(1000);
    PMatrix pmatrix2 = BuildMatrix(1000);

    // print original matrices
    /*
    PrintMatrix(pmatrix1);
    std::cout << std::endl;
    PrintMatrix(pmatrix2);
    std::cout << std::endl;
    */

    PMatrix psingle_matrix = MatrixMultiply(pmatrix1, pmatrix2);
    // print final matrix
    // PrintMatrix(psingle_matrix);

    PMatrix pthreaded_matrix = ThreadedMatrixMultiply(pmatrix1, pmatrix2);
    // print final matrix
    // PrintMatrix(pthreaded_matrix);

    delete pmatrix1;
    delete pmatrix2;
    delete psingle_matrix;
    delete pthreaded_matrix;
    
    return 0;
}
