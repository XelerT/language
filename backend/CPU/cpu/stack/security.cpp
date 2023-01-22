#include <stdio.h>
#include <cstdint>

#include "security.h"

#ifdef CANARY_ON
static const int CANARY = -2147443648;
static const int DEAD_CANARY = 16;

void get_canaries (stack *stk)
{
        int value = CANARY;
        for (int i = 0; i < N_CANARIES; i++) {
                stk->l_canary[i] = value;
                stk->r_canary[i] = value;
        }
}

int check_canaries (stack *stk)
{
        for (int i = 0; i < N_CANARIES; i++)
                if (stk->l_canary[i] != CANARY || stk->r_canary[i] != CANARY)
                        return DEAD_CANARY;

        return 0;
}
#endif /*CANARY_ON*/


#ifdef HASH_ON
uint64_t gnu_hash_stack (uint64_t seed)
{
        uint64_t h = seed;

        h = ((h << 5) + h) + sizeof(stack);

        return h & 0xffffffff;
}

uint64_t gnu_hash_data (const void *ptr, uint64_t seed)
{
        const stack *stk = (const stack*) ptr;
        elem_t *data = (elem_t*) stk->data;
        uint64_t h = seed;

        for (size_t i = 0; i < stk->capacity; i++)
                h = ((h << 5) + h) + data[i];

        return h & 0xffffffff;
}

uint64_t check_data_hash (const void *ptr, uint64_t seed)
{
        const stack *stk = (const stack*) ptr;
        uint64_t new_hash = gnu_hash_data(stk, seed);
        if (new_hash != stk->hash->hash_data)
                return 1;
        return 0;
}

uint64_t check_stack_hash (const void *ptr, uint64_t seed)
{
        const stack *stk = (const stack*) ptr;
        uint64_t new_hash = gnu_hash_stack(seed);
        if (new_hash != stk->hash->hash_stack)
                return 1;
        return 0;
}
#endif /*HASH_ON*/
