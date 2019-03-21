/*
 *   copyright 2019 Jiri Lidinsky
 *    
 *   This file is part of k-means.
 *
 *   Foobar is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Foobar is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "em.h"

int em_mean(gsl_matrix * training_set, int * classification, gsl_matrix * means) {
    
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

int em_covariance(gsl_matrix * training_set, int * classification, gsl_matrix * means, gsl_block * covariance) {

    int dim = training_set->size2;
    int n = training_set->size1;
    int k = means->size1;
    int counts[k];
    
    for (int i = 0; i < k; i++) {
        counts[i] = 0;
    }
    
    for (int i = 0; i < n; i++) {
        int class = classification[i];
        int base = class * dim * dim;
        gsl_vector_view x = gsl_matrix_row(training_set, i);
        gsl_vector_view mean = gsl_matrix_row(means, class);
        for (int j = 0; j < dim; j++) {
            for (int m = 0; m < dim; m++) {
                covariance->data[base + j * dim + m]
                    += (gsl_vector_get(&(x.vector), j) - gsl_vector_get(&(mean.vector), j))
                    * (gsl_vector_get(&(x.vector), m) - gsl_vector_get(&(mean.vector), m));
            }
        }
        counts[class]++;
    }

    for (int i = 0; i < k; i++) {
        int class = classification[i];
        if (counts[class] > 1) {
            int base = class * dim * dim;
            for (int j = 0; j < dim * dim; j++) {
                covariance->data[base + j] /= (double) (counts[class] - 1);
            }
        }
    }
    return 1;
}

int em_classify(gsl_matrix * x, gsl_matrix * means, gsl_block * covariance, int * classification) {

    int dim = x->size2;
    int n = x->size1;
    int k = means->size1;
    double p;
    gsl_vector * work = gsl_vector_calloc(k);
    double max_p = 0;
    int arg_max_p = 0;
    int change_flag = FALSE;

    for (int i = 0; i < n; i++) {
        gsl_vector_view xi = gsl_matrix_row(x, i);
        max_p = 0;
        for (int j = 0; j < k; j++) {
            gsl_vector_view mu = gsl_matrix_row(means, j);
            gsl_matrix_view L = gsl_matrix_view_array(&(covariance->data[j * dim * dim]), dim, dim);
            gsl_ran_multivariate_gaussian_pdf(&xi.vector, &mu.vector, &L.matrix, &p, work);
            if (p > max_p) {
                max_p = p;
                arg_max_p = j;
            }
        }
        if (classification[i] != arg_max_p) {
            classification[i] = arg_max_p;
            change_flag = TRUE;
        }
    }

    gsl_vector_free(work);
    return 1;

}
