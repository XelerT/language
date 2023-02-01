#ifndef CMD_H
#define CMD_H

#include "../../include/utils.h"

// DEFINE_REG(DICK, 0x0, 'big_dick', HIDDEN, )
#define DEF_REG(name,num) REG_##name = num,

enum registers {
        #include "registers.en"
        N_REGS  = 9
};
#undef DEF_REG

enum args {
        ARG_IMMED = 1 << 5, //0010 0000
        ARG_REG   = 1 << 6, //0100 0000
        ARG_RAM   = 1 << 7  //1000 0000
};

enum masks {
        MASK_CMD  = 0x1F
};

// /* Instruction encoding */
// DEFINE_CMD(PUSH, 0xF, "push", ...)

#define DEF_CMD(name,num,...) CMD_##name = num,
#define DEF_STND_CMD(name,num,...) CMD_##name = num,
#define DEF_J_CMD(name,num,...) CMD_##name = num,
enum cmd {
        #include "instructions.en"
        CMD_LABEL      = 12,
        CMD_CALL_LABEL = 14
};
#undef DEF_CMD
#undef DEF_STND_CMD
#undef DEF_J_CMD

enum errors {
        NO_REGISTER    = -7,
        DIVIDE_ERR     = -6, //
        NO_LABEL       = -5, // 0x Hex speak
        EXECUTE_ERR    = -3,
        NO_SOURCE      = -2,
        NO_OUTPUT_NAME = -1
};

static const int threshold = 100;

#endif /*CMD_H*/
