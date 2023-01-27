#ifndef TEXT_TREE_H
#define TEXT_TREE_H

#include "tree.h"
#include "text_tree.h"

struct line_t
{
        char *ptr = nullptr;
        size_t length = 0;
};

struct text_t {
        size_t n_lines = 0;
        size_t n_chars = 0;
        char *buf = nullptr;
        line_t *lines = nullptr;
};

const int MAX_LINE_LENGTH = 512;

enum text_error {
        EMPTY_FILE    = -1,
        NULL_TEXT_PTR = -2,
        FILE_ERR      = 0xF11E
};

int get_text (FILE *input, text_t *text, const char *file_name);
void replace_n (text_t *text);
void divide_text (text_t *text);
void get_tree (tree_t *tree, node_t *node, text_t *text, size_t *line_count);
char* get_line(char *line);
char *skip_tabs (char *line);
int tree2text (node_t *node, FILE *output, int n_tabs);
// int edit_temp (node_t *temp_node, char node_type, double node_data);

// node_t* get_g (const char *str, size_t *char_count, tree_t *tree);
// node_t* get_div_mul (const char *str, size_t *char_count, tree_t *tree);
// node_t* get_t (const char *str, size_t *char_count, tree_t *tree);
// node_t* get_d (const char *str, size_t *char_count, tree_t *tree);
// node_t* get_brackets(const char *str, size_t *char_count, tree_t *tree);
// node_t* get_f (const char *str, size_t *char_count, tree_t *tree);
// node_t* get_n (const char *str, size_t *char_count);

#endif /*TREE_TEXT_H*/
