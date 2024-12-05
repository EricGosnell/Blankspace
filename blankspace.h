#pragma once
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#if defined(_MSC_VER) && defined(_DEBUG)
#  include <msvcdbg.h>
#endif

#ifndef MAX_SOURCE_SIZE
#  define MAX_SOURCE_SIZE  65536
#endif
#ifndef MAX_BYTECODE_SIZE
#  define MAX_BYTECODE_SIZE  1048576
#endif
#ifndef MAX_LABEL_LENGTH
#  define MAX_LABEL_LENGTH  65536
#endif
#ifndef MAX_N_LABEL
#  define MAX_N_LABEL  1024
#endif
#ifndef UNDEF_LIST_SIZE
#  define UNDEF_LIST_SIZE  256
#endif
#ifndef STACK_SIZE
#  define STACK_SIZE  65536
#endif
#ifndef HEAP_SIZE
#  define HEAP_SIZE  65536
#endif
#ifndef CALL_STACK_SIZE
#  define CALL_STACK_SIZE  65536
#endif
#ifndef WS_INT
#  define WS_INT  int
#endif
#ifndef WS_ADDR_INT
#  define WS_ADDR_INT  unsigned int
#endif
#ifndef INDENT_STR
#  define INDENT_STR  "  "
#endif

#define TRUE  1
#define FALSE 0
#define UNDEF_ADDR  ((WsAddrInt) -1)
#define LENGTHOF(array)  (sizeof(array) / sizeof((array)[0]))
#define ADDR_DIFF(a, b) \
  ((const unsigned char *) (a) - (const unsigned char *) (b))
#define SWAP(type, a, b) \
  do { \
    type __tmp_swap_var__ = *(a); \
    *(a) = *(b); \
    *(b) = __tmp_swap_var__; \
  } while (0)


enum OpCode {
  FLOW_HALT = 0x00,
  STACK_PUSH, STACK_DUP_N, STACK_DUP, STACK_SLIDE, STACK_SWAP, STACK_DISCARD,
  ARITH_ADD, ARITH_SUB, ARITH_MUL, ARITH_DIV, ARITH_MOD,
  HEAP_STORE, HEAP_LOAD,
  FLOW_LABEL, FLOW_GOSUB, FLOW_JUMP, FLOW_BEZ, FLOW_BLTZ, FLOW_ENDSUB,
  IO_PUT_CHAR, IO_PUT_NUM, IO_READ_CHAR, IO_READ_NUM
};


typedef WS_INT  WsInt;
typedef WS_ADDR_INT  WsAddrInt;

typedef struct {
  const char *in_filename;
  const char *out_filename;
  int mode;
} Param;

typedef struct {
  WsAddrInt  addr;
  int        n_undef;
  char      *label;
  WsAddrInt *undef_list;
} LabelInfo;


 void
parse_arguments(Param *param, int argc, char *argv[]);

 void
show_usage(const char *progname);

 int
read_file(FILE *fp, char *code, size_t length);


 void
execute(const unsigned char *bytecode);

 void
compile(unsigned char *bytecode, size_t *bytecode_size, const char *code);

 void
gen_stack_code(unsigned char **bytecode_ptr, const char **code_ptr);

 void
gen_arith_code(unsigned char **bytecode_ptr, const char **code_ptr);

 void
gen_heap_code(unsigned char **bytecode_ptr, const char **code_ptr);

 void
gen_io_code(unsigned char **bytecode_ptr, const char **code_ptr);

 void
gen_flow_code(unsigned char **bytecode_ptr, const char **code_ptr, unsigned char *base);


 void
process_label_define(unsigned char **bytecode_ptr, const char **code_ptr, unsigned char *base);

 void
process_label_jump(unsigned char **bytecode_ptr, const char **code_ptr, unsigned char *base);

 LabelInfo *
search_label(const char *label);

 void
add_label(const char *_label, WsAddrInt addr);

 void
add_undef_label(const char *_label, WsAddrInt pos);

 void
free_label_info_list(LabelInfo *label_info_list[]);


 void
stack_push(WsInt e);

 WsInt
stack_pop(void);

 void
stack_dup_n(size_t n);

 void
stack_slide(size_t n);

 void
stack_swap(void);


 int
read_nstr(const char **code_ptr);

 char *
read_label(const char **code_ptr);


 int
translate(FILE *fp, const char *code);

 void
print_stack_code(FILE *fp, const char **code_ptr);

 void
print_arith_code(FILE *fp, const char **code_ptr);

 void
print_heap_code(FILE *fp, const char **code_ptr);

 void
print_io_code(FILE *fp, const char **code_ptr);

 void
print_flow_code(FILE *fp, const char **code_ptr);

 void
print_code_header(FILE *fp);

 void
print_code_footer(FILE *fp);


 void
show_bytecode(const unsigned char *bytecode, size_t bytecode_size);

 void
show_mnemonic(FILE *fp, const unsigned char *bytecode, size_t bytecode_size);

 void
filter(FILE *fp, const char *code);

extern WsInt stack[STACK_SIZE];
extern size_t stack_idx;

extern LabelInfo *label_info_list[MAX_N_LABEL];
extern size_t n_label_info;
