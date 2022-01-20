#include<omp.h>
#include<math.h>

// y = alpha * x + y
void axpy(size_t N, double alpha, const double* x, double* y){ 
    #pragma omp parallel for
    for(size_t i=0; i<N; i++){
        y[i] = alpha * x[i] + y[i];
    }
}

// z = alpha * x + y
void axpyz(size_t N, double alpha, const double* x, const double* y, double* z){ 
    #pragma omp parallel for
    for(size_t i=0; i<N; i++){
        z[i] = alpha * x[i] + y[i];
    }
}

// x = x + alpha * y
void xpay(size_t N, double alpha, double* x, const double* y){ 
    #pragma omp parallel for
    for(size_t i=0; i<N; i++){
        x[i] = x[i] + alpha * y[i];
    }
}

// x = alpha * x
void scal(size_t N, double alpha, double* x){ 
    #pragma omp parallel for
    for(size_t i=0; i<N; i++){
        x[i] = alpha * x[i];
    }
}

// inner product
double dot(size_t N, const double* x, const double* y){ 
    double ans;

    #pragma omp parallel for
    for(size_t i=0; i<N; i++){
        ans += x[i] * y[i];
    }

    return ans;
}

// 2nrm
double nrm2(size_t N, const double* x){ 
    return sqrt(dot(N, x, x));
}

// copy (y=x)
void copy(size_t N, const double* x, double* y){ 
    #pragma omp parallel for
    for(size_t i=0; i<N; i++){
        y[i] = x[i];
    }
}

// matvec:y=Ax
void matvec(int N, 
        const int* row_ptr, const int* col_ind, const double* val, 
        const double* x, double* y)
{
#pragma omp parallel for
    for(size_t i = 0; i < N; i++){
        y[i] = 0;
        for(size_t j = row_ptr[i]; j < row_ptr[i+1]; j++){
            y[i] += x[col_ind[j]] * val[j];
        }
    }
}
