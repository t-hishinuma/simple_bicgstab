/* Copyright (C) 2021 T.Hishinuma, All rights reserved.*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MM_BANNER "%%MatrixMarket"
#define MM_FMT "coordinate"
#define MM_TYPE_REAL "real"
#define MM_TYPE_GENERAL "general"

void SpUtil_read_mm_header(FILE* file, int* N, int* nnz) {
    char buf[256], banner[128];
    char mtx[64], fmt[64], dtype[64], dstruct[64];
    char* p;
    int row, col, nz;

    if (file == NULL) {
        printf("SpUtil Error: can not open file\n");
        abort();
    }
    if (fgets(buf, 256, file) == NULL) {
        printf("SpUtil Error: blank file\n");
        abort();
    }

    // check banner
    rewind(file);
    fgets(buf, 256, file);
    sscanf(buf, "%s %s %s %s %s", banner, mtx, fmt, dtype, dstruct);

    for (p = mtx; *p != '\0'; p++) *p = (char)tolower(*p);
    for (p = fmt; *p != '\0'; p++) *p = (char)tolower(*p);
    for (p = dtype; *p != '\0'; p++) *p = (char)tolower(*p);
    for (p = dstruct; *p != '\0'; p++) *p = (char)tolower(*p);

    if (strncmp(banner, MM_BANNER, strlen(MM_BANNER)) != 0) {
        printf("SpUtil Error: Not Matrix Market banner, banner is %s\n",
               banner);
        abort();
    }
    if (strncmp(fmt, MM_FMT, strlen(MM_FMT)) != 0) {
        printf("SpUtil Error: Not Coodinate format\n");
        abort();
    }
    if (strncmp(dtype, MM_TYPE_REAL, strlen(MM_TYPE_REAL)) != 0) {
        printf("SpUtil Error: Not real\n");
        abort();
    }
    if (strncmp(dstruct, MM_TYPE_GENERAL, strlen(MM_TYPE_GENERAL)) != 0) {
        printf("SpUtil Error: Not general\n");
        abort();
    }
    do {
        if (fgets(buf, 256, file) == NULL) {
            printf("SpUtil Error: check size error\n");
            abort();
        }
    } while (buf[0] == '%');

    // check size
    if (sscanf(buf, "%d %d %d", &col, &row, &nz) != 3) {
        printf("SpUtil Error: matrix size in unknown\n");
        abort();
        if (col != row) {
            printf("SpUtil Error: matrix is not square matrix (row!=col)\n");
            abort();
        }
    }
    *N = row;
    *nnz = nz;
}

void SpUtil_mm2coo(FILE* file, int N, int nnz, int* row_index, int* col_index,
                   double* val) {
    char buf[256];
    int i;
    int idx, jdx;
    double value;
    // rewind(file);

    /* read data */

    for (i = 0; i < nnz; i++) {
        if (fgets(buf, 256, file) == NULL) {
            printf("SpUtil Error: cant read data, [row col value]\n");
            abort();
        }
        if (sscanf(buf, "%d %d %lf", &idx, &jdx, &value) != 3) {
            printf("SpUtil Error: not data, [col=%d,row=%d,val=%f]\n", idx, jdx,
                   value);
            abort();
        }
        row_index[i] = idx - 1;
        col_index[i] = jdx - 1;
        val[i] = value;
    }
}

void SpUtil_print_coo(int N, int nnz, int* row_index, int* col_index,
                      double* val) {
    for (int i = 0; i < nnz; i++) {
        printf("%d\t", row_index[i]);
        printf("%d\t", col_index[i]);
        printf("%e\n", val[i]);
    }
}

void SpUtil_coo2crs(int N, int nnz, const int* coo_row, const int* coo_col,
                    const double* coo_val, int* row_ptr, int* col_ind,
                    double* val) {
    for (int i = 0; i < N + 1; i++) {
        row_ptr[i] = 0;
    }

    for (int i = 0; i < nnz; i++) {
        val[i] = coo_val[i];
        col_ind[i] = coo_col[i];
        row_ptr[coo_row[i] + 1]++;
    }

    for (int i = 0; i < N + 1; i++) {
        row_ptr[i + 1] += row_ptr[i];
    }
}

void SpUtil_print_crs(int N, int nnz, int* row_ptr, int* col_ind, double* val) {
    for (int i = 0; i < N; i++) {
        for (int j = row_ptr[i]; j < row_ptr[i + 1]; j++) {
            printf("%d\t", i);
            printf("%d\t", col_ind[j]);
            printf("%e\n", val[j]);
        }
    }
}
