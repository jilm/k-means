#ifndef nn_h
#define nn_h

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_block.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include "tools.h"

int nn_learn(gsl_matrix * training_set, gsl_matrix * means, int * classification);


#endif /* nn_h */
