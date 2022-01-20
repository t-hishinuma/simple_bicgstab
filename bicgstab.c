#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include "include/SpUtil.h"
#include "include/myblas.h"
#include "include/precond.h"

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
    // Create CRS format matrix from Matrixmarket format file
    // See https://zenn.dev/hishinuma_t/books/sparse-matrix-and-vector-product
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
    // SpUtil_print_crs(A.N, A.nnz, A.row_ptr, A.col_ind, A.val);

    free(coo_row_index);
    free(coo_col_index);
    free(coo_val);

    ////////////////////
    // Initialize BiCGSTAB vectors
    // BiCGSTAB
    ///////////////////

    //change me
    size_t maxiter = A.N*100;
    double tol = 1.0e-12;
    double* b = (double*)malloc(sizeof(double)*(A.N));
    double* x = (double*)malloc(sizeof(double)*(A.N));

    //change me
#pragma omp parallel for
    for(size_t i = 0; i < A.N; i++){
        x[i] = 0.0; //initial x
        b[i] = 1.0; //initial b
    }

    double* r = (double*)malloc(sizeof(double)*(A.N));
    double* r0 = (double*)malloc(sizeof(double)*(A.N));

    double* p = (double*)malloc(sizeof(double)*(A.N));
    double* phat = (double*)malloc(sizeof(double)*(A.N));

    double* s = (double*)malloc(sizeof(double)*(A.N));
    double* shat = (double*)malloc(sizeof(double)*(A.N));

    double* v = (double*)malloc(sizeof(double)*(A.N));
    double* t = (double*)malloc(sizeof(double)*(A.N));

    double* M = (double*)malloc(sizeof(double)*(A.N)); //for precond

    ////////////////////
    // BiCGSTAB
    ///////////////////
    double rho_old = 1.0, rho = 1.0, alpha = 1.0, beta, omega = 1.0;

    create_precond_jacobi(A.N, A.row_ptr, A.col_ind, A.val, M);

    // r = b-Ax
    matvec(A.N, A.row_ptr, A.col_ind, A.val, x, r);
#pragma omp parallel for
    for(size_t i = 0; i < A.N; i++){
        r[i] = b[i] - r[i];
    }

    // r0 = r, (r*0, r0)!=0
    copy(A.N, r, r0);

    for (size_t iter = 0; iter < maxiter; iter++) {

        // alpha = (r(i-1), r0) / (AM^-1*p(i-1), r0)
        rho = dot(A.N, r, r0);

        if (rho == 0.0) {
            printf("%ld: %e\n", iter+1, rho);
            printf("success\n");
            free(A.row_ptr); free(A.col_ind); free(A.val); free(M);
            free(r); free(r0); free(v); free(t);
            free(p); free(phat); free(s); free(shat);
            free(x); free(b);
            return 0;
        }

        if (iter == 0) {
            copy(A.N, r, p);
        } else {
            // beta = (rho / rho_old) * (alpha / omega)
            beta = (rho / rho_old) * (alpha / omega);

            // p = r + beta(p + omega * AM-1 p(i-1) )
            axpy(A.N, -omega, v, p); // p = -omega*v + p
            xpay(A.N, beta, r, p);   // p = r + beta*p
        }

        // phat = M^-1 p(i-1)
        apply_precond_jacobi(A.N, M, p, phat);
        // v = AM^-1p(i-1)
        matvec(A.N, A.row_ptr, A.col_ind, A.val, phat, v);
        alpha = rho / dot(A.N, v, r0);

        // s(i) = r(i-1) - alpha v
        axpyz(A.N, -alpha, v, r, s);

        // shat = M^-1 s(i)
        apply_precond_jacobi(A.N, M, s, shat);
        // t = A * shat
        matvec(A.N, A.row_ptr, A.col_ind, A.val, shat, t);

        // omega = (AM-1s, s) / (AM-1s, AM-1s)
        omega = dot(A.N, t, s) / dot(A.N, t, t);

        if (omega == 0.0) {
            printf("fail (div zero breakdown)\n");
            free(A.row_ptr); free(A.col_ind); free(A.val); free(M);
            free(r); free(r0); free(v); free(t);
            free(p); free(phat); free(s); free(shat);
            free(x); free(b);
            return -1;
        }

        // x(i) = x(i-1) + alpha * M^-1 p(i-1) + omega * M^-1 s(i)
        axpy(A.N, alpha, phat, x);
        axpy(A.N, omega, shat, x);

        // r(i) = s(i-1) - omega * AM^-1 s(i-1)
        axpyz(A.N, -omega, t, s, r);

        // convergence check
        double resid = nrm2(A.N, r);
        printf("%ld: %e\n", iter+1, resid);
        if (resid < tol) {
            printf("success\n");
            free(A.row_ptr); free(A.col_ind); free(A.val); free(M);
            free(r); free(r0); free(v); free(t);
            free(p); free(phat); free(s); free(shat);
            free(x); free(b);
            return 0;
        }

        rho_old = rho;
    }

    printf("fail (maxiter)\n");
    free(A.row_ptr); free(A.col_ind); free(A.val); free(M);
    free(r); free(r0); free(v); free(t);
    free(p); free(phat); free(s); free(shat);
    free(x); free(b);
    return -2;
}
