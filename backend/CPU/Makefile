
ASM_CFILES = buffer.cpp asm\main.cpp asm\assembler.cpp
ASM_OUTPUT = asm.exe
CPU_CFILES = cpu\stack\stack.cpp cpu\stack\security.cpp buffer.cpp cpu\main.cpp cpu\cpu.cpp
CPU_OUTPUT = cpu.exe
DIS_ASM_CFILES = buffer.cpp dis_asm\main.cpp dis_asm\dis_asm.cpp
DIS_ASM_OUTPUT = dis_asm.exe

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
	@ g++ -o $(OUTPUT) $(CFLAGS) $(CFILES)
run:
	@ a.exe

.PHONY: clean
clean:
	@ rm -f $(OUTPUT)
.PHONY: clean_cmd
clean_cmd:
	@ cls

.PHONY: asm
asm:
	@ g++ -o $(ASM_OUTPUT) $(CFLAGS) $(ASM_CFILES)

.PHONY: cpu_def
cpu_def:
	@ g++ -E $(CPU_OUTPUT) $(CFLAGS) $(CPU_CFILES) >> defines.txt

.PHONY: asm_def
asm_def:
	@ g++ -E $(ASM_OUTPUT) $(CFLAGS) $(ASM_CFILES) >> defines.txt

.PHONY: cpu
cpu:
	@ g++ -o $(CPU_OUTPUT) $(CFLAGS) $(CPU_CFILES)

.PHONY: run_asm
run_asm:
	@ $(ASM_OUTPUT) output.txt input.txt

.PHONY: run_cpu
run_cpu:
	@ cpu.exe output.txt

.PHONY: dis_asm
dis_asm:
	@ g++ -o $(DIS_ASM_OUTPUT) $(CFLAGS) $(DIS_ASM_CFILES)

.PHONY: run_dis_asm
run_dis_asm:
	@ dis_asm.exe output.txt
