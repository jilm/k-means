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


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <syslog.h>
#include <gsl/gsl_matrix.h>
#include "cluster.h"

/* Flag set by ‘--verbose’. */
static int verbose_flag;
static int mode_flag = 0;
static int debug_flag = 0;

/*
 * Machine Learning clustering algorithm implementation.
 *
 * Usage:
 *    cluster -v --version
 *    cluster -d -v --clasify -k <int> --centr <filename> --clasification <filename> --clusters <filename>
 *    cluster -d -v --learn -k <int> --center <filename> --clasification <filename> --clusters <filename>
 *
 */
int main (int argc, char **argv) {
    
    gsl_matrix *pokus_matrix = gsl_matrix_alloc(6, 7);

    openlog(argv[0], LOG_PERROR | LOG_NDELAY, 0);
    syslog(LOG_INFO, "");
    
    int c;
    int k=0;
    char *centroids_filename = NULL;
    char *clasification_filename = NULL;
    char *clusters_filename = NULL;
    
    while (TRUE) {
        static struct option long_options[] = {
            /* These options set a flag. */
            {"verbose", no_argument, &verbose_flag, 1},
            {"brief",   no_argument, &verbose_flag, 0},
            /* These options don’t set a flag.
             We distinguish them by their indices. */
            
            {"version", no_argument, &mode_flag, 'v'},
            {"clasify", no_argument, &mode_flag, 'f'},
            {"learn",   no_argument, &mode_flag, 'l'},
            {"analyze", no_argument, &mode_flag, 'a'},
            
            {"debug",   no_argument, &debug_flag, 'd'},
            {"center",          required_argument, 0, 'c'},
            {"clasification",   required_argument, 0, 'n'},
            {"clusters",        required_argument, 0, 's'},
            {"k",        required_argument, 0, 'k'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;
        
        c = getopt_long (argc, argv, "vdflac:n:s:k:",
                         long_options, &option_index);
        
        /* Detect the end of the options. */
        if (c == -1)
            break;
        
        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;
                
            case 'v':
                puts ("option -v\n");
                break;
                
            case 'f':
                puts ("option -f\n");
                break;

            case 'l':
                puts ("option -l\n");
                break;

            case 'd':
                puts ("option -d\n");
                break;

            case 'c':
                centroids_filename = (char*)malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(centroids_filename, optarg);
                break;
                
            case 'n':
                clasification_filename = (char*)malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(clasification_filename, optarg);
                break;
                
            case 's':
                clusters_filename = (char*)malloc((strlen(optarg) + 1) * sizeof(char));
                strcpy(clusters_filename, optarg);
                break;
                
            case 'k':
                k = (int)strtol(optarg, NULL, 10);
                break;

            case '?':
                /* getopt_long already printed an error message. */
                break;
                
            default:
                abort ();
        }
    }
    
    /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
    if (debug_flag) {
        setlogmask(LOG_UPTO(LOG_DEBUG));
    } else if (verbose_flag) {
        setlogmask(LOG_UPTO(LOG_INFO));
    } else {
        setlogmask(LOG_UPTO(LOG_WARNING));
    }
    
    /* load training set */
    syslog(LOG_DEBUG, "Going to load a training set...");
    matrix training_set;
    int dimension = 0;
    int n = 0;
    read_vectors(stdin, &(training_set.buffer), &(dimension), &(n));
    syslog(LOG_DEBUG, "Training set has been loaded.");
    training_set.dimension = dimension;
    training_set.n = n;
    syslog(LOG_DEBUG, "dimension: %d, size: %d\n", dimension, n);

    if (mode_flag == 'f') {
        syslog(LOG_DEBUG, "Entering clasify mode.");
        
    } else if (mode_flag == 'l') {
        syslog(LOG_DEBUG, "Entering learn mode.");
    } else if (mode_flag == 'a') {
        
        for (k = 2; k < training_set.dimension / 4; k++) {
            double inertia = k_means_train(training_set, k);
            printf("%g\n", inertia);
        }
        return 0;
        
    }
    
    
    // select initial cluster center
    int *centroid_indexes = (int*)malloc(k * sizeof(int));
    random_vector(centroid_indexes, k, n);
    for (int i=0; i<k; i++) {
        printf("%d\n", centroid_indexes[i]);
    }
    MATRIX(centroids, dimension, k)
    for (int i=0; i<k; i++) {
        for (int j=0; j<dimension; j++) {
            centroids.buffer[i * dimension + j] = training_set.buffer[centroid_indexes[i]*dimension+j];
        }
    }
    
    syslog(LOG_DEBUG, "Going to enter the learning loop...");
    double inertia = 0.0;
    k_train(training_set, centroids, &inertia);

    // write results
    FILE *centroids_file = NULL;
    if (centroids_filename == NULL) {
        // do nothing
    } else if (centroids_filename[0] == '-' ) {
        syslog(LOG_DEBUG, "Going to write centroid coordinates into the stdout...");
        centroids_file = stdout;
        print_matrix(centroids_file, centroids);
    } else {
        syslog(LOG_DEBUG, "Going to write centroid coordinates into the file: %s", centroids_filename);
        centroids_file = fopen(centroids_filename, "w");
        print_matrix(centroids_file, centroids);
        fclose(centroids_file);
    }

    

    return 0;
}
