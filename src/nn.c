
#include "nn.h"

int nn_mean(gsl_matrix * training_set, int * classification, gsl_matrix * means) {
    
    int dim = training_set->size2;
    int n = training_set->size1;
    int k = means->size1;
    int counts[k];

    for (int i = 0; i < k; i++) {
        counts[i] = 0;
    }

    for (int i = 0; i < n; i++) {
        int class = classification[i];
        gsl_vector_view x = gsl_matrix_row(training_set, i);
        gsl_vector_view mean = gsl_matrix_row(means, class);
        gsl_vector_add(&mean.vector, &x.vector);
        counts[class]++;
    }

    for (int i = 0; i < k; i++) {
        if (counts[i] > 0) {
            gsl_vector_view mean = gsl_matrix_row(means, i);
            double scale = 1.0/((double)counts[i]);
            gsl_vector_scale(&mean.vector, scale);
        }
    }
    return 1;
    
}

int nn_classify(gsl_matrix * x, gsl_matrix * means, int * classification, int * has_changed) {

    int dim = x->size2;
    int n = x->size1;
    int k = means->size1;
    double distance, diff;
    double min_d = 0;
    int arg_min_d = 0;
    int change_flag = FALSE;

    for (int i = 0; i < n; i++) {
        gsl_vector_view xi = gsl_matrix_row(x, i);
        min_d = DBL_MAX;
        for (int j = 0; j < k; j++) {
            gsl_vector_view mu = gsl_matrix_row(means, j);
            distance = 0.0;
            for (int m = 0; m < dim; m++) {
                diff= (gsl_vector_get(&xi.vector, m) - gsl_vector_get(&mu.vector, m));
                distance += diff * diff;
            }
            if (distance < min_d) {
                min_d = distance;
                arg_min_d = j;
            }
        }
        if (classification[i] != arg_min_d) {
            classification[i] = arg_min_d;
            change_flag = TRUE;
        }
    }

    if (has_changed != NULL) {
        (* has_changed) = change_flag;
    }
    return 1;

}


int nn_learn(gsl_matrix * training_set, gsl_matrix * means, int * classification) {

    int dim = training_set->size2;
    int n = training_set->size1;
    int k = means->size1;
    int has_changed = TRUE;

    while (TRUE) {
        nn_classify(training_set, means, classification, &has_changed);
        if (has_changed) {
            nn_mean(training_set, classification, means);
        } else {
            break;
        }
    }
    return 1;

}