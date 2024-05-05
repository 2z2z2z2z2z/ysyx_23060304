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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[256];
  uint32_t previous;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void new_wp(char *expression) {
  bool success = true;
  if (free_ == NULL) {
    assert(0);
  }
  WP *new = free_;
  free_ = free_->next;
  strcpy(new->expr, expression);
  new->previous = expr(new->expr, &success);
  if (!success) {
    printf("WP wrong expression");
  }
  if (head == NULL) {
    head = new;
    new->next = NULL;
  } else {
    new->next = head;
    head = new;
  }
}

void free_wp(int i) {
  WP *wp = head;
  if (wp == NULL) { return; }
  else if (wp->NO == i) {
    head = wp->next;
    return;
  }
  while (1) {
    if (wp->next == NULL) { return; }
    if (wp->next->NO == i) {
      WP *tmp = wp->next;
      wp->next = wp->next->next;
      tmp->next = free_;
      free_ = tmp;
      return;
    } else {
      wp = wp->next;
    }
  }
}

bool watchpoint_check() {
  WP *wp = head;
  bool success = true;
  while (1) {
    if (wp == NULL) { break; }
    uint32_t res = expr(wp->expr, &success);
    if (!success) { assert(0); }
    if (res != wp->previous) {
      printf("%s\n", wp->expr);
      printf("Old value = %u\nNew value = %u\n", wp->previous, res);
      wp->previous = res;
      return true;
    } else {
      wp = wp->next;
    }
  }
  return false;
}

void watchpoint_display() {
  WP *wp = head;
  printf("Num\tWhat\t\n");
  if (wp == NULL) return;
  while (1) {
    printf("%d\t%s\t\n", wp->NO, wp->expr);
    if (wp->next == NULL) {
      break;
    } else {
      wp = wp->next;
    }
  }
}