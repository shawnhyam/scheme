#include "compile.h"

#include <assert.h>

any car(any obj) { 
  return untag_pair(obj)->car;
}

any cdr(any obj) {
  return untag_pair(obj)->cdr;
}

#define cadr(x) car(cdr(x))

any cons(any args) {
  return tag_pair(make_pair(car(args), cadr(args)));
}


any quote(any obj) {
  return car(obj);
}

any add(any args) {
  return car(args) + cadr(args);
}


/// COMPILER

// TODO compile should generate code instead of a sexp
any compile(any obj) {
  // FIXME these should be changed into environment references
  if (is_symbol(obj)) {
    if (equal_symbols(make_symbol("+"), untag_symbol(obj))) {
      return tag_primitive_expr(&add);
    } else if (equal_symbols(make_symbol("cons"), untag_symbol(obj))) {
      return tag_primitive_expr(&cons);
    } else if (equal_symbols(make_symbol("car"), untag_symbol(obj))) {
      return tag_primitive_expr(&car);
    } else if (equal_symbols(make_symbol("cdr"), untag_symbol(obj))) {
      return tag_primitive_expr(&cdr);
    } else {
      return obj;
    }
  } else if (is_pair(obj)) {
    pair_t pair = untag_pair(obj);
    return tag_pair(make_pair(compile(pair->car),
			      compile(pair->cdr)));
  } else {
    return obj;
  }
}

