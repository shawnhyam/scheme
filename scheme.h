#ifndef __SCHEME_H
#define __SCHEME_H

#include <inttypes.h>

typedef uint64_t obj_t;

// FIXNUM

typedef int64_t fixnum_t;

char is_fixnum(obj_t obj);
obj_t wrap_fixnum(fixnum_t x);
fixnum_t unwrap_fixnum(obj_t obj);


// CONSTANTS

static const obj_t imm_true;
static const obj_t imm_false;
static const obj_t imm_empty_list;

char is_boolean(obj_t obj);

// PRIMITIVE PROC

typedef obj_t (*primitive_proc_t)(obj_t*);

char is_primitive_proc(obj_t obj);
obj_t wrap_primitive_proc(primitive_proc_t x);
primitive_proc_t unwrap_primitive_proc(obj_t obj);


// PAIR

struct pair {
  obj_t car;
  obj_t cdr;
};

typedef struct pair * pair_t;

char is_pair(obj_t obj);
obj_t wrap_pair(pair_t x);
pair_t unwrap_pair(obj_t obj);
pair_t alloc_pair(obj_t car, obj_t cdr);
obj_t mk_pair(obj_t car, obj_t cdr);


// SYMBOL

struct symbol {
  int reserved;
  char value[];
};

typedef struct symbol * symbol_t;

char is_symbol(obj_t obj);
obj_t wrap_symbol(symbol_t x);
symbol_t unwrap_symbol(obj_t obj);
symbol_t alloc_symbol(char *value);
obj_t mk_symbol(char *value);


// STRING

struct string {
  int reserved;
  char value[];
};

typedef struct string * string_t;

char is_string(obj_t obj);
obj_t wrap_string(string_t x);
string_t unwrap_string(obj_t obj);
string_t alloc_string(char *value);
obj_t mk_string(char *value);

#endif


