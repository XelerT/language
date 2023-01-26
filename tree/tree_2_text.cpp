#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "tree_2_text.h"

int tree_2_text (tree_t *tree, const char* txt_file_name)
{
        assert_ptr(tree);

        FILE *text_tree = fopen(txt_file_name, "w");
        if (write_node(text_tree, tree->root, 1))
                return UNKNOWN_TYPE;

        fclose(text_tree);
        return 0;
}

#define print(format_line, ...) fprintf(text_tree, format_line __VA_OPT__(,) __VA_ARGS__)

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
                print_tabs(text_tree, n_tabs - 1);
        }
        if (node->right) {
                print("\n");
                print_tabs(text_tree, n_tabs);
                write_node(text_tree, node->right, n_tabs + 1);
                print_tabs(text_tree, n_tabs - 1);
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
        case NEXT_ELEM:
        case END_LINE:
                print("%d", node->type);
                break;
        default:
                log(2, "Default switch case in texting tree(%s)", node->name);
                return UNKNOWN_TYPE;
        }

        print(" %d %d", node->left ? 1 : 0, node->right ? 1 : 0);

        return 0;
}

void print_tabs (FILE *text_tree, int n_tabs)
{
        assert_ptr(text_tree);

        for (int i = 0; i < n_tabs; i++)
                print("\t");
}

#undef print

#define skip    while (isspace(buf[*counter]))          \
                        ++*counter;

int text_2_tree (tree_t *tree)
{
        assert_ptr(tree);

        FILE *text_tree = fopen("tree.txt", "r");
        text_t text = {};

        get_text(text_tree, &text, "tree.txt");
        fclose(text_tree);

        node_t temp_node = {};
        size_t counter   =  1;
        int has_right    =  0;
        int has_left     =  0;

        read_node_data(text.buf, &counter, &temp_node);
        tree->root = tree_insert(&temp_node);

        if (text.buf[counter] == '1') {
                has_left = 1;
                counter += 2;
        } else if (text.buf[counter] == '0') {
                counter += 2;
        }

        if (text.buf[counter] == '1') {
                has_right = 1;
                ++counter;
        } else if (text.buf[counter] == '0') {
                ++counter;
        }

        read_node(text.buf, &counter, tree->root, has_left, has_right);

        return 0;
}

int read_node (char *buf, size_t *counter, node_t *node, int prev_has_left, int prev_has_right)
{
        assert_ptr(buf);
        assert_ptr(counter);
        assert_ptr(node);

        node_t temp_node = {};
        int has_right = 0;
        int has_left  = 0;

        skip

        if (buf[*counter] == '{') {
                ++*counter;
                read_node_data(buf, counter, &temp_node);

                if (prev_has_left)
                        node->left  = tree_insert(&temp_node);
                else
                        node->right = tree_insert(&temp_node);

                if (buf[*counter] == '1') {
                        has_left = 1;
                        *counter += 2;
                } else if (buf[*counter] == '0') {
                        *counter += 2;
                }
                if (buf[*counter] == '1') {
                        has_right = 1;
                        ++*counter;
                } else if (buf[*counter] == '0') {
                        ++*counter;
                }

                read_node(buf, counter, node->left ? node->left: node->right, has_left, has_right);

                skip

                while (buf[*counter] == '}') {
                        ++*counter;
                        skip
                }

                skip

                if (prev_has_right && prev_has_left) {
                        if (buf[*counter] == '{')
                                ++*counter;
                        read_node_data(buf, counter, &temp_node);
                        node->right = tree_insert(&temp_node);
                        skip
                        if (buf[*counter] == '1') {
                                has_left = 1;
                                *counter += 2;
                        } else if (buf[*counter] == '0') {
                                *counter += 2;
                        }
                        if (buf[*counter] == '1') {
                                has_right = 1;
                                ++*counter;
                        } else if (buf[*counter] == '0') {
                                ++*counter;
                                has_right = 0;
                        }
                        read_node(buf, counter, node->right, has_left, has_right);

                        if (buf[*counter] == '}')
                                ++*counter;
                }
        }

        return 0;
}

