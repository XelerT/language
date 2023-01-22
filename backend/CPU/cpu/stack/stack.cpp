#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <cstdint>

#include "security.h"
#ifdef HASH_ON
static const char N_ERRORS = 7;
#elif  CANARY_ON
static const char N_ERRORS = 5;
#else
static const char N_ERRORS = 4;
#endif  /*HASH_ON*/

// static int errors = 0;

int oper_stack_ctor (stack *stk, size_t capacity, const char *var, const char *func, const char *file, int line)
{
        assert(stk);

        stk->capacity = capacity;
        stk->data = (elem_t*) calloc(stk->capacity, sizeof(stack)); // how to make it with voids
        if (stk->data == nullptr)
                return NULL_DATA_PTR;
        fill_poison (stk);
        stk->info.line = line;
        stk->info.file = file;
        stk->info.func = func;
        stk->info.var = var;

#ifdef HASH_ON
        stk->hash = (gnu_hash*) calloc(1, sizeof(gnu_hash));
        if (stk->data == nullptr)
                return NULL_HASH_PTR;
        stk->hash->hash_stack = gnu_hash_stack (SEED);
        stk->hash->hash_data  = gnu_hash_data  (stk, SEED);
#endif /*HASH_ON*/

        get_canaries(stk);

        // ASSERT_OK(stk,__PRETTY_FUNCTION__,__FILE__,__LINE__);
        if (ASSERT_OK(stk))
                return CTOR_ERROR;
        return 0;
}

int stack_resize (stack *stk, size_t capacity)
{
        assert(stk);
        stk->capacity = capacity;
        elem_t *ptr = (elem_t*) realloc(stk->data, stk->capacity * sizeof(elem_t));
        if (ptr == nullptr)
                return NULL_DATA_PTR;
        stk->data = ptr;

        fill_poison(stk);

#ifdef HASH_ON
        stk->hash->hash_data  = gnu_hash_data(stk, SEED);
#endif /*HASH_ON*/

        // ASSERT_OK(stk,__PRETTY_FUNCTION__,__FILE__,__LINE__);
        if (ASSERT_OK(stk))
                return RESIZE_ERROR;

        return 0;
}

int stack_push (stack *stk, elem_t value) //returns errors
{
        assert(stk);
        // ASSERT_OK(stk,__PRETTY_FUNCTION__,__FILE__,__LINE__);
        if (ASSERT_OK(stk))
                return PUSH_ERROR;
        if (stk->size + DEF_CAPACITY > stk->capacity) {
                stack_resize(stk, stk->capacity + DEF_CAPACITY);
        }
        elem_t *data = stk->data + (int) stk->size++;
        *data = value;

        #ifdef HASH_ON
        stk->hash->hash_data  = gnu_hash_data  (stk, SEED);
        #endif /*HASH_ON*/

        // ASSERT_OK(stk,__PRETTY_FUNCTION__,__FILE__,__LINE__);

        return ASSERT_OK(stk);
}

elem_t stack_pop(stack *stk)
{
        assert(stk);
        if (ASSERT_OK(stk))
                return POP_ERROR;

        if (stk->size == 0)
                return POISON;
        elem_t poped_val = stk->data[--stk->size];
        stk->data[stk->size] = POISON;

        if (stk->size +  2 * DEF_CAPACITY < stk->capacity)
                stk->capacity = stk->capacity - 2 * DEF_CAPACITY;

#ifdef HASH_ON
        stk->hash->hash_data  = gnu_hash_data  (stk, SEED);
#endif /*HASH_ON*/

        if (ASSERT_OK(stk))
                return POP_ERROR;

        return poped_val;
}

void print_stack (stack *stk)
{
        for (size_t i = 0; i < stk->capacity; i++) {
                printf("%d ", stk->data[i]);
        }
        printf("\n");
}

void fill_poison (stack *stk)
{
        for (size_t i = stk->size + 1; i < stk->capacity; i++)
                if (stk->data[i] == 0)
                        stk->data[i] = POISON;
}

void free_stack (stack *stk)
{
        free(stk->data);
        free(stk->hash);
}

int stack_dump (stack *stk, const char *func, const char *file, int line) // print all info, returns errors
{
        // FILE *output = nullptr;
        // output = fopen("log.txt", "a");

        int errors = stack_error(stk);
        if (errors == 0)
                return 0;

        int multp_errors = 0;
        for (int i = 1; i < N_ERRORS + 1; i++) {
                if (((errors >> (N_ERRORS - i)) & ~(~0u << 1)) != 0) {
                        printf("\nError:\n");
                        if (i == N_ERRORS) {
                                printf(" Stack pointer is null.\n\n");
                                return NULL_STACK_PTR;
                        } else if (i == N_ERRORS - 1) {
                                printf(" Data pointer in null.\n\n");
                                return NULL_DATA_PTR;
                        }
                        if (!multp_errors) {
                                printf(" %s at %s (%d):\n"
                                       " Stack[%p] (%s)\n"
                                       " %s at %s at %s (%d)\n"
                                       " size = %lld,"
                                       " capacity = %lld,"
                                       " data[%p]\n", func, file, line,
                                       (stk == nullptr) ? 0: stk, (stk == nullptr) ? "mistake": "ok", stk->info.var,
                                       stk->info.func, stk->info.file, stk->info.line, stk->size, stk->capacity, (stk->data == nullptr) ? 0: stk->data);
#ifdef HASH_ON
                                printf("Required stack hash: %lld\nActual   stack hash: %lld\n", stk->hash->hash_stack, gnu_hash_stack (SEED));
                                printf("Required  data hash: %lld\nActual    data hash: %lld\n", stk->hash->hash_data,  gnu_hash_data  (stk, SEED));
#endif /*HASH_ON*/
                                for (size_t j = 0; j < stk->capacity; j++) {
                                        if (stk->data[j] == POISON)
                                                        printf("[%lld] = NAN(POISON)\n", j);
                                        else
                                                printf("*[%lld] = %d\n", j, stk->data[j]);
                                }
                        }
                        multp_errors = 1;
                        switch (i) {
                        case N_ERRORS - 2:
                                printf(" Stack size is negative.\n\n");
                                break;
                        case N_ERRORS - 3:
                                printf(" Stack capacity is negative.\n\n");
                                break;
#ifdef CANARY_ON
                        case N_ERRORS - 4:
                                printf(" Canary has been changed.\n\n");
                                return 2;
#endif /*CANARY_ON*/
#ifdef HASH_ON
                        case N_ERRORS - 5:
                                printf(" Hash of data has been changed.\n\n");
                                return 5;
                        case N_ERRORS - 6:
                                printf(" Hash of struct has been changed.\n\n");
                                return 6;
#endif /*HASH_ON*/
                        default:
                                printf("Unknown error.\n");
                       }
                        printf("\n");
                }
        }

        // fclose(output);
        return 0;
}

int stack_error (stack *stk) // checks have an error
{
        int error = 0;
        if (stk == nullptr) {
                error |= 1;
                return error;
        }
#ifdef CANARY_ON
        if (check_canaries(stk)) {
                error |= 16;
                return error;
        }
#endif /*CANARY_ON*/
#ifdef HASH_ON
        if (check_data_hash(stk, SEED)) {
                error |= 32;
        }
        if (check_stack_hash(stk, SEED)) {
                error |= 64;
                return error;
        }
#endif /*HASH_ON*/
        if (stk->data == nullptr)
                error |= 2;

        return error;
}
