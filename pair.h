#ifndef __PAIR_H
#define __PAIR_H

#include "scheme.h"


static obj_t cons(obj_t car, obj_t cdr) { return mk_pair(car, cdr); }
static obj_t car(obj_t pair) { return unwrap_pair(pair)->car; }
static obj_t cdr(obj_t pair) { return unwrap_pair(pair)->cdr; }

#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))


static obj_t list0() { return imm_empty_list; }
static obj_t list1(obj_t e) { return cons(e, list0()); }
static obj_t list2(obj_t e0, obj_t e1) { return cons(e0, list1(e1)); }
static obj_t list3(obj_t e0, obj_t e1, obj_t e2) { return cons(e0, list2(e1, e2)); }
static obj_t list4(obj_t e0, obj_t e1, obj_t e2, obj_t e3) { return cons(e0, list3(e1, e2, e3)); }

#endif
