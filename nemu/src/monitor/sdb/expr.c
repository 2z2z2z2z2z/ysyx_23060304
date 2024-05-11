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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include "memory/paddr.h"

enum {
  TK_NOTYPE = 256, TK_NUM, TK_NEG, TK_DEREF, TK_HEX, TK_REG, TK_NEQ, TK_AND, TK_EQ,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +",                TK_NOTYPE},    // spaces
  {"0[xX][0-9a-fA-F]+", TK_HEX},       // hexadecimal
  {"[0-9]+",            TK_NUM},       // numbers
  {"\\+",               '+'},          // plus
  {"-",                 '-'},          // minus or negative sign
  {"\\*",               '*'},          // multiple
  {"/",                 '/'},          // divide
  {"\\(",               '('},          // (
  {"\\)",               ')'},          // )
  {"\\$[\\$a-z0-9]+",   TK_REG},       // register
  {"==",                TK_EQ},       // equal
  {"!=",                TK_NEQ},      // not equal
  {"&&",                TK_AND},      // and
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            nr_token -= 1;
            break;
          case TK_HEX:
            tokens[nr_token].type = TK_NUM;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            sprintf(tokens[nr_token].str, "%lu", strtoul(tokens[nr_token].str, NULL, 16));
            break;
          case TK_NUM:
            tokens[nr_token].type = TK_NUM;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_REG:
            tokens[nr_token].type = TK_NUM;
            strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
            tokens[nr_token].str[substr_len - 1] = '\0';
            bool success = true;
            sprintf(tokens[nr_token].str, "%d", isa_reg_str2val(tokens[nr_token].str, &success));
            if (!success) {
              printf("wrong reg");
            }
            break;
          case '+':
            tokens[nr_token].type = '+';
            break;
          case '-':
            tokens[nr_token].type = '-';
            break;
          case '*':
            tokens[nr_token].type = '*';
            break;
          case '/':
            tokens[nr_token].type = '/';
            break;
          case '(':
            tokens[nr_token].type = '(';
            break;
          case ')':
            tokens[nr_token].type = ')';
            break;
          case TK_AND:
            tokens[nr_token].type = TK_AND;
            break;
          case TK_EQ:
            tokens[nr_token].type = TK_EQ;
            break;
          case TK_NEQ:
            tokens[nr_token].type = TK_NEQ;
            break;
          default:
            Log("unknown token");
            assert(0);
        }
        nr_token += 1;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q) {
  int parentheses = 0;
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    for (int i = p; i <= q; ++i) {
      if (tokens[i].type == '(') {
        parentheses += 1;
      } else if (tokens[i].type == ')') {
        parentheses -= 1;
        if (parentheses == 0 && i != q) {
          return false;
        }
      }
      if (parentheses < 0) {
        assert(0);
      }
    }
    return true;
  } else {
    return false;
  }
}

static int parentheses = 0;

int eval(int p, int q) {
//  if (tokens[p].type == TK_NEG) {
//    int i = p;
//    while (tokens[i + 1].type != TK_NUM) {
//      i = i + 1;
//    }
//    if (i == q - 1) {
//      return (i - p) % 2 ? strtol(tokens[q].str, 0, 10) : -strtol(tokens[q].str, 0, 10);
//    }
//  }
  int op = -1;
  if (p > q) {
    /* Bad expression */
    assert(0);
  } else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    return strtol(tokens[q].str, 0, 10);
  } else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  } else {
    parentheses = 0;
    for (int i = p; i < q; ++i) {
      if (tokens[i].type == TK_NUM) {
        continue;
      }
      if (tokens[i].type == '(') {
        parentheses += 1;
        continue;
      }
      if (tokens[i].type == ')') {
        parentheses -= 1;
        continue;
      }
      if (parentheses) {
        continue;
      }
      if (tokens[i].type == TK_AND) {
        op = i;
      } else if (tokens[op].type == TK_AND) {
      } else if (tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ) {
        op = i;
      } else if (tokens[op].type == TK_EQ || tokens[op].type == TK_NEQ) {
      } else if (tokens[i].type == '+' || tokens[i].type == '-') {
        op = i;
      } else if (tokens[op].type == '+' || tokens[op].type == '-') {
      } else if (tokens[i].type == '*' || tokens[i].type == '/') {
        op = i;
      }
    }
  }
  if (op == -1) {
    if (tokens[p].type == TK_NEG) {
      return -eval(p + 1, q);
    }
    if (tokens[p].type == TK_DEREF) {
      return paddr_read(eval(p + 1, q), 4);
    }
  }
  int val1 = eval(p, op - 1);
  int val2 = eval(op + 1, q);

  switch (tokens[op].type) {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2;
    case '*':
      return (int) val1 * val2;
    case '/':
      return (int) val1 / val2;
    case TK_EQ:
      return val1 == val2;
    case TK_NEQ:
      return val1 != val2;
    case TK_AND:
      return val1 && val2;
    default:
      assert(0);
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    assert(0);
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  for (int i = 0; i < nr_token; i++) {
    if (tokens[i].type == '*' && (i == 0 || !(tokens[i - 1].type == ')' || tokens[i - 1].type == TK_NUM))) {
      tokens[i].type = TK_DEREF;
    } else if (tokens[i].type == '-' && (i == 0 || !(tokens[i - 1].type == ')' || tokens[i - 1].type == TK_NUM))) {
      tokens[i].type = TK_NEG;
    }
  }

  return (word_t) eval(0, nr_token - 1);
}
