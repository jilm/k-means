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


#ifndef cluster_h
#define cluster_h

#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include <syslog.h>
#include <strings.h>
#include <ctype.h>

/*
 * buffer: an array of floats
 * size: the number of elements in the buffer
 */
typedef struct {
    float *buffer;
    int size;
} vector;

typedef struct {
    float *buffer;
    int dimension;
    int n;
    int size;
} matrix;

#define VECTOR(name, dim) vector name; \
name.buffer = (float *)malloc(dim * sizeof(float));\
name.size = dim;

#define MATRIX(name, dim, m) matrix name; \
name.buffer = (float *)malloc(dim * m * sizeof(float));\
name.dimension = dim;\
name.n = m;\
name.size = dim * m;\

#define RESET_MATRIX(matrix) \
    for (int __idx__=0; __idx__<matrix.size; __idx__++) \
        matrix.buffer[__idx__] = 0.0f;

/*
 * Returns the index of the closest centroid.
 *
 * In other wods it returns the index number of the cluster that the given
 * vector belongs to. Euclide norm is used to calculate vector distance.
 *
 *         eval = \arg\min_{i\in K} || v - c_i ||_2
 *
 * Params:
 *      v: a vector to clasify.
 *      centroids: the centroids.
 *
 * Returns a number from interval 0..centrids.n
 */
int evaluate_vector(vector v, matrix centroids, double *inertia);

/*
 * Returns a vector that contains a cluster index for each vector in the
 * training set. Moreover the flag that indicates evaluation change against
 * previous evaluation is returned. The evaluate_vector function is called
 * for each vector in the training set.
 *
 * Params:
 *      training_set:
 *      centrids:
 *      evaluation:
 *
 * Returns
 */
int evaluate_training_set(matrix training_set, matrix centroids, int *evaluation, double *inertia);

double k_means_train(matrix training_set, int k);

/*
 * For each cluster, it takes all of the vectors from training set and calculaes
 * the new centroid as an average number.
 *
 * Params:
 *      training_set:
 *      centroids:
 *      evaluation:
 */
void update_centroids(matrix trainig_set, matrix centroids, int *evaluation);

void random_vector(int *vector, int vector_size, int data_size);


void k_train(matrix training_set, matrix centroids, double *inertia);



void write_vectors(FILE *file, float *buffer, int dimension, int n);
void print_matrix(FILE *file, matrix m);


typedef struct {
    int size;
    int begin;
    int length;
    char *buffer;
} char_buffer;

#define FALSE 0
#define TRUE 1

#define BUFFER_SIZE 0xff
#define VECTOR_DELIMITER ';'
#define CR 0x0d
#define LF 0x0a

/*
 * Read, parse and return numeric values from the give file.
 *
 * Params:
 *      file: a text representation of input data
 *      output_buffer: a buffer to return result
 *      dimension: values on one row of the input file
 *      n: number of rows
 */
void read_vectors(FILE *file, float **output, int *dimension, int *n);


void parse_vector(float *output_buffer, char *input_buffer, int length, int dimension);

int read_vector(FILE *file, char **buffer, int *size, int *dimension);




#endif /* cluster_h */
