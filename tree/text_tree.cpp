#include <stdio.h>
#include <sys\stat.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <locale.h>

#include "text_tree.h"

int get_text (FILE *input, text_t *text, const char *file_name)
{
        if (!input) {
                log(1, "File pointer is null.");
                return NULL_FILE_PTR;
        }
        if (!text) {
                log(1, "Text pointer is null.");
                return NULL_TEXT_PTR;
        }

        struct stat file = {};
        if (stat(file_name, &file) < 0)
                return FILE_ERR;

        size_t n_chars = 0;
        char *buf = (char*) calloc(file.st_size + 1, sizeof(char));
        if (!buf) {
                log(1, "Calloc returned NULL.");
                return NULL_CALLOC;
        }

        n_chars = fread(buf, sizeof(char), file.st_size, input);
        setlocale(LC_CTYPE, "be_BY.UTF-8");
        log(3, "%d ўўўўўўўў %d", isalpha(*buf), *buf);
        text->n_chars = n_chars;

        text->buf = buf;
        text->n_lines = file.st_size - n_chars;
        if (!n_chars) {
                log(1, "Input file is empty.");
                return EMPTY_FILE;
        }
        return 0;
}

void replace_n (text_t *text)
{
        if (!text) {
                log(1, "Text pointer is null.");
                return;
        }

        char *buf = text->buf;

        for (size_t i = 0; i < text->n_chars; i++)
                if (text->buf[i] == '\n')
                        buf[i] = '\0';
}

void divide_text (text_t *text)
{
        if (!text) {
                log(1, "Text pointer is null.");
                return;
        }

        char *buf = text->buf;
        buf[text->n_chars + 1] = '\0';

        line_t *lines = (line_t*) calloc(text->n_lines + 1, sizeof(line_t));
        if (!lines) {
                log(1, "Calloc returned NULL.");
                return;
        }
        text->lines = lines;
        for (size_t i = 0; i < text->n_lines; i++) {
                lines[i].ptr = buf;
                while (*buf != '\n' && *buf != '\0')
                        buf++;
                buf++;
                lines[i].length = buf - lines[i].ptr;
        }
}

