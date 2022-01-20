#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include "./SpUtil.h"
#include "./myblas.h"

struct CRS_matrix{
    int N;
    int nnz;
    int* row_ptr;
    int* col_ind;
    double* val;
};

int main(int argc, char **argv){

    // e.g. ./a.out data/test.mtx
    if(argc!=2){
        printf("error! $1 is matrix market file name\n");
        abort();
    }

////////////////////
// Create CRS format matrix from file
///////////////////
    int N, nnz;
    //file open and read header to get matrix size
    FILE* fp = fopen(argv[1], "r");
    SpUtil_read_mm_header(fp, &N, &nnz);
    //printf("N = %d, nnz = %d\n", N, nnz);
    
    // allocate COO array
    int* coo_row_index = (int*)malloc(sizeof(int)*nnz);
    int* coo_col_index = (int*)malloc(sizeof(int)*nnz);
    double* coo_val = (double*)malloc(sizeof(double)*nnz);

    // create COO from file
    SpUtil_mm2coo(fp, N, nnz, coo_row_index, coo_col_index, coo_val);

    // close
    fclose(fp);

    // allocate CRS array
    struct CRS_matrix A;
    A.row_ptr = (int*)malloc(sizeof(int)*(N+1));
    A.col_ind = (int*)malloc(sizeof(int)*nnz);
    A.val = (double*)malloc(sizeof(double)*nnz);
    A.N = N;
    A.nnz = nnz;
    
    // crate CRS from COO
    SpUtil_coo2crs(N,nnz,
                coo_row_index, coo_col_index, coo_val,
                A.row_ptr, A.col_ind, A.val);

    // debug
    SpUtil_print_crs(N, nnz, A.row_ptr, A.col_ind, A.val);

////////////////////
// BiCGSTAB
///////////////////
    size_t maxiter = A.N
    for(size_t iter = 0; iter < maxiter; iter++){
        //impl..//

    }
    return 0;
}
