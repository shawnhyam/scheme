#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

typedef uint64_t obj_t;

// some scheme structs

// FIXNUM

typedef int64_t fixnum_t;

static const char fixnum_tag = 0;

char is_fixnum(obj_t obj) { return (obj&7) == fixnum_tag; }
obj_t wrap_fixnum(fixnum_t n) { return (obj_t)n + fixnum_tag; }
fixnum_t unwrap_fixnum(obj_t obj) {
  assert(is_fixnum(obj));
  return (fixnum_t)(obj-fixnum_tag);
}


// PAIR

struct pair {
  obj_t car;
  obj_t cdr;
};

static const char pair_tag = 1;

struct pair * make_pair(obj_t car, obj_t cdr) {
  struct pair *pair = malloc(sizeof(struct pair));
  pair->car = car;
  pair->cdr = cdr;
  return pair;
}

char is_pair(obj_t obj) { return (obj&7) == pair_tag; }

obj_t wrap_pair(struct pair *pair) {
  return (obj_t)pair + pair_tag;
}

struct pair *unwrap_pair(obj_t obj) {
  assert(is_pair(obj));
  return (struct pair *)(obj-pair_tag);
}

//

obj_t the_empty_list;

char is_the_empty_list(obj_t obj) { return obj == the_empty_list; }

obj_t cons(obj_t car, obj_t cdr) {
  return wrap_pair(make_pair(car, cdr));
}

obj_t car(obj_t pair) { return unwrap_pair(pair)->car; }
obj_t cdr(obj_t pair) { return unwrap_pair(pair)->cdr; }

obj_t caar(obj_t pair) { return car(car(pair)); }
obj_t cdar(obj_t pair) { return cdr(car(pair)); }

// SYMBOL

// impl functions (SYMBOL)

struct symbol {
  int reserved;
  char value[];
};

static const char symbol_tag = 2;

char is_symbol(obj_t obj) { return (obj&7) == symbol_tag; }

obj_t wrap_symbol(struct symbol *s) {
  return (obj_t)s + symbol_tag;
}

struct symbol *unwrap_symbol(obj_t obj) {
  assert(is_symbol(obj));
  return (struct symbol *)(obj-symbol_tag);
}

obj_t symbol_table;

struct symbol *make_symbol(char *value) {
  /* search for they symbol in the symbol table */
  obj_t element = symbol_table;
  while (!is_the_empty_list(element)) {
    obj_t e = car(element);
    if (strcmp(unwrap_symbol(e)->value, value) == 0) {
      return unwrap_symbol(e);
    }
    element = cdr(element);
  };
    
  /* create the symbol and add it to the symbol table */
  struct symbol *s = malloc(sizeof(*s) + strlen(value+1)*sizeof(s->value[0]));
  strcpy(s->value, value);

  symbol_table = cons(wrap_symbol(s), symbol_table);
  return s;
}





// 

// environment == pair

struct call_frame {
  obj_t expression;
  struct pair *environment;
  struct pair *rib;
  struct call_frame *next_frame;
};

struct closure {
  obj_t function_body;  // could be a compound_proc instead, I think
  struct pair *environment;
  struct pair *variables;
};

struct continuation {
};


struct pair * lookup(obj_t var, obj_t e) {
 nxtrib: {
  obj_t vars = caar(e);
  obj_t vals = cdar(e);
  
 nxtelt:
  if (is_the_empty_list(vars)) {
    e = cdr(e);
    goto nxtrib;
  } else if (unwrap_symbol(car(vars)) == unwrap_symbol(var)) {
    return unwrap_pair(vals);
  } else {
    vars = cdr(vars);
    vals = cdr(vals);
    goto nxtelt;
  }
  }
}


int main() {
  obj_t a = 0;
  //obj_t x = wrap_symbol(make_symbol("hello"));
  obj_t x = wrap_fixnum(88);
  obj_t e = the_empty_list;

  obj_t vars = cons(wrap_symbol(make_symbol("hello")), the_empty_list);
  obj_t vals = cons(44, the_empty_list);
  e = cons(cons(vars, vals), e);

  struct pair *r;
  struct call_frame *s;

 vm:
  printf("acc: %llu\n", a);
  if (is_symbol(x)) {
    // look up in environment (refer)
    a = lookup(x, e)->car;
    x = 0;
  } else {
    // constant
    a = x;
  }
  printf("acc: %llu\n", a);


}
