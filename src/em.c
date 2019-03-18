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
