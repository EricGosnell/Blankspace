#include "blankspace.h"

/* ------------------------------------------------------------------------- *
 * Blankspace translator                                                     *
 * ------------------------------------------------------------------------- */
/*!
 * @brief Translate blankspace source code into C source code
 * @param [in,out] fp    output file pointer
 * @param [in]     code  Pointer to Blankspace source code buffer
 * @return Status-code
 */
int translate(FILE *fp, const char *code) {
  print_code_header(fp);
  for (; *code != '\0'; code++) {
    switch (*code) {
      case ' ':   /* Stack Manipulation */
        print_stack_code(fp, &code);
        break;
      case '\t':  /* Arithmetic, Heap Access or I/O */
        switch (*++code) {
          case ' ':  /* Arithmetic */
            print_arith_code(fp, &code);
            break;
          case '\t':  /* Heap Access */
            print_heap_code(fp, &code);
            break;
          case '\n':  /* I/O */
            print_io_code(fp, &code);
            break;
        }
        break;
      case '\n':  /* Flow Control */
        print_flow_code(fp, &code);
        break;
    }
  }
  print_code_footer(fp);
  return TRUE;
}


/*!
 * @brief Print C source code about stack manipulation
 * @param [in,out] fp    output file pointer
 * @param [in]     code  Pointer to Blankspace source code pointer
 */
void print_stack_code(FILE *fp, const char **code_ptr) {
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      fprintf(fp, INDENT_STR "push(%d);\n", read_nstr(&code));
      break;
    case '\t':
      switch (*++code) {
        case ' ':
          fprintf(fp, INDENT_STR "dup_n(%d);\n", read_nstr(&code));
          break;
        case '\t':
          fputs("Undefined Stack manipulation command is detected: [S][TT]\n", stderr);
          break;
        case '\n':
          fprintf(fp, INDENT_STR "slide(%d);\n", read_nstr(&code));
          break;
      }
      break;
    case '\n':
      switch (*++code) {
        case ' ':
          fputs(INDENT_STR "dup_n(0);\n", fp);
          break;
        case '\t':
          fputs(INDENT_STR "swap();\n", fp);
          break;
        case '\n':
          fputs(INDENT_STR "pop();\n", fp);
          break;
      }
      break;
  }
  *code_ptr = code;
}


/*!
 * @brief Print C source code about arithmetic
 * @param [in,out] fp    output file pointer
 * @param [in]     code  Pointer to Blankspace source code pointer
 */
void print_arith_code(FILE *fp, const char **code_ptr) {
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      switch (*++code) {
        case ' ':
          fputs(INDENT_STR "arith_add();\n", fp);
          break;
        case '\t':
          fputs(INDENT_STR "arith_sub();\n", fp);
          break;
        case '\n':
          fputs(INDENT_STR "arith_mul();\n", fp);
          break;
      }
      break;
    case '\t':
      switch (*++code) {
        case ' ':
          fputs(INDENT_STR "arith_div();\n", fp);
          break;
        case '\t':
          fputs(INDENT_STR "arith_mod();\n", fp);
          break;
        case '\n':
          fputs("Undefined arithmetic command is detected: [TS][TN]\n", stderr);
          break;
      }
      break;
    case '\n':
      fputs("Undefined arithmetic command is detected: [TS][N]\n", stderr);
      break;
  }
  *code_ptr = code;
}


/*!
 * @brief Print C source code about heap access
 * @param [in,out] fp    output file pointer
 * @param [in]     code  Pointer to Blankspace source code pointer
 */
void print_heap_code(FILE *fp, const char **code_ptr) {
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      fputs(INDENT_STR "heap_store();\n", fp);
      break;
    case '\t':
      fputs(INDENT_STR "heap_read();\n", fp);
    break;
    case '\n':
      fputs("Undefined heap access command is detected: [TT][N]\n", stderr);
      break;
  }
  *code_ptr = code;
}


/*!
 * @brief Print C source code about flow control
 * @param [in,out] fp    output file pointer
 * @param [in]     code  Pointer to Blankspace source code pointer
 */
