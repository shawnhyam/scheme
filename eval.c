#include "eval.h"

#include <stdio.h>
#include <assert.h>
#include "compile.h"  // TODO remove uses of car, cadr, etc.


any operator(any obj) {
  return untag_pair(obj)->car;
}

any operands(any obj) {
  return untag_pair(obj)->cdr;
}


any eval(any obj);

any list_of_values(any obj) {
  if (is_empty_list(obj)) { return obj; }
  return tag_pair(make_pair(eval(car(obj)),
			    list_of_values(cdr(obj))));
}

char is_self_evaluating(any obj) {
  return is_fixnum(obj) || is_boolean(obj) || is_string(obj) || is_primitive_expr(obj);
}

any eval(any obj) {
  if (is_self_evaluating(obj)) {
    return obj;
  } else if (is_pair(obj)) {
    if (is_symbol(car(obj)) && (equal_symbols(untag_symbol(car(obj)), make_symbol("quote")))) {
      return cadr(obj);
    } else {
      // 
      any procedure = eval(operator(obj));
      any arguments = list_of_values(operands(obj));
    
      return untag_primitive_expr(procedure)(arguments);    
    }
  } 
  assert(0);
}

