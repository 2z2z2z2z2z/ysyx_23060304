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

#include <isa.h>
#include <cpu/cpu.h>
#include <memory/paddr.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();

void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    cpu_exec(1);
  } else {
    cpu_exec(atoi(arg));
  }
  return 0;
}

static int cmd_info(char *args) {
  if (!args) {
    cmd_help("info");
    return 0;
  }
  char subcommand;
  sscanf(args, "%c", &subcommand);
  if (subcommand == 'r') {
    isa_reg_display();
  } else if (subcommand == 'w') {
    watchpoint_display();
  } else {
    printf("unknown subcommand\n");
  }
  return 0;
}

static int cmd_mem(char *args) {
  if (!args) {
    cmd_help("x");
    return 0;
  }
  unsigned int start_addr, len;
  char expression[256];
  bool success = true;
  sscanf(args, "%u%s", &len, expression);
  start_addr = expr(expression, &success);
  if (!success) { printf("mem address wrong expression"); }
  for (int i = 0; i < len; ++i) {
    printf("0x%x: 0x%08x\n", start_addr, paddr_read(start_addr, 4));
    start_addr = start_addr + 4;
  }
  return 0;
}

static int cmd_expr(char *args) {
  if (!args) {
    cmd_help("p");
    return 0;
  }
  bool success = true;
  printf("%u\n", expr(args, &success));
  if (success) {
    return 0;
  } else {
    return 1;
  }
}

static int cmd_add_watch(char *args) {
  if (!args) {
    cmd_help("w");
    return 0;
  }
  new_wp(args);
  return 0;
}

static int cmd_del_watch(char *args) {
  if (!args) {
    cmd_help("d");
    return 0;
  }
  free_wp(strtoul(args, NULL, 10));
  return 0;
}

static struct {
  const char *name;
  const char *description;

  int (*handler)(char *);
} cmd_table[] = {
  {"help", "Display information about all supported commands",                           cmd_help},
  {"c",    "Continue the execution of the program",                                      cmd_c},
  {"q",    "Exit NEMU",                                                                  cmd_q},
  {"si",   "si [N]: Execute N instructions and pause, default 1",                        cmd_si},
  {"info", "info r: Print register; info w: Print watchpoint",                           cmd_info},
  {"x",    "x N EXPR: Use EXPR as the starting memory address, output N 4 bytes in hex", cmd_mem},
  {"p",    "Calculate EXPR value",                                                       cmd_expr},
  {"w",    "w EXPR: add watchpoint EXPR",                                                cmd_add_watch},
  {"d",    "d N: delete No. N watchpoind",                                               cmd_del_watch},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(args, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  } else {
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