void print_flow_code(FILE *fp, const char **code_ptr) {
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      switch (*++code) {
        case ' ':
          fprintf(fp, "\n%s:\n", read_label(&code));
          break;
        case '\t':
          fprintf(fp,
              INDENT_STR "if (!setjmp(call_stack[call_stack_idx++])) {\n"
              INDENT_STR INDENT_STR "goto %s;\n"
              INDENT_STR "}\n",
              read_label(&code));
          break;
        case '\n':
          fprintf(fp, INDENT_STR "goto %s;\n", read_label(&code));
          break;
      }
      break;
    case '\t':
      switch (*++code) {
        case ' ':
          fprintf(fp,
              INDENT_STR "if (!pop()) {\n"
              INDENT_STR INDENT_STR "goto %s;\n"
              INDENT_STR "}\n",
              read_label(&code));
          break;
        case '\t':
          fprintf(fp,
              INDENT_STR "if (pop() < 0) {\n"
              INDENT_STR INDENT_STR "goto %s;\n"
              INDENT_STR "}\n",
              read_label(&code));
          break;
        case '\n':
          fputs(INDENT_STR "longjmp(call_stack[--call_stack_idx], 1);\n", fp);
          break;
      }
      break;
    case '\n':
      if (*++code == '\n') {
        fputs(INDENT_STR "exit(EXIT_SUCCESS);\n", fp);
      } else {
        fputs("Undefined flow control command is detected: [N][S/T]\n", stderr);
      }
      break;
  }
  *code_ptr = code;
}


/*!
 * @brief Print C source code about I/O
 * @param [in,out] fp    output file pointer
 * @param [in]     code  Pointer to Blankspace source code pointer
 */
void print_io_code(FILE *fp, const char **code_ptr) {
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      switch (*++code) {
        case ' ':
          fputs(INDENT_STR "putchar(pop());\n", fp);
          break;
        case '\t':
          fputs(INDENT_STR "printf(\"%d\", pop());\n", fp);
          break;
        case '\n':
          fputs("Undefined I/O command is detected: [TN][SN]\n", stderr);
          break;
      }
      break;
    case '\t':
      switch (*++code) {
        case ' ':
          fputs(
              INDENT_STR "fflush(stdout);\n"
              INDENT_STR "heap[pop()] = getchar();\n",
              fp);
          break;
        case '\t':
          fputs(
              INDENT_STR "fflush(stdout);\n"
              INDENT_STR "scanf(\"%d\", &heap[pop()]);\n",
              fp);
          break;
        case '\n':
          fputs("Undefined I/O command is detected: [TN][TN]\n", stderr);
          break;
      }
      break;
    case '\n':
      fputs("Undefined I/O command is detected: [TN][N]\n", stderr);
      break;
  }
  *code_ptr = code;
}


/*!
 * @brief Print the header of translated C-source code
 * @param [in,out] fp  Output file pointer
 */
void print_code_header(FILE *fp) {
  fputs(
      "#include <assert.h>\n"
      "#include <setjmp.h>\n"
      "#include <stdio.h>\n"
      "#include <stdlib.h>\n\n", fp);
  fputs(
      "#ifndef __cplusplus\n"
      "#  if defined(_MSC_VER)\n"
      "#    define inline      __inline\n"
      "#    define __inline__  __inline\n"
      "#  elif !defined(__GNUC__) && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)\n"
      "#    define inline\n"
      "#    define __inline\n"
      "#  endif\n"
      "#endif\n\n", fp);
  fprintf(fp,
      "#define STACK_SIZE %d\n"
      "#define HEAP_SIZE %d\n"
      "#define CALL_STACK_SIZE %d\n\n"
      "#define LENGTHOF(array) (sizeof(array) / sizeof((array)[0]))\n"
      "#define SWAP(type, a, b) \\\n"
      INDENT_STR "do { \\\n"
      INDENT_STR INDENT_STR "type __tmp_swap_var__ = *(a); \\\n"
      INDENT_STR INDENT_STR "*(a) = *(b); \\\n"
      INDENT_STR INDENT_STR "*(b) = __tmp_swap_var__; \\\n"
      INDENT_STR "} while (0)\n\n",
      STACK_SIZE, HEAP_SIZE, CALL_STACK_SIZE);
  fputs(
      "inline static int  pop(void);\n"
      "inline static void push(int e);\n"
      "inline static void dup_n(size_t n);\n"
      "inline static void slide(size_t n);\n"
      "inline static void swap(void);\n", fp);
  fputs(
      "inline static void arith_add(void);\n"
      "inline static void arith_sub(void);\n"
      "inline static void arith_mul(void);\n"
      "inline static void arith_div(void);\n"
      "inline static void arith_mod(void);\n", fp);
  fputs(
      "inline static void heap_store(void);\n"
      "inline static void heap_read(void);\n\n", fp);
  fputs(
      "static int stack[STACK_SIZE];\n"
      "static int heap[HEAP_SIZE];\n"
      "static jmp_buf call_stack[CALL_STACK_SIZE];\n"
      "static size_t stack_idx = 0;\n"
      "static size_t call_stack_idx = 0;\n\n\n", fp);
  fputs(
      "int main(void)\n"
      "{\n", fp);
}
/*!
 * @brief Print the footer of translated C-source code
 * @param [in,out] fp  Output file pointer
 */
