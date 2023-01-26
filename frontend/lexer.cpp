#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>
#include "..\include\lexer.h"
#include "..\log\log.h"
#include "..\include\graph_tokens.h"

tokens_t* tokens_ctor (tokens_t *tokens, size_t capacity)
{
        assert_ptr(tokens);

        if (!capacity) {
                log(1, "Your value of capacity is null.");
                // return NULL_CAP;
        }

        tokens->tok_args = (token_arg_t*) calloc(capacity, sizeof(token_arg_t));
        if (!tokens->tok_args) {
                log(1, "Calloc returned null.");
                // return NULL_CALLOC;
        }
        tokens->capacity = capacity;
        log(2, "Created tokens with capacity %lld.", tokens->capacity);

        return tokens;
}

int resize_tokens (tokens_t *tokens)
{
        assert_ptr(tokens);

        if (tokens->size >= tokens->capacity / 2) {
                token_arg_t *new_tokens = (token_arg_t*) realloc(tokens->tok_args, tokens->capacity * 2);
                if (new_tokens) {
                        tokens->tok_args = new_tokens;
                        tokens->capacity = tokens->capacity * 2;
                }
                return RESIZED;
        } else if (tokens->size >= tokens->capacity * 4) {
                token_arg_t *new_tokens = (token_arg_t*) realloc(tokens->tok_args, tokens->capacity / 2);
                if (new_tokens) {
                        tokens->tok_args = new_tokens;
                        tokens->capacity = tokens->capacity / 2;
                }
                return RESIZED;
        }

        return 0;
}

// int insert_token (tokens_t *tokens, token_arg_t *token_arg)
// {
//         assert_ptr(tokens);
//         assert_ptr(token_arg);
//
//         resize_tokens(tokens);
//         tokens->tok_args[tokens->size++] = token_arg;
//
//         return 0;
// }

int get_tokens (tokens_t *tokens, const char *file_name)
{
        assert_ptr(tokens);
        assert_ptr(file_name);
        log(1, "<span style = \"color: blue; font-size:30px;\">START GETTING TOKENS</span>");

        text_t text = {};
        FILE *input = fopen(file_name, "r");
        if (!input) {
                log(1, "Fopen returned nullptr.");
                return NULL_FILE;
        }
        get_text(input, &text, file_name);
        token_arg_t  temp_token = {};

        for (size_t ip = 0, i = 0; ip < text.n_chars; i++) {
                if (get_arg(tokens, text.buf, &ip, i)) {
                        resize_tokens(tokens);
                        tokens->tok_args[i].atr = temp_token.atr;
                        tokens->size += 1;
                        log(2, "Number of tokens: %lld", tokens->size);
                        log(4, "token(%lld) is \"%s\", type: %d", i, tokens->tok_args[i].name, tokens->tok_args[i].type);
                } else {
                        i--;
                }
        }

        log(2, "%lld", tokens->size);
        tokens->tok_args[tokens->size].type = END_FILE;
        log(4, "token(%lld) is \"%s\", type: %d", tokens->size, tokens->tok_args[tokens->size].name,
                                       tokens->tok_args[tokens->size].type);

        tokens->tok_args[tokens->size].name[0] = '/';
        tokens->tok_args[tokens->size].atr = temp_token.atr;
        tokens->tok_args[tokens->size].val = '/';
        tokens->size += 1;

        return 0;
}

#define KW(name_arg,type_arg,num)       if (!stricmp(#name_arg, token->name)) {                 \
                                                token->type = type_arg;                         \
                                                token->sub_type = num;                          \
                                        } else

#define SYMB(key,arg,num,type_arg,is,code)  case arg:                                                       \
                                                if (key) {                                                 \
                                                        if (is)                                            \
                                                                code                                       \
                                                        contin;                                            \
                                                } else {                                                   \
                                                        get_punct(token, buf, ip);                         \
                                                }                                                          \
                                                log(2, "*****%lld****", type_arg);     \
                                                break;

#define assign(num) token->name[num] = buf[*ip]
#define ass_type(type_arg) token->type = type_arg
#define contin ++*ip

