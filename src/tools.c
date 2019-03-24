
#include "tools.h"

void random_vector(int *vector, int vector_size, int data_size) {
    
    for (int in = 0, im=0; in < data_size && im < vector_size; ++in) {
        int rn = data_size - in;
        int rm = vector_size - im;
        if (arc4random() % rn < rm)
        /* Take it */
            vector[im++] = in + 1; /* +1 since your range begins from 1 */
    }
    
}

void select_vectors(gsl_matrix * source, gsl_matrix * destination, int * indices) {

    int n = destination->size1;
    for (int i = 0; i < n; i++) {
        gsl_vector_view src = gsl_matrix_row(source, indices[i]);
        gsl_vector_view dest = gsl_matrix_row(destination, i);
        gsl_vector_memcpy(&dest.vector, &src.vector);
    }

}

void select_random_centroids(gsl_matrix * training_set, gsl_matrix * centroids) {

    int n = training_set->size1;
    int k = centroids->size1;
    int indices[k];
    random_vector(indices, k, n);
    select_vectors(training_set, centroids, indices);

}

double calculate_inertia(gsl_matrix * training_set, gsl_matrix * centroids, int * classification) {

    int n = training_set->size1;
    int dim = training_set->size2;
    double inertia = 0.0;
    
    for (int i = 0; i < n; i++) {
        int class = classification[i];
        for (int j = 0; j < dim; j++) {
            double diff = gsl_matrix_get(training_set, i, j) - gsl_matrix_get(centroids, class, j);
            inertia += diff * diff;
        }
    }
    return inertia;

}