#ifndef tools_h
#define tools_h

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <syslog.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_block.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

#define FALSE 0
#define TRUE 1

#define DEBUG(msg) syslog(LOG_DEBUG, msg);


void random_vector(int *vector, int vector_size, int data_size);
void select_random_centroids(gsl_matrix * training_set, gsl_matrix * centroids);
double calculate_inertia(gsl_matrix * training_set, gsl_matrix * centroids, int * classification);


#endif