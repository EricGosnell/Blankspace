#include "blankspace.h"

/* ------------------------------------------------------------------------- *
 * Interpreter                                                               *
 * ------------------------------------------------------------------------- */
/*!
 * @brief Execute blankspace
 * @param [in] bytecode  Bytecode of blankspace
 */
void execute(const unsigned char *bytecode) {
  static int heap[HEAP_SIZE] = {0};
  static size_t call_stack[CALL_STACK_SIZE] = {0};
  size_t call_stack_idx = 0;
  const unsigned char *base = bytecode;
  int a = 0, b = 0;
  for (; *bytecode; bytecode++) {
    switch (*bytecode) {
      case STACK_PUSH:
        bytecode++;
        stack_push(*((const WsInt *) bytecode));
        bytecode += sizeof(WsInt) - 1;
        break;
      case STACK_DUP_N:
        bytecode++;
        stack_dup_n((size_t) *((const WsInt *) bytecode));
        bytecode += sizeof(WsInt) - 1;
        break;
      case STACK_DUP:
        stack_dup_n(0);
        break;
      case STACK_SLIDE:
        bytecode++;
        stack_slide((size_t) *((const WsInt *) bytecode));
        bytecode += sizeof(WsInt) - 1;
        break;
      case STACK_SWAP:
        stack_swap();
        break;
      case STACK_DISCARD:
        stack_pop();
        break;
      case ARITH_ADD:
        a = stack_pop();
        b = stack_pop();
        stack_push(b + a);
        break;
      case ARITH_SUB:
        a = stack_pop();
        b = stack_pop();
        stack_push(b - a);
        break;
      case ARITH_MUL:
        a = stack_pop();
        b = stack_pop();
        stack_push(b * a);
        break;
      case ARITH_DIV:
        a = stack_pop();
        b = stack_pop();
        assert(b != 0);
        stack_push(b / a);
        break;
      case ARITH_MOD:
        a = stack_pop();
        b = stack_pop();
        assert(b != 0);
        stack_push(b % a);
        break;
      case HEAP_STORE:
        a = stack_pop();
        b = stack_pop();
        assert(0 <= b && b < (int) LENGTHOF(heap));
        heap[b] = a;
        break;
      case HEAP_LOAD:
        a = stack_pop();
        assert(0 <= a && a < (int) LENGTHOF(heap));
        stack_push(heap[a]);
        break;
      case FLOW_GOSUB:
        call_stack[call_stack_idx++] = (size_t) (ADDR_DIFF(bytecode, base)) + sizeof(WsAddrInt);
        bytecode++;
        bytecode = &base[*((const WsAddrInt *) bytecode)] - 1;
        break;
      case FLOW_JUMP:
        bytecode++;
        bytecode = &base[*((const WsAddrInt *) bytecode)] - 1;
        break;
      case FLOW_BEZ:
        if (!stack_pop()) {
          bytecode++;
          bytecode = &base[*((const WsAddrInt *) bytecode)] - 1;
        } else {
          bytecode += sizeof(WsAddrInt);
        }
        break;
      case FLOW_BLTZ:
        if (stack_pop() < 0) {
          bytecode++;
          bytecode = &base[*((const WsAddrInt *) bytecode)] - 1;
        } else {
          bytecode += sizeof(WsAddrInt);
        }
        break;
      case FLOW_ENDSUB:
        bytecode = &base[call_stack[--call_stack_idx]];
        break;
      case IO_PUT_CHAR:
        putchar(stack_pop());
        break;
      case IO_PUT_NUM:
        printf("%d", stack_pop());
        break;
      case IO_READ_CHAR:
        a = stack_pop();
        assert(0 <= a && a < (int) LENGTHOF(heap));
        fflush(stdout);
        heap[a] = getchar();
        break;
      case IO_READ_NUM:
        a = stack_pop();
        assert(0 <= a && a < (int) LENGTHOF(heap));
        fflush(stdout);
        scanf("%d", &heap[a]);
        break;
      case FLOW_HALT:
        printf("HALT\n");
        break;
      default:
        fprintf(stderr, "Undefined instruction is detected [%02x]\n", *bytecode);
    }
  }
}


