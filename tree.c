#include "tree.h"


tree_node_t* new_node( type_t type, tree_elem_t val, tree_node_t *left, tree_node_t *right )
{
    tree_node_t *node = (tree_node_t*)calloc(1, sizeof(tree_node_t));
    if (node == NULL)
        return node;
    node->type = type;
    switch (type)
    {
        case NUM:
            node->value.num = val.num;
            break;
        case VAR:
            node->value.var = val.var;
            break;
        case OP:
            node->value.op  = val.op;
            break;
        default:
            printf("че за хуйня <%d>\n", type);
            break;
    }

    node->left  = left;
    node->right = right;

    return node;
}

tree_node_t* node_copy( tree_node_t *node )
{
    if (node == NULL)
        return NULL;
    if (node->type == OP)
        return new_node(node->type, (tree_elem_t){.op = node->value.op}, node_copy(node->left), node_copy(node->right));
    return new_node(node->type, node->value, node_copy(node->left), node_copy(node->right));
}

void link_node( tree_node_t **node, tree_elem_t value)
{
    *node = new_node(VAR, value, NULL, NULL);

    return;
}

__attribute__((no_sanitize_address))
void destroy_node( tree_node_t **node )
{
    if (*node == NULL)
        return;
    destroy_node(&((*node)->left));
    destroy_node(&((*node)->right));
    //printf("node type %d\n", (*node)->type);
    //printf("node value %d\n", (*node)->value);

    free((*node));

    (*node)->value = (tree_elem_t){0};
    (*node) = NULL;

    return;
}

void save_tree( tree_node_t *root, const char *filename )
{
    FILE* save_file = fopen(filename, "w");
    if (save_file == NULL)
    {
        printf("ZZZZZOVV");
        return;
    }
    save_node(root, save_file);

    if (fclose(save_file)) printf("can't close save file\n");
    return;
}

void save_node( tree_node_t *node, FILE *file )
{
    if (node == NULL)
    {
        return;
    }
    fprintf(file, "(");
    save_node(node->left, file);
    switch (node->type)
    {
        case NUM:
            fprintf(file, "%lg", node->value.num);
            break;
        case VAR:
            fprintf(file, "%c", node->value.var);
            break;
        case OP:
            switch (node->value.op)
            {
                case SIN:
                    fprintf(file, "sin");
                    break;
                case COS:
                    fprintf(file, "cos");
                    break;
                default:
                    fprintf(file, "%c", node->value.op);
                    break;
            }
            break;
        default:
            printf("uebka\n");
            break;
    }
    save_node(node->right, file);
    fprintf(file, ")");

    return;
}

void load_tree( tree_node_t **root, const char *file_name )
{
    FILE *source_file = fopen(file_name, "r");
    if (source_file == NULL)
    {
        printf("can't open file\n");
        return;
    }

    char *buffer = NULL;
    input_text_from_file(source_file, &buffer, file_name);

    // printf("%s\n", buffer);
    // sscanf(buffer, "(%s", buffer);
    // printf("%s\n", buffer);
    char *buffer_tmp = buffer;

    destroy_node(root);
    *root = load_node(&buffer_tmp, 0);

    free(buffer);

    return;
}

