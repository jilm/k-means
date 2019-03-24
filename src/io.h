#ifndef io_h
#define io_h

#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <strings.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1

#define BUFFER_SIZE 0xff
#define VECTOR_DELIMITER ';'
#define CR 0x0d
#define LF 0x0a

int read_vector(FILE *file, char **buffer, int *size, int *dimension);

void parse_vector(double *output_buffer, char *input_buffer, int length, int dimension);

void read_matrix(FILE * file, double **output_buffer, int *dimension, int *n);

#endif /* io_h */