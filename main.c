#include "obj.h"
#include "read.h"
#include "write.h"
#include "compile.h"
#include "eval.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
  for (int i=0; i<10000000; i++) {
    int64_t r = rand();
    any obj = tag_fixnum(r);
    assert(is_fixnum(obj));
    assert(untag_fixnum(obj) == r);
  }

  write(eval(tag_fixnum(33)), stdout);
  printf("\n");

  write(tag_boolean(0), stdout);
  printf(" ");
  write(tag_boolean(1), stdout);
  printf("\n");

  /*
  any fn = tag_primitive_expr(&quote);
  printf("%p %lld\n", &quote, fn);
  assert(is_primitive_expr(fn));
  assert(untag_primitive_expr(fn) == &quote);
  */

  pair_t pair = make_pair(tag_fixnum(33), tag_fixnum(44));
  any cell = tag_pair(pair);
  write(cell, stdout);
  printf("\n");

  any cell2 = tag_pair(make_pair(tag_fixnum(88), empty_list));
  write(tag_pair(make_pair(tag_fixnum(22), cell2)), stdout);
  printf("\n");

  assert(is_pair(cell));
  assert(untag_pair(cell) == pair);

  char v = 21;
  any o = tag_character(v);
  printf("%d %ld %d\n", v, (long int)o, untag_character(o));

    printf("Welcome to Bootstrap Scheme. "
           "Use ctrl-c to exit.\n");

    //init();

    while (1) {
        printf("> ");
	write(eval(compile(read(stdin))), stdout);
        //write(eval(read(stdin), the_global_environment));
        printf("\n");
    }


  return 0;
}
