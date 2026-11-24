#ifndef TREE_H
#define TREE_H

#include <string.h>
#include <ctype.h>
#include "logs/logger.h"
#include "utils/utils.h"
#include "DSL.h"

enum type_t
{
    VAR = 0,
    NUM = 1,
    OP  = 2
};

enum op_t
{
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
    SIN = 4,
    COS = 5,
    LN  = 6,
    POW = '^'
};

union tree_elem_t
{
    double num;
    op_t op;
    char var;
};

struct tree_node_t
{
    type_t type;
    tree_elem_t value;
    tree_node_t *left, *right;
};


void link_node   ( tree_node_t **node, tree_elem_t value);
void destroy_node( tree_node_t **node                   );
tree_node_t* new_node( type_t type, tree_elem_t val, tree_node_t *left, tree_node_t *right );
tree_node_t* node_copy( tree_node_t *node );


void tree_draw_graph( tree_node_t *root                           );
bool draw_node      ( tree_node_t *node, FILE *graph, size_t rank );

void save_tree( tree_node_t *root, const char *filename );
void save_node( tree_node_t *node, FILE *file           );

void load_tree( tree_node_t **root, const char *filename );
tree_node_t* load_node( char **buffer, size_t offset );

#endif
