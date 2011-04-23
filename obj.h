#ifndef OBJ_H_
#define OBJ_H_

#include <stdint.h>

typedef int64_t any;

typedef struct pair {
  any car;
  any cdr;
} pair_t;

typedef struct symbol {
  char *sym;
} symbol_t;

any tag_fixnum(int64_t v);
int64_t untag_fixnum(any obj);
char is_fixnum(any obj);

any tag_boolean(char v);
char untag_boolean(any obj);
char is_boolean(any obj);

any tag_character(char v);
char untag_character(any obj);
char is_character(any obj);

any tag_pair(pair_t *pair);
pair_t *untag_pair(any obj);
pair_t *make_pair(any car, any cdr);
char is_pair(any obj);

// what about the empty list?
char is_empty_list(any obj);

static const any empty_list = 0xa;
static const any true;
static const any false;

any tag_symbol(char *symbol);
char *untag_symbol(any obj);
char *make_symbol(char *sym);
char is_symbol(any obj);

any tag_string(char *s);
char *untag_string(any obj);
char *make_string(char *s);
char is_string(any obj);


typedef any (*prim_expr_t)(any arg);

any tag_primitive_expr(prim_expr_t fn);
prim_expr_t untag_primitive_expr(any obj);
char is_primitive_expr(any obj);

#endif
