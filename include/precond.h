void create_precond_jacobi(int N, 
        const int* row_ptr, const int* col_ind, const double* val, 
        double* M)
{
#pragma omp parallel for
    for(size_t i = 0; i < N; i++){
        for(size_t j = row_ptr[i]; j < row_ptr[i+1]; j++){
            if(i==col_ind[j]){
                M[i] = 1 / val[j];
            }
        }
    }
}

void apply_precond_jacobi(int N, const double* M, 
        const double* r, double* z)
{
#pragma omp parallel for
    for(size_t i = 0; i < N; i++){
        z[i] = r[i] * M[i];
    }
}
