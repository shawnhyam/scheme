#include "scheme.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char fixnum_tag = 0;
static const char pair_tag = 1;
static const char symbol_tag = 2;
static const char immediate_tag = 3;
static const char string_tag = 4;

static const char primitive_proc_tag = 7;

// FIXNUM


char is_fixnum(obj_t obj) { return (obj&7) == fixnum_tag; }
obj_t wrap_fixnum(fixnum_t n) { return ((obj_t)n) << 3; }
fixnum_t unwrap_fixnum(obj_t obj) {
  assert(is_fixnum(obj));
  return (fixnum_t)(obj>>3);
}


// CONSTANTS

static const obj_t imm_false = immediate_tag;
static const obj_t imm_true = (1<<3) + immediate_tag;
static const obj_t imm_empty_list = (2<<3) + immediate_tag;  // FIXME

char is_boolean(obj_t obj) {
  return (obj==imm_true) || (obj==imm_false);
}

// PRIMITIVE PROC

char is_primitive_proc(obj_t obj) { return (obj&7) == primitive_proc_tag; }
obj_t wrap_primitive_proc(primitive_proc_t fn) {
  assert(((obj_t)fn & 7) == 0);
  return (obj_t)fn + primitive_proc_tag;
}
primitive_proc_t unwrap_primitive_proc(obj_t obj) { 
  return (primitive_proc_t)(obj-primitive_proc_tag);
}


// PAIR

char is_pair(obj_t obj) { return (obj&7) == pair_tag; }

obj_t wrap_pair(pair_t pair) {
  return (obj_t)pair + pair_tag;
}

pair_t unwrap_pair(obj_t obj) {
  assert(is_pair(obj));
  return (pair_t)(obj-pair_tag);
}

pair_t alloc_pair(obj_t car, obj_t cdr) {
  pair_t pair = malloc(sizeof(struct pair));
  pair->car = car;
  pair->cdr = cdr;
  return pair;
}

obj_t mk_pair(obj_t car, obj_t cdr) { return wrap_pair(alloc_pair(car, cdr)); }


// SYMBOL

char is_symbol(obj_t obj) { return (obj&7) == symbol_tag; }

obj_t wrap_symbol(symbol_t s) {
  return (obj_t)s + symbol_tag;
}

symbol_t unwrap_symbol(obj_t obj) {
  assert(is_symbol(obj));
  return (symbol_t )(obj-symbol_tag);
}


symbol_t alloc_symbol(char *value) {
  static obj_t symbol_table = imm_empty_list;

  /* search for they symbol in the symbol table */
  obj_t element = symbol_table;
  while (element != imm_empty_list) {
    obj_t e = unwrap_pair(element)->car;
    if (strcmp(unwrap_symbol(e)->value, value) == 0) {
      return unwrap_symbol(e);
    }
    element = unwrap_pair(element)->cdr;
  };
    
  /* create the symbol and add it to the symbol table */
  symbol_t s = malloc(sizeof(*s) + strlen(value+1)*sizeof(s->value[0]));
  strcpy(s->value, value);

  symbol_table = mk_pair(wrap_symbol(s), symbol_table);
  return s;
}

obj_t mk_symbol(char *value) { return wrap_symbol(alloc_symbol(value)); }


// STRING

char is_string(obj_t obj) { return (obj&7) == string_tag; }

obj_t wrap_string(string_t s) {
  return (obj_t)s + string_tag;
}

string_t unwrap_string(obj_t obj) {
  assert(is_string(obj));
  return (string_t )(obj-string_tag);
}


string_t alloc_string(char *value) {
  string_t s = malloc(sizeof(*s) + strlen(value+1)*sizeof(s->value[0]));
  strcpy(s->value, value);
  return s;
}

obj_t mk_string(char *value) { return wrap_string(alloc_string(value)); }


