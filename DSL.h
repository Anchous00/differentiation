#ifndef DSL_H
#define DSL_H

#define _NUM(X)     new_node(NUM, (tree_elem_t){.num = (X)}, NULL, NULL)
#define _VAR(X)     new_node(VAR, (tree_elem_t){.var = (X)}, NULL, NULL)

#define _ADD(X, Y)  new_node(OP,  (tree_elem_t){.op  = ADD}, (X), (Y))
#define _SUB(X, Y)  new_node(OP,  (tree_elem_t){.op  = SUB}, (X), (Y))
#define _MUL(X, Y)  new_node(OP,  (tree_elem_t){.op  = MUL}, (X), (Y))
#define _DIV(X, Y)  new_node(OP,  (tree_elem_t){.op  = DIV}, (X), (Y))
#define _POW(X, Y)  new_node(OP,  (tree_elem_t){.op  = POW}, (X), (Y))

#define _SIN(X)     new_node(OP,  (tree_elem_t){.op  = SIN}, _VAR(' '), (X))
#define _COS(X)     new_node(OP,  (tree_elem_t){.op  = COS}, _VAR(' '), (X))
#define _LN(X)      new_node(OP,  (tree_elem_t){.op  = LN }, _VAR(' '), (X))

#define _DR     diff(node->right)
#define _DL     diff(node->left)
#define _CR     node_copy(node->right)
#define _CL     node_copy(node->left)

#endif
