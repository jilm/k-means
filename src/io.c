
#include "io.h"

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
void parse_vector(double *output_buffer, char *input_buffer, int length, int dimension) {

    char *begin = input_buffer;
    char *end = input_buffer;
    for (int i=0; i<dimension; i++) {
        output_buffer[i] = strtof(begin, &end);
        begin = end + 1;
    }

}


void read_matrix(FILE * file, double **output_buffer, int *dimension, int *n) {
    
    char *char_buffer = NULL;
    int char_buffer_size = 0;
    (*dimension) = 0;
    int length = read_vector(file, &char_buffer, &char_buffer_size, dimension);
    int vector_dimension = 0;
    // allocate output buffer
    (*n) = 0;
    int output_buffer_size = 10; // number of vectors
    (*output_buffer) = (double *) malloc(output_buffer_size * (*dimension) * sizeof(double));
    while(length > 0) {
        // reallocate output buffer if neccesery
        if ((*n) == output_buffer_size) {
            output_buffer_size *= 2;
            (*output_buffer) = (double *)realloc(*output_buffer, output_buffer_size * (*dimension) * sizeof(double));
        }
        parse_vector(&(*output_buffer)[(*n) * (*dimension)], char_buffer, length, *dimension);
        length = read_vector(file, &char_buffer, &char_buffer_size, &vector_dimension);
        (*n)++;
    }
    free(char_buffer);
    
}

