/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
  "#include <stdio.h>\n"
  "int main() { "
  "  unsigned result = %s; "
  "  printf(\"%%u\", result); "
  "  return 0; "
  "}";
static int buf_index = 0;
static int depth = 0;

static void gen_rand_expr() {
  int r;
  depth = depth + 1;
  if (buf_index + 2 * depth > 65535) {
    r = 0;
  } else {
    r = random() % 3;
  }
  switch (r) {
    case 0:
      switch (random() % 4) {
        case 0:
          buf[buf_index] = random() % 10 + 48;
          break;
        case 1:
          buf[buf_index] = ' ';
          buf_index += 1;
          buf[buf_index] = random() % 10 + 48;
          break;
        case 2:
          buf[buf_index] = '-';
          buf_index += 1;
          buf[buf_index] = random() % 10 + 48;
          break;
        default:
          buf[buf_index] = random() % 10 + 48;
          buf_index += 1;
          buf[buf_index] = ' ';
      }
      buf_index += 1;
      break;
    case 1:
      buf[buf_index] = '(';
      buf_index += 1;
      gen_rand_expr();
      buf[buf_index] = ')';
      buf_index += 1;
      break;
    default:
      gen_rand_expr();
      switch (random() % 4) {
        case 0:
          buf[buf_index] = '+';
          break;
        case 1:
          buf[buf_index] = '-';
          break;
        case 2:
          buf[buf_index] = '*';
          break;
        case 3:
          buf[buf_index] = '/';
          break;
      }
      buf_index += 1;
      gen_rand_expr();
      break;
  }
  buf[buf_index] = '\0';
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++) {
    buf_index = 0;
    depth = 0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -Werror=div-by-zero -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
