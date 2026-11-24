#ifndef UTILS_H
#define UTILS_H

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdint>

typedef uint64_t hash_t;

ssize_t file_size( const char* filename );
size_t input_text_from_file( FILE *input_file, char **buffer, const char *file_name );
#endif
