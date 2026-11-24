#include "utils.h"

ssize_t file_size( const char* filename )
{
    struct stat input_file_stat = {};

    if (stat(filename, &input_file_stat) == -1)
        return -1;

    return input_file_stat.st_size;
}

size_t input_text_from_file( FILE *input_file, char **buffer, const char *file_name )
{
    const ssize_t FILE_SIZE = file_size(file_name);
    if (FILE_SIZE < 0)
        return 0;

    *buffer = (char*)calloc((size_t)FILE_SIZE + 1, sizeof(char));
    if (*buffer == NULL)
        return 0;

    size_t symbcnt = fread(*buffer, sizeof(char), (size_t)FILE_SIZE, input_file);
    (*buffer)[symbcnt - 1] = '\0';

    if (fclose(input_file) == -1)
        fprintf(stderr, "can't close input file\n");

    return symbcnt;
}