void print_code_footer(FILE *fp) {
  fputs(
      "\n"
      INDENT_STR "return EXIT_SUCCESS;\n"
      "}\n\n\n", fp);
  fputs(
      "inline static int pop(void)\n"
      "{\n"
      INDENT_STR "assert(stack_idx < LENGTHOF(stack));\n"
      INDENT_STR "return stack[--stack_idx];\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void push(int e)\n"
      "{\n"
      INDENT_STR "assert(stack_idx < LENGTHOF(stack));\n"
      INDENT_STR "stack[stack_idx++] = e;\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void dup_n(size_t n)\n"
      "{\n"
      INDENT_STR "assert(n < stack_idx && stack_idx < LENGTHOF(stack) - 1);\n"
      INDENT_STR "stack[stack_idx] = stack[stack_idx - (n + 1)];\n"
      INDENT_STR "stack_idx++;\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void slide(size_t n)\n"
      "{\n"
      INDENT_STR "assert(stack_idx > n);\n"
      INDENT_STR "stack[stack_idx - (n + 1)] = stack[stack_idx - 1];\n"
      INDENT_STR "stack_idx -= n;\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void swap(void)\n"
      "{\n"
      INDENT_STR "assert(stack_idx > 1);\n"
      INDENT_STR "SWAP(int, &stack[stack_idx - 1], &stack[stack_idx - 2]);\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void arith_add(void)\n"
      "{\n"
      INDENT_STR "assert(stack_idx > 1);\n"
      INDENT_STR "stack_idx--;\n"
      INDENT_STR "stack[stack_idx - 1] += stack[stack_idx];\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void arith_sub(void)\n"
      "{\n"
      INDENT_STR "assert(stack_idx > 1);\n"
      INDENT_STR "stack_idx--;\n"
      INDENT_STR "stack[stack_idx - 1] -= stack[stack_idx];\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void arith_mul(void)\n"
      "{\n"
      INDENT_STR "assert(stack_idx > 1);\n"
      INDENT_STR "stack_idx--;\n"
      INDENT_STR "stack[stack_idx - 1] *= stack[stack_idx];\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void arith_div(void)\n"
      "{\n"
      INDENT_STR "assert(stack_idx > 1);\n"
      INDENT_STR "stack_idx--;\n"
      INDENT_STR "assert(stack[stack_idx] != 0);\n"
      INDENT_STR "stack[stack_idx - 1] /= stack[stack_idx];\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void arith_mod(void)\n"
      "{\n"
      INDENT_STR "assert(stack_idx > 1);\n"
      INDENT_STR "stack_idx--;\n"
      INDENT_STR "assert(stack[stack_idx] != 0);\n"
      INDENT_STR "stack[stack_idx - 1] %= stack[stack_idx];\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void heap_store(void)\n"
      "{\n"
      INDENT_STR "int value = pop();\n"
      INDENT_STR "int addr  = pop();\n"
      INDENT_STR "assert(0 <= addr && addr < (int) LENGTHOF(heap));\n"
      INDENT_STR "heap[addr] = value;\n"
      "}\n\n\n", fp);
  fputs(
      "inline static void heap_read(void)\n"
      "{\n"
      INDENT_STR "int addr = pop();\n"
      INDENT_STR "assert(0 <= addr && addr < (int) LENGTHOF(heap));\n"
      INDENT_STR "push(heap[addr]);\n"
      "}\n", fp);
}


/*!
 * @brief Read integer and seek program pointer.
 * @param [in,out] code_ptr  Program pointer
 * @return  An integer parsed from source code
 */
int read_nstr(const char **code_ptr) {
  const char *code = *code_ptr;
  int is_positive = 1;
  int sum = 0;
  switch (*++code) {
    case '\t':
      is_positive = 0;
      break;
    case '\n':
      *code_ptr = code;
      return 0;
  }
  while (*++code != '\n') {
    sum <<= 1;
    if (*code == '\t') {
      sum++;
    }
  }
  *code_ptr = code;
  return is_positive ? sum : -sum;
}


/*!
 * @brief Read label and convert it into strings.
 * @param [in,out] code_ptr  Program pointer
 * @return  Converted label
 */
char* read_label(const char **code_ptr) {
  static char label_name[MAX_LABEL_LENGTH];
  char *ptr = label_name;
  const char *code = *code_ptr;
  char ch;

  while ((ch = *++code) != '\n') {
    switch (ch) {
      case ' ':
        *ptr++ = 'S';
        break;
      case '\t':
        *ptr++ = 'T';
        break;
    }
  }
  *ptr = '\0';
  *code_ptr = code;
  return label_name;
}


/*!
 * @brief Show byte code in hexadecimal
 * @param [in] bytecode       Blankspace byte code
 * @param [in] bytecode_size  Size of blankspace byte code
 */
