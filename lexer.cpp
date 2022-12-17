#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "log\log.h"
#include "graph_tokens.h"

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

        if (tokens->size >= tokens->capacity - 50) {
                token_arg_t *new_tokens = (token_arg_t*) realloc(tokens->tok_args, tokens->capacity * 2);
                if (new_tokens)
                        tokens->tok_args = new_tokens;
                tokens->capacity = tokens->capacity * 2;
                return RESIZED;
        } else if (tokens->size >= tokens->capacity * 4) {
                token_arg_t *new_tokens = (token_arg_t*) realloc(tokens->tok_args, tokens->capacity / 2);
                tokens->capacity = tokens->capacity / 2;
                if (new_tokens)
                        tokens->tok_args = new_tokens;
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

        text_t text = {};
        FILE *input = fopen(file_name, "r");
        if (!input) {
                log(1, "Fopen returned nullptr.");
                return NULL_FILE;
        }
        get_text(input, &text, file_name);
        token_arg_t  temp_token = {};

        for (size_t ip = 0, i = 0; ip < text.n_chars; ip++, i++) {
                $lld(ip)
                if (get_arg(tokens->tok_args + i, text.buf, &ip)) {
                        $
                        resize_tokens(tokens);
                        tokens->tok_args[i].atr = temp_token.atr;
                        tokens->size += 1;
                        log(2, "Number of tokens: %lld", tokens->size);
                        log(3, "token(%lld) is \"%s\"", i, tokens->tok_args[i].name);
                        // return ARG_ERROR;
                } else {
                        i--;
                }
                $lld(ip)
        }

        return 0;
}

int get_arg (token_arg_t *token, char *buf, size_t *ip)           //add assert errors
{
        assert_ptr(token);
        assert_ptr(buf);
        assert_ptr(ip);

        if ('a' <= buf[*ip] && buf[*ip] <= 'z') {
                get_word(token, buf, ip);
                token->type = WORD;
        } else if ('1' <= buf[*ip] && buf[*ip] <= '9') {
                get_number(token, buf, ip);
                token->type = NUMBER;
        } else {
                log(1, "HERE");
                $c(buf[*ip])
                switch (buf[*ip]) {
                case '+':
                case '-':
                case '*':
                case '/':
                        token->type    = OPERATOR;
                        token->name[0] = buf[*ip];
                        ++*ip;
                        break;
                case '\n':
                case ' ':
                        return 0;
                default:
                        log(1, "HERE");
                        if (!get_relat_op(token, buf, ip)) {
                                log(1, "HERE");
                                if (buf[*ip] == '=') {
                                        log(1, "HERE");
                                        token->type = ASSIGNMENT;
                                        token->name[0] = buf[*ip];
                                        break;
                                }
                                get_punct(token, buf, ip);
                        }
                        break;
                }
        }

        return 1;
}

int get_number(token_arg_t *token, char *buf, size_t *ip)
{
        assert_ptr(token);
        assert_ptr(buf);
        assert_ptr(ip);

        for (int i = 0; '1' <= buf[*ip] && buf[*ip] <= '9'; i++) {
                token->name[i] = buf[*ip];
                ++*ip;
        }

        return 0;
}

int get_word(token_arg_t *token, char *buf, size_t *ip)
{
        assert_ptr(token);
        assert_ptr(buf);
        assert_ptr(ip);

        for (int i = 0; 'a' <= buf[*ip] && buf[*ip] <= 'z'; i++) {
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
        log(1, "HERE");
        $
        switch (buf[*ip]) {
        case '=':
        case '!':
        case '>':
        case '<':
                $
                log(1, "HERE FUCK");
                if (buf[*ip + 1] == '=') {
                        token->name[0] = buf[(*ip)++];
                        token->type = RELATIVE_OP;
                        // ++*ip;
                        return '=';
                }
                break;
        default:
                return 0;
        }

        return 0;
}

int get_punct (token_arg_t *token, char *buf, size_t *ip)
{
        assert_ptr(token);
        assert_ptr(buf);
        assert_ptr(ip);

        switch (buf[*ip]) {
        case '.':
        case ',':
        case '_':
                break;
        case ';':
                token->type = END_LINE;
                token->name[0] = buf[*ip];
                ++*ip;
                break;
        default:
                return 0;
        }

        return 0;
}
