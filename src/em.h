//
//  em.h
//  k-means
//
//  Created by Jiří Lidinský on 16/03/2019.
//  Copyright © 2019 Jiří Lidinský. All rights reserved.
//

#ifndef em_h
#define em_h

#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_block.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

#define FALSE 0
#define TRUE 1

int em_mean(gsl_matrix * training_set, int * classification, gsl_matrix * means);

int em_initialize(int k, gsl_matrix training_set, gsl_matrix *centroids, gsl_block *sigmas);

int em_covariance(gsl_matrix * training_set, int * classification, gsl_matrix * means, gsl_block * covariance);


#endif /* em_h */