void show_bytecode(const unsigned char *bytecode, size_t bytecode_size) {
  size_t i, j;
  size_t quot = bytecode_size / 16;
  size_t rem  = bytecode_size % 16;
  int addr_cnt = 0;

  puts("ADDRESS  00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
  for (i = 0; i < quot; i++) {
    printf("0x%04x: ", addr_cnt);
    addr_cnt += 16;
    for (j = 0; j < 16; j++) {
      printf(" %02x", *bytecode++);
    }
    puts("");
  }
  printf("0x%04x: ", addr_cnt);
  for (i = 0; i < rem; i++) {
    printf(" %02x", *bytecode++);
  }
  puts("");
}


/*!
 * @brief Show the byte code in mnemonic format.
 * @param [in] fp             Output file pointer
 * @param [in] bytecode       Blankspace byte code
 * @param [in] bytecode_size  Size of blankspace byte code
 */
void show_mnemonic(FILE *fp, const unsigned char *bytecode, size_t bytecode_size) {
  const unsigned char *end;
  const unsigned char *base = bytecode;
  for (end = bytecode + bytecode_size; bytecode < end; bytecode++) {
    fprintf(fp, "%04d: ", (int) ADDR_DIFF(bytecode, base));
    switch (*bytecode) {
      case STACK_PUSH:
        bytecode++;
        fprintf(fp, "STACK_PUSH %d\n", *((const WsInt *) bytecode));
        bytecode += sizeof(WsInt) - 1;
        break;
      case STACK_DUP_N:
        bytecode++;
        fprintf(fp, "STACK_DUP_N %d\n", *((const WsInt *) bytecode));
        bytecode += sizeof(WsInt) - 1;
        break;
      case STACK_DUP:
        fprintf(fp, "STACK_DUP\n");
        break;
      case STACK_SLIDE:
        bytecode++;
        fprintf(fp, "STACK_SLIDE %d\n", *((const WsInt *) bytecode));
        bytecode += sizeof(WsInt) - 1;
        break;
      case STACK_SWAP:
        fputs("STACK_SWAP\n", fp);
        break;
      case STACK_DISCARD:
        fputs("STACK_POP\n", fp);
        break;
      case ARITH_ADD:
        fputs("ARITH_ADD\n", fp);
        break;
      case ARITH_SUB:
        fputs("ARITH_SUB\n", fp);
        break;
      case ARITH_MUL:
        fputs("ARITH_MUL\n", fp);
        break;
      case ARITH_DIV:
        fputs("ARITH_DIV\n", fp);
        break;
      case ARITH_MOD:
        fputs("ARITH_MOD\n", fp);
        break;
      case HEAP_STORE:
        fputs("HEAP_STORE\n", fp);
        break;
      case HEAP_LOAD:
        fputs("HEAP_LOAD\n", fp);
        break;
      case FLOW_GOSUB:
        bytecode++;
        fprintf(fp, "FLOW_GOSUB %u\n", *((const WsAddrInt *) bytecode));
        bytecode += sizeof(WsAddrInt) - 1;
        break;
      case FLOW_JUMP:
        bytecode++;
        fprintf(fp, "FLOW_JUMP %u\n", *((const WsAddrInt *) bytecode));
        bytecode += sizeof(WsAddrInt) - 1;
        break;
      case FLOW_BEZ:
        bytecode++;
        fprintf(fp, "FLOW_BEZ %u\n", *((const WsAddrInt *) bytecode));
        bytecode += sizeof(WsAddrInt) - 1;
        break;
      case FLOW_BLTZ:
        bytecode++;
        fprintf(fp, "FLOW_BLTZ %u\n", *((const WsAddrInt *) bytecode));
        bytecode += sizeof(WsAddrInt) - 1;
        break;
      case FLOW_HALT:
        fputs("FLOW_HALT\n", fp);
        break;
      case FLOW_ENDSUB:
        fputs("FLOW_ENDSUB\n", fp);
        break;
      case IO_PUT_CHAR:
        fputs("IO_PUT_CHAR\n", fp);
        break;
      case IO_PUT_NUM:
        fputs("IO_PUT_NUM\n", fp);
        break;
      case IO_READ_CHAR:
        fputs("IO_READ_CHAR\n", fp);
        break;
      case IO_READ_NUM:
        fputs("IO_READ_NUM\n", fp);
        break;
      default:
        fprintf(fp, "UNDEFINED_INSTRUCTION [0x%02x]\n", *bytecode);
    }
  }
}


/*!
 * @brief Visualize the source code using S and T instead of space or tab.
 * @param [in,out] fp    Output file pointer
 * @param [in]     code  Blankspace source code
 */
void filter(FILE *fp, const char *code) {
  for (; *code != '\0'; code++) {
    switch (*code) {
      case ' ':
        fputc('S', fp);
        break;
      case '\t':
        fputc('T', fp);
        break;
      case '\n':
        fputc('\n', fp);
        break;
    }
  }
}
