#include<omp.h>

// y = alpha * x + y
void axpy(size_t N, double alpha, const double* x, double* y){ 
    #pragma omp parallel for
    for(size_t i=0; i<N; i++){
    }
}

// z = alpha * x + y
void axpyz(size_t N, double alpha, const double* x, const double* y, double* z){ 
}

// x = x + alpha * y
void xpay(size_t N, double alpha, double* x, const double* y){ 
}

// x = alpha * x
double scal(size_t N, double* x, double alpha){ 
}

// inner product
double dot(size_t N, const double* x, const double* y){ 
    double ans;
    return ans;
}

// 2nrm
double nrm2(size_t N, const double* x, const double* y){ 
    double ans;
    return ans;
}