tree_node_t* load_node( char **buffer, size_t offset )
{
    size_t i = 1 + offset;
    //*buffer = *buffer + i + offset;
    printf("\n\nbuffer %s\n", *buffer + i);
    size_t brackets = (*(*buffer + i) == '(');
    i++;
    bool op = false;
    while (brackets > 0)
    {
        //printf("symbol %c", **buffer);
        if (*(*buffer + i) == '(') brackets++;
        if (*(*buffer + i) == ')') brackets--;
        //printf("brackets %zu\n", brackets);
        i++;
    }
    op = (*(*buffer + i - 1) == ')');
    printf("reached <%c>, op is %d\n", *(*buffer + i), op);
    if (op)
    {
        switch (*(*buffer + i))
        {
            case '+':
                return _ADD(load_node(buffer, 1), load_node(buffer, i + 1));
            case '*':
                return _MUL(load_node(buffer, 1), load_node(buffer, i + 1));
            case '-':
                return _SUB(load_node(buffer, 1), load_node(buffer, i + 1));
            case '/':
                return _DIV(load_node(buffer, 1), load_node(buffer, i + 1));
            case '^':
                return _POW(load_node(buffer, 1), load_node(buffer, i + 1));
            case 's':
                if (*(*buffer + i + 1) == 'i' && *(*buffer + i + 2) == 'n')
                    return _SIN(load_node(buffer, i + 3));
                break;
            case 'c':
                if (*(*buffer + i + 1) == 'o' && *(*buffer + i + 2) == 's')
                    return _COS(load_node(buffer, i + 3));
                break;
            default:
                printf("че за операция %c\n", *(*buffer + i));
                break;
        }
    }
    i--;
    if (isdigit(*(*buffer + i)))
        return _NUM(atoi(*buffer + i));

    return _VAR(*(*buffer + i));

}

void tree_draw_graph( tree_node_t *root )
{
    time_t t = time(NULL);
    struct tm *time_info = localtime(&t);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);  // или timespec_get(&ts, TIME_UTC)

    char time_str[64] = {};
    strftime(time_str, sizeof(time_str), "%d%m%Y%H%M%S", time_info);

    char file_path[256] = {};
    snprintf(file_path, sizeof(file_path), "logs/%s%ld.gv", time_str, ts.tv_nsec);

    FILE *graph = fopen(file_path, "w");
    if (graph == NULL)
    {
        printf("can't open graph file\n");
        return;
    }


    fprintf(graph,  "digraph Tree{\n"
                    "bgcolor=\"transparent\";\n"
                    "rankdir=TB;\n"
                    "node [shape=record, fontname=\"Courier\"];\n"
                    "edge [color=\"#4d9ae2\"];\n"
                    "splines=\"ortho\"\n"
            );

    draw_node(root, graph, 0);


    fprintf(graph, "}\n");

    if (fclose(graph))
        printf("can't close graph file\n");

    char dot_cmd[1024] = {};
    snprintf(dot_cmd, sizeof(dot_cmd), "dot -Tsvg logs/%s%ld.gv -o logs/%s%ld.svg", time_str, ts.tv_nsec, time_str, ts.tv_nsec);

    system(dot_cmd);

    LOG(myLOG_DEFAULT, "<img src=\"logs/%s%ld.svg\" style=\"width: 100%%; height: auto;\">", time_str, ts.tv_nsec);

    return;
}

bool draw_node( tree_node_t *node, FILE *graph, size_t rank )
{
    if (graph == NULL || node == NULL)
        return 0;
    fprintf(graph, "%llu [label=\"{", (unsigned long long)node);
    switch (node->type)
    {
        case OP:
            switch (node->value.op)
            {
                case COS:
                    fprintf(graph, "cos");
                    break;
                case LN:
                    fprintf(graph, "ln");
                    break;
                case SIN:
                    fprintf(graph, "sin");
                    break;
                default:
                    fprintf(graph, "%c", node->value.op);
                    break;
            }
            break;
        case VAR:
        fprintf(graph, "%c", node->value.var);
            break;
        case NUM:
        fprintf(graph, "%lg", node->value.num);
            break;
        default:
            return false;
    }
    fprintf(graph, "| type = %d | {<left> left = %llu | <right> right = %llu}}\", style=\"rounded,filled\", fillcolor=\"#4ce372\", rank=%zu];\n", node->type, (unsigned long long)node->left, (unsigned long long)node->right, rank);

    if (draw_node(node->right, graph, rank + 1))
        fprintf(graph, "%llu:right->%llu;\n", (unsigned long long)node, (unsigned long long)node->right);
    if (draw_node(node->left, graph, rank + 1))
        fprintf(graph, "%llu:left->%llu;\n", (unsigned long long)node, (unsigned long long)node->left);

    return 1;
}
