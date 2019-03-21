//
//  test_em.c
//  k-means
//
//  Created by Jiří Lidinský on 18/03/2019.
//  Copyright © 2019 Jiří Lidinský. All rights reserved.
//

#include "em.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_block.h>


typedef int (*test_fce)(void);

double train_data[] = {
    1.0, 7.6, 4.3, 23.9, 12.7, 43.0, 4.3, 1.2,  0.0, 87.5,
    6.9, 4.8, 5.9, 57.9, 35.1, 80.5, 1.2, 4.9, 12.0, 34.1
};

double mean[] = {
    124.5/3.0, 13.1/3.0, 10.4/3.0, 35.9/3.0, 134.3/3.0,
    6.9, 4.8, 5.9, 57.9, 35.1
};

double train_data_2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 76, 54, 4, 8, 32, 2, 1, 56, 8, 90};
double covariance_2[] = {546.044, 184.667, 184.667, 964.444};
double mean_2[] = {14.600, 24.000};
int dim2 = 2;
int n2 = 10;
int k2 = 1;
int class2[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int class[] = {0, 0, 1, 0};

int k = 2;
int dim = 5;
int n = 4;


int assert_equal_array_double(double * array, double * expected, int dim, double epsilon) {
    
    for (int i = 0; i < dim; i++) {
        double diff = array[i] - expected[i];
        if (diff > epsilon || diff < -epsilon) {
            for (int j=0; j<dim; j++) printf("%g, ", array[j]);
            printf("\n");
            for (int j=0; j<dim; j++) printf("%g, ", expected[j]);
            printf("\n");
            return 0;
        }
    }
    return 1;
    
}

int test_em_mean() {
    
    printf("Mean value calculation test\n");
    gsl_matrix_view training_set = gsl_matrix_view_array(train_data, n, dim);
    gsl_matrix * means;
    means = gsl_matrix_calloc(k, dim);
    em_mean(&training_set.matrix, class, means);
    return assert_equal_array_double(mean, means->data, k*dim, 1e-6);
    
}

int test_em_covariance() {

    printf("Covariance matrix calculation test\n");
    gsl_matrix_view training_set = gsl_matrix_view_array(train_data_2, n2, dim2);
    gsl_matrix_view means = gsl_matrix_view_array(mean_2, k2, dim2);
    gsl_block * covariance = gsl_block_calloc(k2 * dim2 * dim2);
    em_covariance(&training_set.matrix, class2, &means.matrix, covariance);
    //gsl_block_fprintf(stdout, covariance, "%g");

    return assert_equal_array_double(covariance_2, covariance->data, k2*dim2*dim2, 1e-3);

}

test_fce tests[] = {
    &test_em_mean,
    &test_em_covariance
};

int main (int argc, char **argv) {
    
    for (int i = 0; i < 2; i++) {
        printf("============== test %d ==============\n", i);
        if ( tests[i]() ) {
            printf("passed\n");
        } else {
            printf("FAILED!\n");
        }
    }
 
    return 0;
    
}
