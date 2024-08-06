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

PMatrix MatrixMultiply(PMatrix pmatrix1, PMatrix pmatrix2){
    // start threads to handle vector multiplication and then combine into a final matrix
    // probably need a mutex to lock the final matrix and make adjustments to it? Although
    // the indices that are changed will be different for every thread
    int n = (*pmatrix1).size();
    PMatrix new_matrix = new std::vector<std::vector<int>>;
    new_matrix->resize(n,std::vector<int>(n,0));

    TransposeMatrix(pmatrix2);

    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            (*new_matrix)[i][j] = VectorMultiply((*pmatrix1)[i], (*pmatrix2)[j]);
        }
    }

    return new_matrix;
}

int Ex2(){

    PMatrix pmatrix1 = BuildMatrix(3);
    PMatrix pmatrix2 = BuildMatrix(3);

    // print original matrices
    PrintMatrix(pmatrix1);
    std::cout << std::endl;
    PrintMatrix(pmatrix2);
    std::cout << std::endl;
    
    PMatrix pfinal_matrix = MatrixMultiply(pmatrix1, pmatrix2);
    // print final matrix
    PrintMatrix(pfinal_matrix);

    return 0;
}
