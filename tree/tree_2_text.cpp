#include <stdio.h>
#include "tree_2_text.h"

int tree_2_text (tree_t *tree)
{
        assert_ptr(tree);

        FILE *text_tree = fopen("tree.txt", "w");
        if (write_node(text_tree, tree->root, 0))
                return UNKNOWN_TYPE;

        fclose(text_tree);
        return 0;
}

#define print(format_line, ... ) fprintf(text_tree, format_line __VA_OPT__(,) __VA_ARGS__)

int write_node (FILE *text_tree, node_t *node, int n_tabs)
{
        assert_ptr(text_tree);
        assert_ptr(node);

        print("{ ");
        if (write_node_data(text_tree, node))
                return UNKNOWN_TYPE;

        if (node->left) {
                print("\n");
                print_tabs(text_tree, n_tabs);
                write_node(text_tree, node->left, n_tabs + 1);
                print_tabs(text_tree, n_tabs);
        }
        if (node->right) {
                print("\n");
                print_tabs(text_tree, n_tabs);
                write_node(text_tree, node->right, n_tabs + 1);
                print_tabs(text_tree, n_tabs);
        }
        print(" }\n");

        return 0;
}

int write_node_data (FILE *text_tree, node_t *node)
{
        assert_ptr(node);

        switch (node->type) {
        case STAFF:
                print("%d %d", node->type, node->sub_type);
                break;
        case FUNC:
        case NAME:
                print("%d %s", node->type, node->name);
                break;
        case FUNC_INIT:
        case VARIABLE:
                print("%d %d %s", node->type, node->sub_type, node->name);
                break;
        case ASSIGNMENT:
                print("%d", node->type);
                break;
        case NUMBER:
                print("%d %d %d", node->type, node->sub_type, node->data);
                break;
        case ADD_OPERATOR:
        case SUB_OPERATOR:
        case MUL_OPERATOR:
        case DIV_OPERATOR:
        case RELATIVE_OP:
        case CYCLE:
        case AND:
        case OR:
        case OPERATOR:
                print("%d %d", node->type, node->sub_type);
                break;
        case END_LINE:
                print("%d", node->type);
                break;
        default:
                log(2, "Default switch case in texting tree(%s)", node->name);
                return UNKNOWN_TYPE;
        }

        return 0;
}

void print_tabs (FILE *text_tree, int n_tabs)
{
        assert_ptr(text_tree);

        for (int i = 0; i < n_tabs; i++)
                print("\t");
}

#undef print

// switch (node->type) {
//         case FUNC:
//                 print("%d %d %s\n", node->type, node->sub_type, node->name);
//                 break;
//         case STAFF:
//                 print("%d %d\n", node->type, node->sub_type);
//                 break;
//         case FUNC_INIT:
//                 print("%d %d\n", node->type, node->sub_type, node->name)
//                 break;
//         case RELATIVE_OP:
//         case CYCLE:
//         case AND:
//         case OR:
//         case OPERATOR:
//                 print("%d %d\n", node->type, node->sub_type);
//                 break;
//         case NAME:
//                 print("%d %s\n", node->type, node->name);
//                 break;
//         case VARIABLE:
//
//                 break;
//         case ASSIGNMENT:
//
//                 break;
//         case NUMBER:
//
//                 break;
//         case ADD_OPERATOR:
//
//                 break;
//         case SUB_OPERATOR:
//
//                 break;
//         case MUL_OPERATOR:
//
//                 break;
//         case DIV_OPERATOR:
//
//                 break;
//         default:
//                 log(2, "Default switch case in texting tree(%s)", node->name);
//         }
