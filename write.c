#include "obj.h"

#include <assert.h>
#include <stdio.h>


void write_pair(pair_t pair, FILE *f) {
  write(pair->car, f);
  if (is_pair(pair->cdr)) {
    fprintf(f, " ");
    write_pair(untag_pair(pair->cdr), f);
  } else if (is_empty_list(pair->cdr)) {
    return; 
  } else {
    fprintf(f, " . ");
    write(pair->cdr, f);
  }
}

void write(any obj, FILE *f) {
  if (is_fixnum(obj)) {
    fprintf(f, "%lld", untag_fixnum(obj));
  } else if (is_boolean(obj)) {
    char b = untag_boolean(obj);
    if (b) {
      fprintf(f, "#t");
    } else {
      fprintf(f, "#f");
    }
  } else if (is_pair(obj)) {
    fprintf(f, "(");
    write_pair(untag_pair(obj), f);
    fprintf(f, ")");
  } else if (is_symbol(obj)) {
    fprintf(f, "%s", untag_symbol(obj)->sym);
  } else if (is_string(obj)) {
    fprintf(f, "\"%s\"", untag_string(obj)+1);
  } else if (is_empty_list(obj)) {
    fprintf(f, "()");
  } else if (is_primitive_expr(obj)) {
    fprintf(f, "<#!primfn>");
  } else {
    printf("!!!");
    assert(0);
  }

}

