#include "obj.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

any tag_fixnum(int64_t v) { return v<<2; }
int64_t untag_fixnum(any obj) { return obj>>2; }
char is_fixnum(any obj) { return (obj&0x3) == 0; }

any tag_boolean(char v) { return ((v ? 1 : 0) << 2) + 2; }
char untag_boolean(any obj) { return obj>>2; }
char is_boolean(any obj) { return (obj==0x2) || (obj==0x6); }

any tag_character(char v) { return (v<<8) + 0x26; }
char untag_character(any obj) { return obj>>8; }
char is_character(any obj) { return (obj & 0xff) == 0x26; }


any tag_symbol(symbol_t symbol) { return (int64_t)symbol + 0x3; }
symbol_t untag_symbol(any obj) { return (symbol_t)(obj - 0x3); }
symbol_t make_symbol(char *value) {
  static symbol_t symbols[100];
  static size_t nsymbols = 0;

  for (size_t i=0; i<nsymbols; i++) {
    if (strcmp(symbols[i]->sym, value) == 0) {
      return symbols[i];
    }
  }

  

  // need a new symbol
  symbol_t sym = (symbol_t)malloc(sizeof(symbol_t) + strlen(value) + 1);
  sym->header = '\'';
  strcpy(sym->sym, value);
  symbols[nsymbols++] = sym;
  return sym;
}
char is_symbol(any obj) { return ((obj & 0x7) == 3) && (untag_symbol(obj)->header == '\''); }

any tag_string(char *symbol) { return (int64_t)symbol + 0x3; }
char *untag_string(any obj) { return (char*)(obj - 0x3); }
char *make_string(char *value) {
  // need a new symbol
  char *sym = (char*)malloc(strlen(value)+2);
  sym[0] = 's';
  strcpy(sym+1, value);
  return sym;
}
char is_string(any obj) { return ((obj & 0x7) == 3) && (untag_string(obj)[0] == 's'); }

any tag_pair(pair_t pair) { return (int64_t)pair + 1; }
pair_t untag_pair(any obj) { return (pair_t)(obj-1); }
pair_t make_pair(any car, any cdr) { 
  pair_t p = (pair_t)malloc(sizeof(pair_t));
  p->car = car;
  p->cdr = cdr;
  return p;
}

char is_pair(any obj) { return (obj & 0x7) == 0x1; }
char is_empty_list(any obj) { return obj == empty_list; }



any tag_primitive_expr(prim_expr_t fn) {
  return (int64_t)fn + 0x7;
}
prim_expr_t untag_primitive_expr(any obj) { return (prim_expr_t)(obj - 0x7); }
char is_primitive_expr(any obj) { return ((obj & 0x7) == 0x7); }
