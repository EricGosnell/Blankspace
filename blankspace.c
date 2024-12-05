/*!
 * @file blankspace.c
 * @brief An interpreter and C-translator of Blankspace
 * @author koturn
 */

#include "blankspace.h"

WsInt stack[STACK_SIZE] = {0};
size_t stack_idx = 0;

LabelInfo *label_info_list[MAX_N_LABEL] = {NULL};
size_t n_label_info = 0;


/*!
 * @brief Entry point of this program
 * @param [in] argc  The number of argument (include this program name)
 * @param [in] argv  The array off argument strings
 * @return  Status-code
 */
int main(int argc, char *argv[]) {
  static char code[MAX_SOURCE_SIZE] = {0};
  static unsigned char bytecode[MAX_BYTECODE_SIZE] = {0};
  Param param = {NULL, NULL, '*'};
  FILE *ifp, *ofp;
  size_t bytecode_size;

  parse_arguments(&param, argc, argv);
  if (param.in_filename == NULL) {
    fprintf(stderr, "Invalid arguments\n");
    return EXIT_FAILURE;
  }
  if (!strcmp(param.in_filename, "-")) {
    ifp = stdin;
  } else if ((ifp = fopen(param.in_filename, "r")) == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", argv[1]);
    return EXIT_FAILURE;
  }
  if (!read_file(ifp, code, LENGTHOF(code))) {
    return EXIT_FAILURE;
  }
  if (ifp != stdin) {
    fclose(ifp);
  }

  switch (param.mode) {
    case 'b':
      compile(bytecode, &bytecode_size, code);
      show_bytecode(bytecode, bytecode_size);
      break;
    case 'f':
      if (param.out_filename == NULL) {
        filter(stdout, code);
      } else {
        if ((ofp = fopen(param.out_filename, "w")) == NULL) {
          fprintf(stderr, "Unable to open file: %s\n", param.out_filename);
          return EXIT_FAILURE;
        }
        filter(ofp, code);
        fclose(ofp);
      }
      break;
    case 'm':
      compile(bytecode, &bytecode_size, code);
      show_mnemonic(stdout, bytecode, bytecode_size);
      break;
    case 't':
      if (param.out_filename == NULL) {
        translate(stdout, code);
      } else {
        if ((ofp = fopen(param.out_filename, "w")) == NULL) {
          fprintf(stderr, "Unable to open file: %s\n", param.out_filename);
          return EXIT_FAILURE;
        }
        translate(ofp, code);
        fclose(ofp);
      }
      break;
    default:
      compile(bytecode, &bytecode_size, code);
      execute(bytecode);
      break;
  }
  return EXIT_SUCCESS;
}


/*!
 * @brief Parse command-line arguments and set parameters.
 *
 * 'argv' is sorted after called getopt_long().
 * @param [out]    param  Parameters of this program
 * @param [in]     argc   A number of command-line arguments
 * @param [in,out] argv   Command-line arguments
 */
void parse_arguments(Param *param, int argc, char *argv[]) {
  static const struct option opts[] = {
    {"bytecode",  no_argument,       NULL, 'b'},
    {"filter",    no_argument,       NULL, 'f'},
    {"help",      no_argument,       NULL, 'h'},
    {"mnemonic",  no_argument,       NULL, 'm'},
    {"output",    required_argument, NULL, 'o'},
    {"translate", no_argument,       NULL, 't'},
    {0, 0, 0, 0}  /* must be filled with zero */
  };
  int ret;
  int optidx = 0;
  while ((ret = getopt_long(argc, argv, "bfhmo:tests", opts, &optidx)) != -1) {
    switch (ret) {
      case 'b':  /* -b, --bytecode */
      case 'f':  /* -f, --filter */
      case 'm':  /* -n or --nocompile */
      case 't':  /* -tests or --translate */
        param->mode = ret;
        break;
      case 'h':  /* -h, --help */
        show_usage(argv[0]);
        exit(EXIT_SUCCESS);
      case 'o':  /* -o or --output */
        param->out_filename = optarg;
        break;
      case '?':  /* unknown option */
        show_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  if (optind != argc - 1) {
    fputs("Please specify one blankspace source code\n", stderr);
    show_usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  param->in_filename = argv[optind];
}


/*!
 * @brief Show usage of this program and exit
 * @param [in] progname  A name of this program
 */
void show_usage(const char *progname) {
  printf(
      "[Usage]\n"
      "  $ %s FILE [options]\n"
      "[Options]\n"
      "  -b, --bytecode\n"
      "    Show code in hexadecimal\n"
      "  -f, --filter\n"
      "    Visualize blankspace source code\n"
      "  -h, --help\n"
      "    Show help and exit\n"
      "  -m, --mnemonic\n"
      "    Show byte code in mnemonic format\n"
      "  -o FILE, --output=FILE\n"
      "    Specify output filename\n"
      "  -tests, --translate\n"
      "    Translate brainfuck to C source code\n", progname);
}