/*!
 * @brief Compile blankspace source code into bytecode
 * @param [out] bytecode  Bytecode buffer
 * @param [in]  code      Brainfuck source code
 */
void compile(unsigned char *bytecode, size_t *bytecode_size, const char *code) {
  unsigned char *base = bytecode;
  for (; *code != '\0'; code++) {
    switch (*code) {
      case ' ':   /* Stack Manipulation */
        gen_stack_code(&bytecode, &code);
        break;
      case '\t':  /* Arithmetic, Heap Access or I/O */
        switch (*++code) {
          case ' ':  /* Arithmetic */
            gen_arith_code(&bytecode, &code);
            break;
          case '\t':  /* Heap Access */
            gen_heap_code(&bytecode, &code);
            break;
          case '\n':  /* I/O */
            gen_io_code(&bytecode, &code);
            break;
        }
        break;
      case '\n':  /* Flow Control */
        gen_flow_code(&bytecode, &code, base);
        break;
    }
  }
  *bytecode_size = (size_t) ADDR_DIFF(bytecode, base);
  free_label_info_list(label_info_list);
}


/*!
 * @brief Generate bytecode about stack manipulation
 * @param [out]    bytecode_ptr  Pointer to bytecode buffer
 * @param [in,out] code_ptr      pointer to blankspace source code
 */
void gen_stack_code(unsigned char **bytecode_ptr, const char **code_ptr) {
  unsigned char *bytecode = *bytecode_ptr;
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      *bytecode++ = STACK_PUSH;
      *((WsInt *) bytecode) = read_nstr(&code);
      bytecode += sizeof(WsInt);
      break;
    case '\t':
      switch (*++code) {
        case ' ':
          *bytecode++ = STACK_DUP_N;
          *((WsInt *) bytecode) = read_nstr(&code);
          bytecode += sizeof(WsInt);
          break;
        case '\t':
          fputs("Undefined Stack manipulation command is detected: [S][TT]\n", stderr);
          break;
        case '\n':
          *bytecode++ = STACK_SLIDE;
          *((WsInt *) bytecode) = read_nstr(&code);
          bytecode += sizeof(WsInt);
          break;
      }
      break;
    case '\n':
      switch (*++code) {
        case ' ':
          *bytecode++ = STACK_DUP_N;
          *((WsInt *) bytecode) = 0;
          bytecode += sizeof(WsInt);
          break;
        case '\t':
          *bytecode++ = STACK_SWAP;
          break;
        case '\n':
          *bytecode++ = STACK_DISCARD;
          break;
      }
      break;
  }
  *bytecode_ptr = bytecode;
  *code_ptr = code;
}


/*!
 * @brief Generate bytecode about arithmetic
 * @param [out]    bytecode_ptr  Pointer to bytecode buffer
 * @param [in,out] code_ptr      pointer to blankspace source code
 */
void gen_arith_code(unsigned char **bytecode_ptr, const char **code_ptr) {
  unsigned char *bytecode = *bytecode_ptr;
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      switch (*++code) {
        case ' ':
          *bytecode++ = ARITH_ADD;
          break;
        case '\t':
          *bytecode++ = ARITH_SUB;
          break;
        case '\n':
          *bytecode++ = ARITH_MUL;
          break;
      }
      break;
    case '\t':
      switch (*++code) {
        case ' ':
          *bytecode++ = ARITH_DIV;
          break;
        case '\t':
          *bytecode++ = ARITH_MOD;
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
  *bytecode_ptr = bytecode;
  *code_ptr = code;
}


/*!
 * @brief Generate bytecode about heap access
 * @param [out]    bytecode_ptr  Pointer to bytecode buffer
 * @param [in,out] code_ptr      pointer to blankspace source code
 */
void gen_heap_code(unsigned char **bytecode_ptr, const char **code_ptr) {
  unsigned char *bytecode = *bytecode_ptr;
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      *bytecode++ = HEAP_STORE;
      break;
    case '\t':
      *bytecode++ = HEAP_LOAD;
      break;
    case '\n':
      fputs("Undefined heap access command is detected: [TT][N]\n", stderr);
      break;
  }
  *bytecode_ptr = bytecode;
  *code_ptr = code;
}


/*!
 * @brief Generate bytecode about flow control
 * @param [out]    bytecode_ptr  Pointer to bytecode buffer
 * @param [in,out] code_ptr      pointer to blankspace source code
 * @param [in]     base          Base address of the bytecode buffer
 */
void gen_flow_code(unsigned char **bytecode_ptr, const char **code_ptr, unsigned char *base) {
  unsigned char *bytecode = *bytecode_ptr;
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      switch (*++code) {
        case ' ':
          process_label_define(&bytecode, &code, base);
          break;
        case '\t':
          *bytecode++ = FLOW_GOSUB;
          process_label_jump(&bytecode, &code, base);
          break;
        case '\n':
          *bytecode++ = FLOW_JUMP;
          process_label_jump(&bytecode, &code, base);
          break;
      }
      break;
    case '\t':
      switch (*++code) {
        case ' ':
          *bytecode++ = FLOW_BEZ;
          process_label_jump(&bytecode, &code, base);
          break;
        case '\t':
          *bytecode++ = FLOW_BLTZ;
          process_label_jump(&bytecode, &code, base);
          break;
        case '\n':
          *bytecode++ = FLOW_ENDSUB;
          break;
      }
      break;
    case '\n':
      if (*++code == '\n') {
        *bytecode++ = FLOW_HALT;
      } else {
        fputs("Undefined flow control command is detected: [N][S/T]\n", stderr);
      }
      break;
  }
  *bytecode_ptr = bytecode;
  *code_ptr = code;
}


/*!
 * @brief Generate bytecode about I/O
 * @param [out]    bytecode_ptr  Pointer to bytecode buffer
 * @param [in,out] code_ptr      pointer to blankspace source code
 */
void gen_io_code(unsigned char **bytecode_ptr, const char **code_ptr) {
  unsigned char *bytecode = *bytecode_ptr;
  const char *code = *code_ptr;
  switch (*++code) {
    case ' ':
      switch (*++code) {
        case ' ':
          *bytecode++ = IO_PUT_CHAR;
          break;
        case '\t':
          *bytecode++ = IO_PUT_NUM;
          break;
        case '\n':
          fputs("Undefined I/O command is detected: [TN][SN]\n", stderr);
          break;
      }
      break;
    case '\t':
      switch (*++code) {
        case ' ':
          *bytecode++ = IO_READ_CHAR;
          break;
        case '\t':
          *bytecode++ = IO_READ_NUM;
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
  *bytecode_ptr = bytecode;
  *code_ptr = code;
}


/*!
 * @brief Check given label is already defined or not
 *
 * If label is already defined, return the label information
 * @param [in] label  Label you want to check
 * @return  Label information
 */
__attribute__((pure))
LabelInfo* search_label(const char *label) {
  size_t i;
  for (i = 0; i < n_label_info; i++) {
    if (!strcmp(label, label_info_list[i]->label)) {
      return label_info_list[i];
    }
  }
  return NULL;
}


/*!
 * @brief Write where to jump to the bytecode
 * @param [out]    bytecode_ptr  Pointer to bytecode buffer
 * @param [in,out] code_ptr      pointer to blankspace source code
 * @param [in]     base          Base address of the bytecode buffer
 */
void process_label_define(unsigned char **bytecode_ptr, const char **code_ptr, unsigned char *base) {
  const char *code = *code_ptr;
  unsigned char *bytecode = *bytecode_ptr;
  char *label = read_label(&code);
  LabelInfo *label_info = search_label(label);

  if (label_info == NULL) {
    add_label(label, (WsAddrInt) ADDR_DIFF(bytecode, base));
  } else {
    if (label_info->addr == UNDEF_ADDR) {
      int i;
      for (i = 0; i < label_info->n_undef; i++) {
        *((WsAddrInt *) &base[label_info->undef_list[i]]) = (WsAddrInt) ADDR_DIFF(bytecode, base);
      }
      label_info->addr = (WsAddrInt) ADDR_DIFF(bytecode, base);
      free(label_info->undef_list);
      label_info->undef_list = NULL;
    } else {
      fputs("Duplicate label definition\n", stderr);
    }
  }
  *code_ptr = code;
  *bytecode_ptr = bytecode;
}


/*!
 * @brief Write where to jump to the bytecode
 *
 * If label is not defined yet, write it after label is defined.
 * @param [out]    bytecode_ptr  Pointer to bytecode buffer
 * @param [in,out] code_ptr      pointer to blankspace source code
 * @param [in]     base          Base address of the bytecode buffer
 */
void process_label_jump(unsigned char **bytecode_ptr, const char **code_ptr, unsigned char *base) {
  const char *code = *code_ptr;
  unsigned char *bytecode = *bytecode_ptr;
  char *label = read_label(&code);
  LabelInfo *label_info = search_label(label);

  if (label_info == NULL) {
    add_undef_label(label, (WsAddrInt) ADDR_DIFF(bytecode, base));
  } else if (label_info->addr == UNDEF_ADDR) {
    label_info->undef_list[label_info->n_undef++] = (WsAddrInt) ADDR_DIFF(bytecode, base);
  } else {
    *((WsAddrInt *) bytecode) = label_info->addr;
  }
  bytecode += sizeof(WsAddrInt);
  *code_ptr = code;
  *bytecode_ptr = bytecode;
}


/*!
 * @brief Add label information to the label list
 * @param [in] _label  Label name
 * @param [in] addr    Label position
 */
void add_label(const char *_label, WsAddrInt addr) {
  char *label = (char *) calloc(strlen(_label) + 1, sizeof(char));
  LabelInfo *label_info = (LabelInfo *) calloc(1, sizeof(LabelInfo));

  if (label == NULL || label_info == NULL) {
    fprintf(stderr, "Failed to allocate heap for label\n");
    exit(EXIT_FAILURE);
  }
  strcpy(label, _label);

  free(label_info->undef_list);
  label_info->undef_list = NULL;
  label_info->label = label;
  label_info->addr = addr;
  label_info->n_undef = 0;
  label_info_list[n_label_info++] = label_info;
}


/*!
 * @brief Add unseen/undefined label to the label list
 * @param [in] _label  Label name
 * @param [in] pos     The position given label was found
 */
void add_undef_label(const char *_label, WsAddrInt pos) {
  char *label = (char *) calloc(strlen(_label) + 1, sizeof(char));
  LabelInfo *label_info = (LabelInfo *) calloc(1, sizeof(LabelInfo));
  label_info->undef_list = (WsAddrInt *) calloc(UNDEF_LIST_SIZE, sizeof(WsAddrInt));

  if (label == NULL || label_info == NULL || label_info->undef_list == NULL) {
    fprintf(stderr, "Failed to allocate heap for label\n");
    exit(EXIT_FAILURE);
  }
  strcpy(label, _label);

  label_info->undef_list[0] = pos;
  label_info->label = label;
  label_info->addr = UNDEF_ADDR;
  label_info->n_undef = 1;
  label_info_list[n_label_info++] = label_info;
}


/*!
 * @brief Free label informations
 * @param [in] label_info_list  Label list
 */
void free_label_info_list(LabelInfo *label_info_list[]) {
  size_t i = 0;
  for (i = 0; i < n_label_info; i++) {
    free(label_info_list[i]->label);
    free(label_info_list[i]->undef_list);
    free(label_info_list[i]);
  }
}
