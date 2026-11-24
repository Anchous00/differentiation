#include <stdio.h>
#include <math.h>
#include "tree.h"

tree_node_t* diff( tree_node_t *node );
void solve_neutral_elements( tree_node_t *node );
void solve_const( tree_node_t *node );
void simplify_tree( tree_node_t *node );
hash_t tree_hash( tree_node_t *node );

int main( void )
{
    system("rm -f logs/*.gv");
    system("rm -f logs/*.svg");

    //tree_node_t *root = _MUL(_DIV(_NUM(2), _VAR('x')), _VAR('x'));
    tree_node_t *diff_tree = NULL;
    //tree_node_t *root = _COS(_SIN(_VAR('x')));
    //tree_node_t *root = _POW(_VAR('x'), _NUM(2));
    //tree_node_t *root = _COS(_NUM(1.5707));
    //tree_node_t *root = _LN(_VAR('x'));
    //tree_node_t *root = _ADD(_VAR('x'), _NUM(1));
    tree_node_t *root = _POW(_VAR('x'), _VAR('x'));

    //tree_node_t *root = _POW(_NUM(1), _NUM(1));
    //tree_node_t *root = _ADD(_ADD(_NUM(1), _VAR('x')), _VAR('x'));


    char c = 0;

    printf("q - quit\n1 - start session\n2 - save tree\n3 - load tree\n4 - draw tree\n5 - diff tree\n6 - save diff\n9 - superhlop\n");

    while ((c = (char)getchar()))
    {
        switch (c)
        {
            case 'q':
                destroy_node(&root);
                printf("root freed\n");
                printf("pointer %p root %p\n", diff_tree, root);
                destroy_node(&diff_tree);
                return 0;
                break;
            case '2':
                save_tree(root, "furry.txt");
                break;
            case '3':
                load_tree(&root, "furry.txt");
                break;
            case '4':
                log_file_open("log.html");
                tree_draw_graph(root);
                log_file_close();

                system("xdg-open log.html");
                //log_file_open("log.html");
                break;
            case '5':
                diff_tree = diff(root);
                log_file_open("log.html");
                tree_draw_graph(diff_tree);
                log_file_close();
                system("xdg-open log.html");
                break;
            case '6':
                save_tree(diff_tree, "furry.txt");
                break;
            case '9':
                simplify_tree(diff_tree);
                log_file_open("log.html");
                tree_draw_graph(diff_tree);
                log_file_close();
                system("xdg-open log.html");
                break;
            default:
                break;
        }
    }

    destroy_node(&root);
    destroy_node(&diff_tree);

    return 0;
}

tree_node_t* diff( tree_node_t *node )
{
    if (node == NULL)
        return NULL;
    switch (node->type)
    {
        case NUM:
            return _NUM(0);
        case VAR:
            if (node->value.var == 'x')
                return _NUM(1);
            return _NUM(0);
        case OP://TODO .tex dump
            switch (node->value.op)
            {
                case ADD:
                    return _ADD(_DL, _DR);
                    break;
                case SUB:
                    return _SUB(_DL, _DR);
                    break;
                case MUL:
                    return _ADD(_MUL(_CL, _DR), _MUL(_CR, _DL));
                    break;
                case DIV:
                    return _DIV(_SUB(_MUL(_DL, _CR), _MUL(_DR, _CL)), _MUL(_CL, _CL));
                    break;
                case POW:
                    return _MUL(_POW(_CL, _CR), _ADD(_DIV(_MUL(_DL, _CR), _CL), _MUL(_LN(_CL), _DR)));
                    break;
                case SIN:
                    return _MUL(_DR, _COS(_CR));
                case COS:
                    return _SUB(_NUM(0), _MUL(_DR, _SIN(_CR)));
                case LN:
                    return _DIV(_DR, _CR);
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return NULL;
}

void simplify_tree( tree_node_t *node )
{
    hash_t prev = tree_hash(node);
    do{
        prev = tree_hash(node);
        solve_const(node);
        solve_neutral_elements(node);
    } while (tree_hash(node) != prev);

    return;
}

void solve_const( tree_node_t *node )
{
    if (node == NULL)
        return;
    solve_const(node->left);
    solve_const(node->right);

    if (node->type == OP)
    {
        double res = 0;
        if (node->right->type == NUM && node->left->type == VAR)
        {
            switch (node->value.op)
            {
                case SIN:
                    res = sin(node->right->value.num);
                    break;
                case COS:
                    res = cos(node->right->value.num);
                    break;
                case LN:
                    res = log(node->right->value.num);
                    break;
                default:
                    return;
            }
        }
        else if (node->right->type == NUM && node->left->type == NUM)
        {
            switch (node->value.op)
            {
                case ADD:
                    res = node->left->value.num + node->right->value.num;
                    break;
                case SUB:
                    res = node->left->value.num - node->right->value.num;
                    break;
                case MUL:
                    res = node->left->value.num * node->right->value.num;
                    break;
                case DIV:
                    res = node->left->value.num / node->right->value.num;
                    break;
                case POW:
                    res = pow(node->left->value.num, node->right->value.num);
                    break;
                default:
                    return;
            }
        }
        else
            return;

        destroy_node(&node->left);
        destroy_node(&node->right);

        node->type = NUM;
        node->value.num = res;

        return;
    }

    return;
}

__attribute__((no_sanitize_address))
void solve_neutral_elements( tree_node_t *node )
{
    if (node == NULL)
        return;

    solve_neutral_elements(node->left);
    solve_neutral_elements(node->right);

    if (node->type == OP)
    {
        if (node->value.op == ADD)
        {

            if (node->left->type == NUM && node->left->value.num == 0)
            {
                destroy_node(&node->left);
                tree_node_t *right = node->right;
                *node = *(node->right);
                free(right);

                return;
            }
            else if (node->right->type == NUM && node->right->value.num == 0)
            {
                destroy_node(&node->right);
                tree_node_t *left = node->left;
                *node = *(node->left);
                free(left);

                return;
            }
        }
        else if (node->value.op == MUL || node->value.op == POW)
        {
            if (node->left->type == NUM && node->left->value.num == 1x)
            {
                destroy_node(&node->left);
                //free(node->right);
                tree_node_t *right = node->right;
                *node = *(node->right);
                free(right);

                return;
            }
            else if (node->right->type == NUM && node->right->value.num == 1)
            {
                destroy_node(&node->right);
                tree_node_t *left = node->left;
                //free(node->left);
                *node = *(node->left);
                free(left);

                return;
            }
        }
    }

    return;
}

hash_t tree_hash( tree_node_t *node )
{
    if (node == NULL)
        return 0;
    return *(hash_t*)(&node->value) + (tree_hash(node->left) << 5) + (tree_hash(node->right) << 5);
}

