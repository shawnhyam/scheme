#include "write.h"

#include <assert.h>
#include <stdio.h>


void write_pair(pair_t *pair) {
  write(pair->car);
  if (is_pair(pair->cdr)) {
    printf(" ");
    write_pair(untag_pair(pair->cdr));
  } else if (is_empty_list(pair->cdr)) {
    return; 
  } else {
    printf(" . ");
    write(pair->cdr);
  }
}

void write(any obj) {
  if (is_fixnum(obj)) {
    printf("%lld", untag_fixnum(obj));
  } else if (is_boolean(obj)) {
    char b = untag_boolean(obj);
    if (b) {
      printf("#t");
    } else {
      printf("#f");
    }
  } else if (is_pair(obj)) {
    printf("(");
    write_pair(untag_pair(obj));
    printf(")");
  } else if (is_symbol(obj)) {
    printf("%s", untag_symbol(obj)+1);
  } else if (is_string(obj)) {
    printf("\"%s\"", untag_string(obj)+1);
  } else if (is_empty_list(obj)) {
    printf("()");
  } else if (is_primitive_expr(obj)) {
    printf("<#!primfn>");
  } else {
    printf("!!!");
    assert(0);
  }

}

