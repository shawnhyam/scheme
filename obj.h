#ifndef OBJ_H_
#define OBJ_H_

#include <stdint.h>
#include <stdio.h>

/*

  an object is a 64-bit integer or pointer

  (lower 16 bits only)
  xxxx xxxx xxxx x000    fixnum
  xxxx xxxx xxxx x010    immediate
  xxxx xxxx xxxx x001    pointer to pair
  xxxx xxxx xxxx x011    pointer to bytevector type (string, symbol, ...)
  xxxx xxxx xxxx x111    pointer to procedure (closure?) struct
  
  immediates:
  0000 0000 0000 0---    #f
  0000 0000 0000 1---    #t
  0000 0000 0001 0---    empty list
  cccc cccc 0001 1---    character
                         primitive proc?

 */

typedef int64_t any;

typedef struct symbol {
  char header;
  char sym[];
} * symbol_t;

typedef struct pair {
  any car;
  any cdr;
} * pair_t;


symbol_t make_symbol(char *sym);
pair_t make_pair(any car, any cdr);

any tag_fixnum(int64_t v);
any tag_boolean(char v);
any tag_character(char v);
any tag_symbol(symbol_t sym);
any tag_pair(pair_t pair);

int64_t untag_fixnum(any obj);
char untag_boolean(any obj);
char untag_character(any obj);
symbol_t untag_symbol(any obj);
pair_t untag_pair(any obj);

void write(any obj, FILE *f);
void write_fixnum(int64_t v, FILE *f);
void write_boolean(char v, FILE *f);
void write_character(char c, FILE *f);
void write_symbol(symbol_t sym, FILE *f);
void write_pair(pair_t pair, FILE *f);

char is_fixnum(any obj);
char is_boolean(any obj);
char is_character(any obj);
char is_symbol(any obj);
char is_pair(any obj);

static char equal_symbols(symbol_t s1, symbol_t s2) { return s1->sym == s2->sym; }

// what about the empty list?
char is_empty_list(any obj);

static const any empty_list = 0xa;
static const any true;
static const any false;

/*
any tag_symbol(char *symbol);
char *untag_symbol(any obj);
char *make_symbol(char *sym);
char is_symbol(any obj);
*/
any tag_string(char *s);
char *untag_string(any obj);
char *make_string(char *s);
char is_string(any obj);


typedef any (*prim_expr_t)(any arg);

any tag_primitive_expr(prim_expr_t fn);
prim_expr_t untag_primitive_expr(any obj);
char is_primitive_expr(any obj);

#endif
