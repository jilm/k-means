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

#include "cluster.h"

int evaluate_vector(vector v, matrix centroids, double *min_distance) {
    
    double distance;
    (*min_distance) = DBL_MAX;
    int arg_min = 0;
    for (int i = 0; i < centroids.n; i++) {
        distance = 0.0;
        for (int j = 0; j < v.size; j++) {
            float diff = v.buffer[j]
                - centroids.buffer[i * centroids.dimension + j];
            distance += diff * diff;
        }
        if (distance < (*min_distance)) {
            (*min_distance) = distance;
            arg_min = i;
        }
    }
    return arg_min;
    
}

int evaluate_training_set(matrix training_set, matrix centroids, int *evaluation, double *inertia) {
    
    int change_flag = FALSE;
    (*inertia) = 0.0;
    for (int i=0; i<training_set.n; i++) {
        vector v = {&(training_set.buffer[i*training_set.dimension]), training_set.dimension};
        double distance = 0.0;
        int eval = evaluate_vector(v, centroids, &distance);
        (*inertia) += distance;
        if (eval != evaluation[i]) change_flag = TRUE;
        evaluation[i] = eval;
    }
    return change_flag;
    
}

void update_centroids(matrix trainig_set, matrix centroids, int *evaluation) {
    
    float lengths[centroids.n];
    for (int i=0; i < centroids.n; i++) lengths[i] = 0.0f;
    RESET_MATRIX(centroids)

    for (int j=0; j<trainig_set.n; j++) {
        for (int k=0; k<trainig_set.dimension; k++) {
            centroids.buffer[evaluation[j] * centroids.dimension + k]
                += trainig_set.buffer[j * trainig_set.dimension + k];
        }
        lengths[evaluation[j]] += 1.0f;
    }
    
    for (int i=0; i<centroids.n; i++) {
        if (lengths[i] > 0) {
            for (int j=0; j<centroids.dimension; j++) {
                centroids.buffer[i * centroids.dimension + j] /= lengths[i];
            }
        }
    }
    
}

void random_vector(int *vector, int vector_size, int data_size) {
    
    for (int in = 0, im=0; in < data_size && im < vector_size; ++in) {
        int rn = data_size - in;
        int rm = vector_size - im;
        if (arc4random() % rn < rm)
        /* Take it */
            vector[im++] = in + 1; /* +1 since your range begins from 1 */
    }
    
}

void k_train(matrix training_set, matrix centroids, double *inertia) {
    
    int evaluation[training_set.n];
    (*inertia) = 0.0;
    for (int i=0; i<training_set.n; i++) evaluation[i] = 0;
    syslog(LOG_DEBUG, "Entering eval-update learning loop...");
    while (evaluate_training_set(training_set, centroids, evaluation, inertia)) {
        printf("%g\n", *inertia);
        (*inertia) = 0.0;
        fprintf(stderr, ".");
        update_centroids(training_set, centroids, evaluation);
        fprintf(stderr, ".");
    }
    
}

double k_means_train(matrix training_set, int k) {
    
    matrix centroids;
    centroids.dimension = training_set.dimension;
    centroids.n = k;
    centroids.size = k * centroids.dimension;
    centroids.buffer = (float *)malloc(centroids.size * sizeof(float));
    
    // select initial cluster center
    int *centroid_indices = (int*)malloc(k * sizeof(int));
    random_vector(centroid_indices, k, training_set.n);
    for (int i=0; i<k; i++) {
        for (int j=0; j<centroids.dimension; j++) {
            centroids.buffer[i * centroids.dimension + j] = training_set.buffer[centroid_indices[i] * centroids.dimension + j];
        }
    }
    double inertia;
    k_train(training_set, centroids, &inertia);
    free(centroid_indices);
    free(centroids.buffer);
    return inertia;
}


void write_vectors(FILE *file, float *buffer, int dimension, int n) {
    
    for (int v=0; v<n; v++) {
        for (int i=0; i<dimension; i++) {
            printf("%g ", buffer[v * dimension + i]);
        }
        fprintf(file, "\n");
    }
    
}

