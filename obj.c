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


any tag_symbol(char *symbol) { return (int64_t)symbol + 0x3; }
char *untag_symbol(any obj) { return (char*)(obj - 0x3); }
char *make_symbol(char *value) {
  static char *symbols[100];
  static size_t nsymbols = 0;

  for (size_t i=0; i<nsymbols; i++) {
    if (strcmp(symbols[i], value) == 0) {
      return symbols[i]-1;
    }
  }
  printf("make symbol <%s>\n", value);

  // need a new symbol
  char *sym = (char*)malloc(strlen(value)+2);
  sym[0] = '\'';
  strcpy(sym+1, value);
  symbols[nsymbols++] = (sym+1);
  return sym;
}
char is_symbol(any obj) { return ((obj & 0x7) == 3) && (untag_symbol(obj)[0] == '\''); }

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

any tag_pair(pair_t *pair) { return (int64_t)pair + 1; }
pair_t *untag_pair(any obj) { return (pair_t*)(obj-1); }
pair_t *make_pair(any car, any cdr) { 
  pair_t *p = (pair_t*)malloc(sizeof(pair_t));
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