int get_arg (tokens_t *tokens, char *buf, size_t *ip, size_t tp)
{
        assert_ptr(tokens);
        assert_ptr(buf);
        assert_ptr(ip);

        token_arg_t *token = tokens->tok_args + tp;

        if (isalpha(buf[*ip])) {
                get_word(token, buf, ip);
#include "..\include\key_words.kw"
                /*else*/
                        token->type = NAME;
                log(3, "Got token with name: \"%s\" and type: \"%d\"", token->name, token->sub_type);
        } else if (isdigit(buf[*ip]) || (buf[*ip] == '-' && isdigit(buf[*ip + 1]) && tokens->tok_args[tp - 1].type != NUMBER &&
                                         tokens->tok_args[tp - 1].type != NAME && tokens->tok_args[tp - 1].type != CL_BRACKET)) {
                get_number(token, buf, ip);
                log(2, "DIGIT-TOKEN has type %d", token->type);
        } else {
                switch (buf[*ip]) {
#include "..\include\symbles.sym"
                default:
                        log(1, "here");
                        if (!get_relat_op(token, buf, ip)) {
                                if (buf[*ip] == '=') {
                                        token->type = ASSIGNMENT;
                                        token->name[0] = buf[*ip];
                                        ++*ip;
                                        log(3, "ASSIGNMENT %d %s", token->type, token->name);
                                        return 1;
                                }
                                get_punct(token, buf, ip);
                        }
                        ++*ip;
                        break;
                }
        }

        return 1;
}
#undef SYMB

int get_number(token_arg_t *token, char *buf, size_t *ip)
{
        assert_ptr(token);
        assert_ptr(buf);
        assert_ptr(ip);

        elem_t val = 0;
        char float_counter = -1;
        elem_t minus = 0;
        token->sub_type = INT;

        if (buf[*ip] == '-') {
                ++*ip;
                minus = -1;
        }

        for (int i = 0; (isdigit(buf[*ip]) || buf[*ip] == '.') && float_counter != 2; i++) {
                if (buf[*ip] == '.') {
                        token->sub_type = FLOAT;
                        ++*ip;
                        float_counter++;
                } else if (float_counter >= 0) {
                        float_counter++;
                }
                val = val * 10 + (buf[*ip] - '0');
                ++*ip;
        }

        if (minus) {
                val *= minus;
        }

        token->val = val;
        token->type = NUMBER;

        return 0;
}

int get_word(token_arg_t *token, char *buf, size_t *ip)
{
        assert_ptr(token);
        assert_ptr(buf);
        assert_ptr(ip);

        for (int i = 0; isalpha(buf[*ip]); i++) {
                token->name[i] = buf[*ip];
                ++*ip;
        }

        return 0;
}

int get_relat_op(token_arg_t *token, char *buf, size_t *ip)
{
        assert_ptr(token);
        assert_ptr(buf);
        assert_ptr(ip);

        switch (buf[*ip]) {
        case '=':
        case '!':
        case '>':
        case '<':
                if (buf[*ip + 1] == '=') {
                        token->name[0] = buf[(*ip)++];
                        token->name[1] = buf[*ip];
                        token->type = RELATIVE_OP;
                        return '=';
                } else if (buf[*ip] != '=') {
                        token->name[0] = buf[(*ip)++];
                        token->type = RELATIVE_OP;
                        return '<';
                }
                break;
        default:
                return 0;
        }

        return 0;
}

#undef SYMB

#define SYMB(key,arg,num,type_arg,is,code) case arg:                                                          \
                                                        if (!key) {                                                   \
                                                                if (is)                                               \
                                                                        code                                          \
                                                                contin;                                               \
                                                        }                                                             \
                                                        break;

int get_punct (token_arg_t *token, char *buf, size_t *ip)
{
        assert_ptr(token);
        assert_ptr(buf);
        assert_ptr(ip);

        switch (buf[*ip]) {
#include "..\include\symbles.sym"
        default:
                return 0;
        }

        return 0;
}

#undef assign
#undef ass_type
#undef contin
#undef SYMB
