BACK_CFILES = backend\main.cpp tree\tree.cpp tree\tree_dump.cpp tree\text_tree.cpp log\log.cpp \
	 frontend\lexer.cpp frontend\graph_tokens.cpp tree\token_tree.cpp backend\backend.cpp tree\tree_2_text.cpp

FRONT_CFILES = frontend\main.cpp tree\tree.cpp tree\tree_dump.cpp tree\text_tree.cpp log\log.cpp \
	 frontend\lexer.cpp frontend\graph_tokens.cpp tree\token_tree.cpp backend\backend.cpp tree\tree_2_text.cpp

ASM_CFILES = backend\CPU\buffer.cpp backend\CPU\asm\main.cpp backend\CPU\asm\assembler.cpp
ASM_OUTPUT = asm.exe

CPU_CFILES = backend\CPU\buffer.cpp backend\CPU\cpu\stack\stack.cpp backend\CPU\cpu\stack\security.cpp backend\CPU\cpu\main.cpp backend\CPU\cpu\cpu.cpp
CPU_OUTPUT = cpu.exe

DIS_ASM_CFILES = backend\CPU\buffer.cpp backend\CPU\dis_asm\main.cpp backend\CPU\dis_asm\dis_asm.cpp
DIS_ASM_OUTPUT = dis_asm.exe

IMG_FORMAT = png
DOT_FILE_NAME = output_utils\tokens_graph.dot
UTILS_DIRECTORY = output_utils

BACK_OUTPUT  = back.exe
FRONT_OUTPUT = front.exe

DRIVE_OUTPUT = drive.exe

CFLAGS= -Wshadow    			\
	-Winit-self 			\
	-Wredundant-decls 		\
	-Wcast-align			\
	-Wundef 			\
	-Wfloat-equal 			\
	-Winline 			\
	-Wunreachable-code 		\
	-Wmissing-declarations 		\
	-Wmissing-include-dirs 		\
	-Wswitch-enum 			\
	-Wswitch-default 		\
	-Weffc++ 			\
	-Wmain 				\
	-Wextra 			\
	-Wall 				\
	-g 				\
	-pipe 				\
	-fexceptions 			\
	-Wcast-qual 			\
	-Wconversion 			\
	-Wctor-dtor-privacy 		\
	-Wempty-body 			\
	-Wformat-security 		\
	-Wformat=2 			\
	-Wignored-qualifiers 		\
	-Wlogical-op 			\
	-Wmissing-field-initializers 	\
	-Wnon-virtual-dtor 		\
	-Woverloaded-virtual 		\
	-Wpointer-arith 		\
	-Wsign-promo 			\
	-Wstack-usage=8192 		\
	-Wstrict-aliasing 		\
	-Wstrict-null-sentinel 		\
	-Wtype-limits 			\
	-Wwrite-strings 		\
	-D_DEBUG 			\
	-D_EJUDGE_CLIENT_SIDE

all:
	@ cls
	@ g++ -o $(BACK_OUTPUT)  $(CFLAGS) $(BACK_CFILES)
	@ g++ -o $(FRONT_OUTPUT) $(CFLAGS) $(FRONT_CFILES)
	@ g++ -o $(ASM_OUTPUT)   $(CFLAGS) $(ASM_CFILES)
	@ g++ -o $(CPU_OUTPUT)   $(CFLAGS) $(CPU_CFILES)
	@ g++ -o $(DRIVE_OUTPUT) $(CFLAGS) drive.cpp
	@ echo Compiled c-files
def:
	@ g++ -E $(OUTPUT) $(CFLAGS) $(CFILES) >> defines.txt

.PHONY: run_asm
run_asm:
	@ $(ASM_OUTPUT) $(ASM_OUT) $(ASM_CODE)

.PHONY: run_cpu
run_cpu:
	@ $(CPU_OUTPUT) $(CPU_CODE)

.PHONY: run_lang
run_backend:
	@ $(BACK_OUTPUT) $(BACK_OUT) $(BACK_CODE)

.PHONY: run_frontend
run_frontend:
	@ $(FRONT_OUTPUT) $(LANG_OUT) $(LANG_CODE)


.PHONY: run
run:
	@ $(DRIVE_OUTPUT) asm_code.txt input.txt
	@ echo Run Drive

.PHONY: clean
clean:
	ifneq ($(OUTPUT))
		del -f $(OUTPUT)
	endif

.PHONY: clean_cmd
clean_cmd:
	@ cls

.PHONY: graphviz
graphviz:
	@ dot -T $(IMG_FORMAT) -o $(IMG) $(DOT)

.PHONY: original_graphviz
original_graphviz:
	@ dot -T $(IMG_FORMAT) -o tree_graph.$(IMG_FORMAT) output_utils\tree_graph.dot

.PHONY: graph_tokens
graph_tokens:
	@ dot -T $(IMG_FORMAT) -o output_utils\tokens.png output_utils\tokens_graph.dot

.PHONY: graph_tree
graph_tree:
	@ dot -T $(IMG_FORMAT) -o $(IMG_PATH) output_utils\tree_graph.dot

.PHONY: run_cpu
run_cpu:

