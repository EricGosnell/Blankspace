#include "blankspace.h"

/* ------------------------------------------------------------------------- *
 * Stack Manipulation (IMP: [Space])                                         *
 * ------------------------------------------------------------------------- */
/*!
 * @brief Push given number onto the stack
 * @param [in] e  A number you want to push onto the stack
 */
void stack_push(WsInt e) {
    assert(stack_idx < LENGTHOF(stack));
    stack[stack_idx++] = e;
}


/*!
 * @brief Pop out one element from the top of the stack
 * @return  An element of the top of the stack
 */
WsInt stack_pop(void) {
    assert(stack_idx > 0);
    return stack[--stack_idx];
}


/*!
 * @brief Copy the nth item on the stack onto the top of the stack
 */
void stack_dup_n(size_t n) {
    assert(n < stack_idx && stack_idx < LENGTHOF(stack) - 1);
    stack[stack_idx] = stack[stack_idx - (n + 1)];
    stack_idx++;
}


/*!
 * @brief Slide n items off the stack, keeping the top item
 * @param [in] n  The number of items you want to slide off the stack
 */
void stack_slide(size_t n) {
    assert(stack_idx > n);
    stack[stack_idx - (n + 1)] = stack[stack_idx - 1];
    stack_idx -= n;
}


/*!
 * @brief Swap the top two items on the stack
 */
void stack_swap(void) {
    assert(stack_idx > 1);
    SWAP(int, &stack[stack_idx - 1], &stack[stack_idx - 2]);
}


/*!
 * @brief Read blankspace-source code characters and push into given array.
 * @param [in,out] fp      File pointer to the blankspace source code
 * @param [out]    code    The array you want to store the source code
 * @param [in]     length  Max size of given array of code
 * @return Status-code
 */
int read_file(FILE *fp, char *code, size_t length) {
    int ch;
    size_t cnt = 0;
    for (; (ch = fgetc(fp)) != EOF; cnt++) {
        if (cnt > length) {
            fprintf(stderr, "Buffer overflow!\n");
            return FALSE;
        }
        switch (ch) {
            case ' ':
            case '\n':
            case '\t':
              *code++ = (char) ch;
            break;
        }
    }
    return TRUE;
}