char read_node_data (char *buf, size_t *counter, node_t *node)
{
        assert_ptr(buf);
        assert_ptr(counter);
        assert_ptr(node);

        node->type = 0;
        free_buf(node->name);
        node->sub_type = 0;

        skip

        if (isdigit(buf[*counter])) {
                node->type = read_number(buf, counter);
                read_special_node_data(buf, counter, node);
        }
        skip

        if (isalpha(buf[*counter]))
                read_name(node->name, buf, counter);
        if (buf[*counter] == '\0')
                return 0;

        skip

        return buf[*counter];
}

int read_name (char *dest, char *src, size_t *counter)
{
        assert_ptr(dest);
        assert_ptr(src);
        assert_ptr(counter);

        for (int i = 0; isalpha(src[*counter]); i++) {
                dest[i] = src[*counter];
                ++*counter;
        }

        return 0;
}

int read_number (char *buf, size_t *counter)
{
        assert_ptr(buf);
        assert_ptr(counter);

        int val = 0;

        for (size_t i = 0; isdigit(buf[*counter]); i++) {
                val = val * 10 + (buf[*counter] - '0');
                ++*counter;
        }

        return val;
}

#pragma GCC diagnostic ignored "-Wformat="

int read_special_node_data (char *buf, size_t *counter, node_t *node)
{
        assert_ptr(buf);
        assert_ptr(counter);
        assert_ptr(node);

        free_buf(node->name);
        node->sub_type = 0;

        int indent = 0;

        switch (node->type) {
        case STAFF:
                sscanf(buf + *counter, "%d %n", &node->sub_type, &indent);
                *counter += indent;
                break;
        case FUNC:
        case NAME:
                sscanf(buf + *counter, "%s %n", node->name, &indent);
                *counter += indent;
                break;
        case FUNC_INIT:
        case VARIABLE:
                sscanf(buf + *counter, "%d %s %n", &node->sub_type, node->name, &indent);
                *counter += indent;
                break;
        case ASSIGNMENT:
                break;
        case NUMBER:
                sscanf(buf + *counter, "%d %d %n", &node->sub_type, &node->data, &indent);
                *counter += indent;
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
                sscanf(buf + *counter, "%d %n", &node->sub_type, &indent);
                *counter += indent;
                break;
        case NEXT_ELEM:
        case END_LINE:
                break;
        default:
                log(2, "Default switch case in texting tree(%s)", node->name);
                return UNKNOWN_TYPE;
        }

        switch (node->type) {
        case STAFF:
                break;
        case FUNC:
                node->atr.fillcolor = "#D681C2";
                break;
        case NAME:
                break;
        case FUNC_INIT:
                node->atr.fillcolor = "#EB8CD5";
                break;
        case VARIABLE:
                node->atr.fillcolor = "#93F558";
                break;
        case ASSIGNMENT:
                node->atr.fillcolor = "#CED8DB";
                break;
        case NUMBER:
                node->atr.fillcolor = "#CEEFF5";
                break;
        case ADD_OPERATOR:
        case SUB_OPERATOR:
                node->atr.fillcolor = "#F5B5D9";
                break;
        case MUL_OPERATOR:
        case DIV_OPERATOR:
                node->atr.fillcolor = "#F5B5D9";
                break;
        case RELATIVE_OP:
                node->atr.fillcolor = "#98B1B5";
                break;
        case CYCLE:
                node->atr.fillcolor = "#00d8ff";
                break;
        case AND:
        case OR:
                node->atr.fillcolor = "#98B1B5";
                break;
        case OPERATOR:
                if (node->sub_type == IF)
                        node->atr.fillcolor = "#18D5F5";
                else
                        node->atr.fillcolor = "#F51E2A";
                break;
        case NEXT_ELEM:
        case END_LINE:
                node->atr.fillcolor = "#BDCCF2";
                break;
        default:
                log(2, "Default switch case in texting tree(%s)", node->name);
                return UNKNOWN_TYPE;
        }

        return 0;
}

void free_buf (char *buf)
{
        assert_ptr(buf);

        for (int i = 0; buf[i] != '\0'; i++) {
                buf[i] = '\0';
        }
}
