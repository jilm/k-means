

#include <inttypes.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include "io.h"
#include "tools.h"
#include "nn.h"

#define LEARN_MODE_EM 1
#define LEARN_MODE_NN 0


gsl_matrix * ata(gsl_matrix * a) {

    int size_x = a->size2;
    gsl_matrix * result = gsl_matrix_calloc(size_x, size_x);
    for (int i = 0; i < size_x; i++) {
        for (int j = 0; j < size_x; j++) {
            for (int k = 0; k < a->size1; k++) {
                gsl_matrix_set(result, i, j, gsl_matrix_get(result, i, j) + gsl_matrix_get(a, k, i) * gsl_matrix_get(a, k, j));
            }
        }
    }
    return result;

}

int init_k = 0;
int learn_mode = 0;

void parse_arguments(int argc, char **argv) {

    int i = 2;
    while (i < argc) {
        if (strcmp(argv[i++], "-k") == 0) {
            if (i < argc) {
                init_k = (int) strtol(argv[i], NULL, 10);
            }            
        } else if (strcmp(argv[i], "-em") == 0) {
            learn_mode = LEARN_MODE_EM;
        } else if (strcmp(argv[i], "-nn") == 0) {
            learn_mode = LEARN_MODE_NN;
        }
        i++;
    }

}

int main (int argc, char **argv) {

    parse_arguments(argc, argv);
    openlog(argv[0], LOG_PERROR | LOG_NDELAY, 0);

    if (argc < 2) {
        
        fprintf(stderr, "The command missing: PCA, learn, analyze, classify\n");
        return 1;

    } else if (strcmp(argv[1], "PCA") == 0) {

        double * training_set;
        int dimension = 0;
        int n = 0;
        read_matrix(stdin, &training_set, &dimension, &n);
        gsl_matrix_view training_data = gsl_matrix_view_array(training_set, n, dimension);
        gsl_matrix * x = ata(&training_data.matrix);
        gsl_matrix * v = gsl_matrix_calloc(dimension, dimension);
        gsl_vector * s = gsl_vector_calloc(dimension);
        gsl_vector * work = gsl_vector_calloc(dimension);
        gsl_linalg_SV_decomp(x, v, s, work);
        fprintf(stdout, "set grid\n");
        fprintf(stdout, "set logscale y\n");
        fprintf(stdout, "plot '-' with lines\n");
        gsl_vector_fprintf(stdout, s, "%g");
        fprintf(stdout, "e\n");
        return 0;

    } else if (strcmp(argv[1], "learn") == 0) {

        // Load the training set
        double * training_set;
        int dimension = 0;
        int n = 0;
        read_matrix(stdin, &training_set, &dimension, &n);
        gsl_matrix_view training_data = gsl_matrix_view_array(training_set, n, dimension);
        DEBUG("Training data have been loaded.");
        // Initialize
        int k = init_k;
        gsl_matrix * centroids = gsl_matrix_alloc(k, dimension);
        select_random_centroids(&training_data.matrix, centroids);
        int * classification = (int*)malloc(n * sizeof(int));
        // Learn
        DEBUG("Going to learn!");
        nn_learn(&training_data.matrix, centroids, classification);
        DEBUG("Learning is finished.");
        gsl_matrix_fprintf(stdout, centroids, "%g");
        // Cleen-up
        gsl_matrix_free(centroids);
        free(classification);
        return 0;

    } else if (strcmp(argv[1], "analyze") == 0) {

        // Load the training set
        double * training_data;
        int dimension = 0;
        int n = 0;
        read_matrix(stdin, &training_data, &dimension, &n);
        gsl_matrix_view training_set = gsl_matrix_view_array(training_data, n, dimension);
        DEBUG("Training data have been loaded.");
        // Initialize
        for (int k = 2; k < n / 4; k++) {
            gsl_matrix * centroids = gsl_matrix_alloc(k, dimension);
            select_random_centroids(&training_set.matrix, centroids);
            int * classification = (int*)malloc(n * sizeof(int));
            // Learn
            DEBUG("Going to learn!");
            nn_learn(&training_set.matrix, centroids, classification);
            DEBUG("Learning is finished.");
            printf("%d %g\n", k, calculate_inertia(&training_set.matrix, centroids, classification));
            // Cleen-up
            gsl_matrix_free(centroids);
            free(classification);
        }
        return 0;

    } else if (strcmp(argv[1], "classify") == 0) {


    } else {

        fprintf(stderr, "Unknown command!\n");
        return 1;

    }

}