void print_matrix(FILE *file, matrix m) {
    
    for (int i=0; i<m.n; i++) {
        for (int j=0; j<m.dimension; j++) {
            fprintf(file, "%g ", m.buffer[i*m.dimension+j]);
        }
    }
    
}


/*
 * Read one line of text from a given file into the buffer.
 * If the buffer argument contains NULL, the buffer is allocated inside
 * this function.
 *
 * The buffer may be given. In such a case, if it is too small, it is
 * realocated inside this function.
 *
 * Params:
 *      file: a file to read from
 *      buffer: a buffer to return result, the memory is allocated inside
 *          this function, it is up to calling function to release it.
 *      length: returns the number of valid characters inside the buffer
 *          which may be begger. If greater than zero,
 *      dimension: the number of numbers inside the buffer.
 *
 * Returns number of valid characters inside the buffer. Zero is returned
 * in case, the end of file is reached.
 */
int read_vector(FILE *file, char **buffer, int *size, int *dimension) {
    
    int length = 0;
    int delimiter_flag = TRUE;
    int vector_delimiter_flag = TRUE;
    (*dimension) = 0;
    while (TRUE) {
        
        // read a caracter from input file
        char c = (char)fgetc(file);
        
        if (c == EOF && length == 0) {
            return 0;
        } else if (c == EOF) {
            break;
        } else if ((c == CR || c == LF) && !vector_delimiter_flag) {
            (*dimension)++;
            vector_delimiter_flag = TRUE;
            break;
        } else if (c == CR || c == LF) {
            // do nothing
        } else if (isspace(c) && !delimiter_flag) {
            (*dimension)++;
            delimiter_flag = TRUE;
        } else if (isspace(c)) {
            // do nothing
        } else {
            delimiter_flag = FALSE;
            vector_delimiter_flag = FALSE;
        }
        
        if ((*buffer) == NULL || (*size) <= 0) {
            (*size) = 0x0f;
            (*buffer) = (char *) malloc(*size * sizeof(char));
            if ((*buffer) == NULL) {
                syslog(LOG_ERR, "error occurred: out of memory");
            }
        } else if (length == (*size)) {
            (*buffer) = (char *) realloc(*buffer, *size * 2 * sizeof(char));
            (*size) *= 2;
        }
        
        (*buffer)[length] = c;
        length++;
        if (c == EOF) break;
    }
    return length;
    
}

/*
 * Parse float numbers from input buffer and stores numbers
 * to the given output buffer.
 *
 * params:
 *      output_buffer: a pointer to the place where the first
 *          number will be stored.
 *      input_buffer: a text representation of the vector.
 *      length: The number of valid characters in the input
 *          buffer
 *      dimension: Number of characters to parse.
 */
void parse_vector(float *output_buffer, char *input_buffer, int length, int dimension) {
    char *begin = input_buffer;
    char *end = input_buffer;
    for (int i=0; i<dimension; i++) {
        output_buffer[i] = strtof(begin, &end);
        begin = end + 1;
    }
}

void read_vectors(FILE *file, float **output_buffer, int *dimension, int *n) {
    
    char *char_buffer = NULL;
    int char_buffer_size = 0;
    (*dimension) = 0;
    int length = read_vector(file, &char_buffer, &char_buffer_size, dimension);
    int vector_dimension = 0;
    // allocate output buffer
    (*n) = 0;
    int output_buffer_size = 10; // number of vectors
    (*output_buffer) = (float *) malloc(output_buffer_size * (*dimension) * sizeof(float));
    while(length > 0) {
        // reallocate output buffer if neccesery
        if ((*n) == output_buffer_size) {
            output_buffer_size *= 2;
            (*output_buffer) = (float *)realloc(*output_buffer, output_buffer_size * (*dimension) * sizeof(float));
        }
        parse_vector(&(*output_buffer)[(*n) * (*dimension)], char_buffer, length, *dimension);
        length = read_vector(file, &char_buffer, &char_buffer_size, &vector_dimension);
        (*n)++;
    }
    free(char_buffer);
    
}
