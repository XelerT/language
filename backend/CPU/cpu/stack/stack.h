#ifndef STACK_H
#define STACK_H
#include <cstdint>

// #define VAR_INFO(var) {#var, __PRETTY_FUNCTION__, __FILE__, __LINE__}
// #define stack_ctor(stk, capacity) oper_stack_ctor(stk, capacity, VAR_INFO(stk))
#define ASSERT_OK(stk) stack_dump(stk, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define stack_ctor(STACK) oper_stack_ctor(STACK, DEFAULT_CAPACITY, #STACK, __PRETTY_FUNCTION__,__FILE__, __LINE__)

#define $ fprintf(stderr, "I'm here. File %s Line %d\n", __FILE__, __LINE__);
#undef  $
#define $

#define FULL_SECURITY //Comment line to turn off canaries and hashes
#ifdef FULL_SECURITY
#define CANARY_ON
#define HASH_ON
#endif /*FULL_SECURITY*/

#ifndef FULL_SECURITY
// #define CANARY_ON //Uncomment line to turn on canaries
// #define HASH_ON   //Unomment line to turn on hashes
#endif /*!FULL_SECURITY*/

static const int DEF_CAPACITY = 10;
static const int N_CANARIES = 3;
static const int POISON = -2147443699;

typedef int elem_t;

#ifdef HASH_ON
struct gnu_hash
{
        uint64_t hash_stack = 0;
        uint64_t hash_data  = 0;
};
#endif /*HASH_ON*/

enum errors_t
{
        NULL_STACK_PTR = -1,
        NULL_DATA_PTR = -2,
        NULL_HASH_PTR = -3,
        PUSH_ERROR = -4,
        POP_ERROR = -2147443699,
        CTOR_ERROR = -5,
        RESIZE_ERROR = -6,
};

struct var_info
{
        int line = 0;
        const char *func;
        const char *file;
        const char *var;
};

struct stack
{
        int l_canary[N_CANARIES] = {};
        elem_t *data = nullptr;
        size_t size = 0;
        size_t capacity = 0;
        var_info info = {};
        // #ifdef HASH_ON
        gnu_hash *hash = nullptr;
        // #endif /*HASH*/
        int r_canary[N_CANARIES] = {};
};

void print_stack (stack *stk);
int stack_error (stack *stk);
int stack_dump (stack *stk, const char *func, const char *file, int line);
int oper_stack_ctor (stack *stk, size_t capacity, const char *var, const char *func, const char *file, int line);
int stack_resize (stack *stk, size_t capacity);
int stack_push (stack *stk, elem_t value);
elem_t stack_pop (stack *stk);
void fill_poison (stack *stk);
void free_stack  (stack *stk);

#endif /*STACK_H*/
