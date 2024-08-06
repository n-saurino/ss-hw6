#include "ThreadPool.h"
#include <iostream>
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
            (*new_matrix)[i][j] = i + j;
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

int MatrixMultiply(PMatrix pmatrix1, PMatrix pmatrix2){
    return 0;
}

PMatrix TransposeMatrix(PMatrix matrix){
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

    return matrix;
}

int VectorMultiply(){
    return 0;
}


int Ex2(){

    PMatrix pmatrix1 = BuildMatrix(2);
    PMatrix pmatrix2 = BuildMatrix(2);
    
    (*pmatrix1)[0][1] = 3;

    PrintMatrix(pmatrix1);

    TransposeMatrix(pmatrix1);

    PrintMatrix(pmatrix1);

    return 0;
}
