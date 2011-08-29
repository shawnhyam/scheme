#include <assert.h>
#include "scheme.h"
#include "read.h"
#include "write.h"
#include "pair.h"


static int gensym_count = 0;

obj_t is_3d_tagged_list(obj_t tag, obj_t lst) {
  return is_pair(lst) && (car(lst) == tag || 0); // FIXME
}


obj_t env_lookup(obj_t env, obj_t exp) {
  assert(0);
}

// expression types

// if
obj_t is_if(obj_t exp) { assert(0); }
obj_t if_cond(obj_t exp) { return cadr(exp); }
obj_t if_true(obj_t exp) { return caddr(exp); }
obj_t if_false(obj_t exp) { return cadddr(exp); }


// application

obj_t is_app(obj_t exp) { return is_pair(exp); }
obj_t app_fun(obj_t exp) { return car(exp); }
obj_t app_args(obj_t exp) { return cdr(exp); }



// macro
obj_t is_macro(obj_t exp) { return is_3d_tagged_list(mk_symbol("macro"), exp); }
obj_t macro_proc(obj_t exp) { return cadr(exp); }


obj_t is_syntax_primitive(obj_t value) { return is_3d_tagged_list(mk_symbol("syntax-primitive"), value); }
obj_t syntax_primitive_eval(obj_t value) { return cadr(value); }


// evaluation

obj_t perform_apply(obj_t fun, obj_t app_exp, obj_t env);

obj_t eval(obj_t exp, obj_t env) {
  if (is_fixnum(exp) || is_boolean(exp) || is_string(exp)) {
    return exp;
  } else if (is_symbol(exp)) {
    return env_lookup(env, exp);
  } else if (is_thunk(exp)) { 
    unwrap_thunk(exp)();
  } else if (is_app(exp)) {
    return perform_apply(eval(app_fun(exp), env), exp, env);
  } else {
    assert(0); // ???
  }

}

obj_t eval_if(obj_t exp, obj_t env) {
  if (eval(if_cond(exp), env) != imm_false) {
    return eval(if_true(exp), env);
  } else {
    return eval(if_false(exp), env);
  }
}



// initial environment


obj_t perform_apply(obj_t fun, obj_t app_exp, obj_t env) {
  obj_t args = app_args(app_exp);
  if (is_macro(fun)) {
    assert(0);
  } else if (is_syntax_primitive(fun)) {
    assert(0);
  } else {
    assert(0);
  }
}


int main() {
  obj_t initial_environment_amap = 
    list1(list2(mk_symbol("if"),
                list2(mk_symbol("syntax-primitive"), eval_if)));
                                               
                                         

  while (1) {
    printf("> ");
    obj_t expr = read(stdin);
    write(expr);
    printf("\n");
    write(eval(expr, imm_empty_list));
    printf("\n");
  }
}