// node_t* get_g (const char *str, size_t *char_count, tree_t *tree)
// {
//         assert_ptr(str);
//         assert_ptr(char_count);
//         $
//         node_t *node = get_e(str, char_count, tree);
//
//         assert(*(str + *char_count) == '\0');
//         return node;
// }
//
// node_t* get_e (const char *str, size_t *char_count, tree_t *tree)
// {
//         assert_ptr(str);
//         assert_ptr(char_count);
//         $
//         node_t *l_node = get_t(str, char_count, tree);
//         node_t *node = nullptr;
//
//         while(*(str + *char_count) == '+' || *(str + *char_count) == '-') {
//                 node_t temp_node = {};
//                 char op = *(str + *char_count);
//                 ++*char_count;
//                 node_t *r_node = get_e(str, char_count, tree);
//
//                 temp_node.type = OPERATOR;
//                 temp_node.data.op = op;
//                 node = tree_insert(&temp_node);
//                 $
//                 node->left  = l_node;
//                 node->right = r_node;
//         }
//
//         if (node)
//                 return node;
//         return l_node;
// }
//
// node_t* get_t (const char *str, size_t *char_count, tree_t *tree)
// {
//         assert_ptr(str);
//         assert_ptr(char_count);
//         $
//         node_t *l_node = get_d(str, char_count, tree);
//         node_t *node = nullptr;
//
//         while(*(str + *char_count) == '*' || *(str + *char_count) == '/') {
//                 node_t temp_node = {};
//                 char op = *(str + *char_count);
//                 ++*char_count;
//                 node_t *r_node = get_t(str, char_count, tree);
//
//                 temp_node.type = OPERATOR;
//                 temp_node.data.op = op;
//                 node = tree_insert(&temp_node);
//                 $
//                 node->left  = l_node;
//                 node->right = r_node;
//         }
//
//         if (node)
//                 return node;
//         return l_node;
// }
//
// node_t* get_d (const char *str, size_t *char_count, tree_t *tree)
// {
//         assert_ptr(str);
//         assert_ptr(char_count);
//         $
//         node_t *l_node = get_p(str, char_count, tree);
//         node_t *node = nullptr;
//
//         while(*(str + *char_count) == '^') {
//                 node_t temp_node = {};
//                 char op = *(str + *char_count);
//                 ++*char_count;
//                 node_t *r_node = get_d(str, char_count, tree);
//
//                 temp_node.type = OPERATOR;
//                 temp_node.data.op = op;
//                 node = tree_insert(&temp_node);
//                 $
//                 node->left  = l_node;
//                 node->right = r_node;
//         }
//
//         if (node)
//                 return node;
//         return l_node;
// }
//
// node_t* get_p (const char *str, size_t *char_count, tree_t *tree)
// {
//         assert_ptr(str);
//         assert_ptr(char_count);
//         $
//         node_t *node = nullptr;
//         if (*(str + *char_count) == '(') {
//                 ++*char_count;
//                 node = get_e(str, char_count, tree);
//                 assert(*(str + *char_count) == ')');
//                 ++*char_count;
//         } else {
//                 node = get_f(str, char_count, tree);
//         }
//
//         return node;
// }
//
// node_t* get_a (const char *str, size_t *char_count, tree_t *tree)
// {
//         assert_ptr(str);
//         assert_ptr(char_count);
//         assert_ptr(tree);
//
//         node_t *l_node = get_f(str, char_count, tree);
//         node_t *node   = nullptr;
//
//         while (*(str + *char_count) == '=') {
//                 ++*char_count;
//                 node_t temp_node = {};
//                 edit_temp(&temp_node, ASSIGNMENT, '=');
//                 node_t *r_node = get_a(str, char_count, tree);
//
//                 node = tree_insert(&temp_node);
//                 node->left  = l_node;
//                 node->right = r_node;
//         }
//
//         if (node)
//                 return node;
//         return l_node;
// }
//
// node_t* get_f (const char *str, size_t *char_count, tree_t *tree)
// {
//         assert_ptr(str);
//         assert_ptr(char_count);
//         $
//         node_t *node = nullptr;
//         char name[MAX_NAME_LENGTH] = {};
//         int i = 0;
//
//         if ('a' <= *(str + *char_count) && *(str + *char_count) <= 'z') {
//                 if ('a' <= *(str + *char_count + 1) && *(str + *char_count + 1) <= 'z') {
//                         while (*(str + *char_count) != '(') {
//                                 name[i++] = *(str + *char_count);
//                                 ++*char_count;
//                         }
//                         ++*char_count;
//                         node_t temp_node = {};
//                         node_t *r_node = get_e(str, char_count, tree);
//
//                         temp_node.type = FUNC;
//                         strcpy(temp_node.name, name);
//                         node = tree_insert(&temp_node);
//                         node->right = r_node;
//
//                         temp_node.type = 0;
//                         assert(*(str + *char_count) == ')');
//                         ++*char_count;
//                 }  else {
//                         node = get_n(str, char_count);
//                 }
//         }  else {
//                 node = get_n(str, char_count);
//         }
//
//         return node;
// }
//
// node_t* get_n (const char *str, size_t *char_count)
// {
//         assert_ptr(str);
//         assert_ptr(char_count);
//         $
//         node_t node = {};
//         double val = 0;
//         size_t prev_char_count = *char_count;
//
//         while ('0' <= *(str + *char_count) && *(str + *char_count) <= '9') {
//                 val = (double) val * 10 + *(str + *char_count) - '0';
//                 ++*char_count;
//         }
//
//         if ('a' <= *(str + *char_count) && *(str + *char_count) <= 'z') {
//                 $
//                 node.data.var = *(str + *char_count);
//                 node.type = VARIABLE;
//                 ++*char_count;
//         } else if (val) {
//                 $
//                 node.data.dbl = val;
//                 node.type = NUMBER;
//         }
//
//         assert(*char_count != prev_char_count);
//         return tree_insert(&node);
// }
//
//
// int edit_temp (node_t *temp_node, char node_type, double node_data)          //returns 0 if smth wrong
// {
//         assert(temp_node);
//
//         temp_node->type = node_type;
//         switch (node_type) {
//         case NUMBER:
//                 temp_node->data.dbl = (double) node_data;
//                 return NUMBER;
//         case ASSIGNMENT:
//         case  OPERATOR:
//                 temp_node->data.op = (char) node_data;
//                 return OPERATOR;
//         case VARIABLE:
//         case CONST:
//                 temp_node->data.var = (char) node_data;
//                 return VARIABLE;
//         // case FUNC:
//         //         temp_node->name
//         default:
//                 return 0;
//         }
// }
