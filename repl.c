#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "scheme.h"
#include "pair.h"
#include "read.h"
#include "write.h"

/// call frames

obj_t mk_call_frame(obj_t next_expr, obj_t env, obj_t rib) {
  return list4(next_expr, env, rib, next_frame);
}


/// control stack

obj_t push_call_frame(obj_t call_frame, obj_t stack) {
  return cons(call_frame, stack);
}

obj_t mk_control_stack() {
  return list0();
}



/// eval

obj_t new_eval(obj_t expr, obj_t env, obj_t rib, obj_t stack) {
  
}





obj_t lookup_in_rib(obj_t var, obj_t vars, obj_t vals) {
  if (imm_empty_list == vars) {
    return imm_undefined;
  }

  if (car(vars) == var) {
    printf("GOT A MATCH!!");
    return cons(car(vars), car(vals));
  }

  return lookup_in_rib(var, cdr(vars), cdr(vals));
}


obj_t lookup(obj_t var, obj_t e) {
  if (imm_empty_list == e) { 
    assert(0);
  }

  printf("lookup: ");
  write(e);
  printf("\n\n");

  obj_t rib = car(e);
  obj_t result = lookup_in_rib(var, car(rib), cadr(rib));
  if (result != imm_undefined) {
    return result;
  }
  return lookup(var, cdr(e));
}

//////

char is_tagged_list(obj_t expr, obj_t tag) {
  if (is_pair(expr)) {
    if (is_symbol(car(expr))) {
      return car(expr) == tag;
    }
  }
  return 0;
}

char is_3d_tagged_list(obj_t expr, obj_t tag) {
  return is_pair(expr) && is_symbol(car(expr)) && car(expr)==tag;
}


obj_t if_symbol;
obj_t quote_symbol;

char is_tail(obj_t next) {
  return is_tagged_list(next, mk_symbol("return"));
}

obj_t compile(obj_t x, obj_t next) {
  printf("<");
  write(x);
  printf(">\n");
  if (is_symbol(x)) {
    return list3(mk_symbol("refer"), x, next);
  } else if (is_pair(x)) {
    if (is_tagged_list(x, quote_symbol)) {
      return list3(mk_symbol("constant"), cadr(x), next);
    } else if (is_tagged_list(x, mk_symbol("lambda"))) {
      return list4(mk_symbol("close"), 
		   cadr(x), 
		   compile(caddr(x), list1(mk_symbol("return"))),
		   next);
    } else if (is_tagged_list(x, if_symbol)) {
      obj_t test = cadr(x);
      obj_t on_true = caddr(x);
      obj_t on_false = cadddr(x);
      obj_t truec = compile(on_true, next);
      obj_t falsec = compile(on_false, next);
      return compile(test, list3(mk_symbol("test"), truec, falsec));
    } else {
      obj_t args = cdr(x);
      obj_t c = compile(car(x), list1(mk_symbol("apply")));
    loop:
      if (args == imm_empty_list) {
	if (is_tail(next))
	  return c;
	else
	  return list3(mk_symbol("frame"), next, c);
      } else {
	c = compile(car(args), list2(mk_symbol("argument"), c));
	args = cdr(args);
	goto loop;
      }
    }
  }
  return list3(mk_symbol("constant"), x, next);
}

obj_t closure(obj_t body, obj_t e, obj_t vars) { 
  return list3(body, e, vars);
}

obj_t call_frame(obj_t x, obj_t e, obj_t r, obj_t s) {
  return list4(x, e, r, s);
}

obj_t extend(obj_t e, obj_t vars, obj_t vals) {
  return cons(cons(vars, vals), e);
}

obj_t VM(obj_t a, obj_t x, obj_t e, obj_t r, obj_t s) {
  printf("<< %s >>\n", unwrap_symbol(car(x))->value);

  if (is_tagged_list(x, mk_symbol("halt"))) {
    return a;
  } else if (is_tagged_list(x, mk_symbol("refer"))) {
    obj_t val = cdr(lookup(cadr(x), e));
    printf("lookup result ==> ");
    write(val);
    printf("\n");
    return VM(val, caddr(x), e, r, s);
  } else if (is_tagged_list(x, mk_symbol("constant"))) {
    return VM(cadr(x), caddr(x), e, r, s);
  } else if (is_tagged_list(x, mk_symbol("close"))) {
    obj_t vars = cadr(x);
    obj_t body = caddr(x);
    obj_t x_ = cadddr(x);
    return VM(closure(body, e, vars), x_, e, r, s);
  } else if (is_tagged_list(x, mk_symbol("test"))) {
    return VM(a, (a!=imm_false) ? cadr(x) : caddr(x), e, r, s);
  } else if (is_tagged_list(x, mk_symbol("frame"))) {
    return VM(a, caddr(x), e, imm_empty_list, call_frame(cadr(x), e, r, s));
  } else if (is_tagged_list(x, mk_symbol("argument"))) {
    return VM(a, cadr(x), e, cons(a, r), s);
  } else if (is_tagged_list(x, mk_symbol("apply"))) {
    obj_t body = car(a);
    obj_t e = cadr(a);
    obj_t vars = caddr(a);
    return VM(a, body, extend(e, vars, r), imm_empty_list, s);
  } else if (is_tagged_list(x, mk_symbol("return"))) {
    return VM(a, car(s), cadr(s), caddr(s), cadddr(s));
  } else {
    assert(0);
  }
} 

obj_t eval_and(obj_t args) {
  obj_t exp = car(args);
  obj_t env = cadr(args);

  assert(0);
}


obj_t eval(obj_t x) {


  obj_t globals = list2(list1(mk_symbol("and")), 
			list1(list2(mk_symbol("syntax-primitive"), 
				    wrap_primitive_proc(eval_and))));
  write(globals);

  return VM(imm_empty_list, 
	    compile(x, list1(mk_symbol("halt"))),
	    list1(globals),
	    imm_empty_list,
	    imm_empty_list);
}

int main() {
  if_symbol = mk_symbol("if");
  quote_symbol = mk_symbol("quote");

  while (1) {
    printf("\n> ");
    write(eval(read(stdin)));
  }

}
