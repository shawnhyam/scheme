#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

typedef uint64_t obj_t;

// some scheme structs

// FIXNUM

typedef int64_t fixnum_t;

static const char fixnum_tag = 0;

char is_fixnum(obj_t obj) { return (obj&7) == fixnum_tag; }
obj_t wrap_fixnum(fixnum_t n) { return ((obj_t)n) << 3; }
fixnum_t unwrap_fixnum(obj_t obj) {
  assert(is_fixnum(obj));
  return (fixnum_t)(obj>>3);
}

// IMMEDIATES

static const char immediate_tag = 3;

char is_immediate(obj_t obj) { return (obj&7) == immediate_tag; }
static obj_t false = immediate_tag;
static obj_t true = (1<<3) + immediate_tag;

char is_boolean(obj_t obj) { return (obj==false) || (obj==true); };

// PRIMITIVE PROC

typedef obj_t (*primitive_proc_t)(obj_t*);

static const char primitive_proc_tag = 7;
char is_primitive_proc(obj_t obj) { return (obj&7) == primitive_proc_tag; }
obj_t wrap_primitive_proc(primitive_proc_t fn) {
  assert(((obj_t)fn & 7) == 0);
  return (obj_t)fn + primitive_proc_tag;
}
primitive_proc_t unwrap_primitive_proc(obj_t obj) { 
  return (primitive_proc_t)(obj-primitive_proc_tag);
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

obj_t cons_symbol(char *value) { return wrap_symbol(make_symbol(value)); }



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
    return unwrap_pair(car(vals));
  } else {
    vars = cdr(vars);
    vals = cdr(vals);
    goto nxtelt;
  }
  }
}
/**************************** PRINT ******************************/

void write(obj_t obj);

void write_pair(obj_t pair) {
    obj_t car_obj;
    obj_t cdr_obj;
    
    car_obj = car(pair);
    cdr_obj = cdr(pair);
    write(car_obj);
    if (is_pair(cdr_obj)) {
        printf(" ");
        write_pair(cdr_obj);
    }
    else if (is_the_empty_list(cdr_obj)) { 
        return;
    }
    else {
        printf(" . ");
        write(cdr_obj);
    }
}

void write(obj_t obj) {
    char c;
    char *str;
    
    if (is_the_empty_list(obj)) {
      printf("()");
    } else if (is_boolean(obj)) {
      printf("#%c", obj==false ? 'f' : 't');
    } else if (is_fixnum(obj)) {
      printf("%lld", unwrap_fixnum(obj));
    } else {
      assert(0);
    }
#if 0
        case SYMBOL:
            printf("%s", obj->data.symbol->value);
            break;
        case FIXNUM:
            printf("%ld", obj->data.fixnum);
            break;
        case CHARACTER:
            c = obj->data.character;
            printf("#\\");
            switch (c) {
                case '\n':
                    printf("newline");
                    break;
                case ' ':
                    printf("space");
                    break;
                default:
                    putchar(c);
            }
            break;
        case STRING:
            str = obj->data.string->value;
            putchar('"');
            while (*str != '\0') {
                switch (*str) {
                    case '\n':
                        printf("\\n");
                        break;
                    case '\\':
                        printf("\\\\");
                        break;
                    case '"':
                        printf("\\\"");
                        break;
                    default:
                        putchar(*str);
                }
                str++;
            }
            putchar('"');
            break;
        case PAIR:
            printf("(");
            write_pair(obj);
            printf(")");
            break;
        case PRIMITIVE_PROC:
        case COMPOUND_PROC:
            printf("#<procedure>");
            break;
        default:
            fprintf(stderr, "cannot write unknown type\n");
            exit(1);
    }
#endif
}

/***************************** READ ******************************/

char is_delimiter(int c) {
    return isspace(c) || c == EOF ||
           c == '('   || c == ')' ||
           c == '"'   || c == ';';
}

char is_initial(int c) {
    return isalpha(c) || c == '*' || c == '/' || c == '>' ||
             c == '<' || c == '=' || c == '?' || c == '!';
}

int peek(FILE *in) {
    int c;

    c = getc(in);
    ungetc(c, in);
    return c;
}

void eat_whitespace(FILE *in) {
    int c;
    
    while ((c = getc(in)) != EOF) {
        if (isspace(c)) {
            continue;
        }
        else if (c == ';') { /* comments are whitespace also */
            while (((c = getc(in)) != EOF) && (c != '\n'));
            continue;
        }
        ungetc(c, in);
        break;
    }
}

void eat_expected_string(FILE *in, char *str) {
    int c;

    while (*str != '\0') {
        c = getc(in);
        if (c != *str) {
            fprintf(stderr, "unexpected character '%c'\n", c);
            exit(1);
        }
        str++;
    }
}

void peek_expected_delimiter(FILE *in) {
    if (!is_delimiter(peek(in))) {
        fprintf(stderr, "character not followed by delimiter\n");
        exit(1);
    }
}

obj_t read_character(FILE *in) {
  assert(0);
#if 0
    int c;

    c = getc(in);
    switch (c) {
        case EOF:
            fprintf(stderr, "incomplete character literal\n");
            exit(1);
        case 's':
            if (peek(in) == 'p') {
                eat_expected_string(in, "pace");
                peek_expected_delimiter(in);
                return make_character(' ');
            }
            break;
        case 'n':
            if (peek(in) == 'e') {
                eat_expected_string(in, "ewline");
                peek_expected_delimiter(in);
                return make_character('\n');
            }
            break;
    }
    peek_expected_delimiter(in);
    return make_character(c);
#endif
}

obj_t read(FILE *in);

obj_t read_pair(FILE *in) {
    int c;
    obj_t car_obj;
    obj_t cdr_obj;
    
    eat_whitespace(in);
    
    c = getc(in);
    if (c == ')') { /* read the empty list */
        return the_empty_list;
    }
    ungetc(c, in);

    car_obj = read(in);

    eat_whitespace(in);
    
    c = getc(in);    
    if (c == '.') { /* read improper list */
        c = peek(in);
        if (!is_delimiter(c)) {
            fprintf(stderr, "dot not followed by delimiter\n");
            exit(1);
        }
        cdr_obj = read(in);
        eat_whitespace(in);
        c = getc(in);
        if (c != ')') {
            fprintf(stderr,
                    "where was the trailing right paren?\n");
            exit(1);
        }
        return cons(car_obj, cdr_obj);
    }
    else { /* read list */
        ungetc(c, in);
        cdr_obj = read_pair(in);        
        return cons(car_obj, cdr_obj);
    }
}

obj_t read(FILE *in) {
    int c;
    short sign = 1;
    int i;
    long num = 0;
#define BUFFER_MAX 1000
    char buffer[BUFFER_MAX];

    eat_whitespace(in);

    c = getc(in);    

    if (c == '#') { /* read a boolean or character */
        c = getc(in);
        switch (c) {
            case 't':
                return true;
            case 'f':
                return false;
            case '\\':
                return read_character(in);
            default:
                fprintf(stderr,
                        "unknown boolean or character literal\n");
                exit(1);
        }
    }
    else if (isdigit(c) || (c == '-' && (isdigit(peek(in))))) {
        /* read a fixnum */
        if (c == '-') {
            sign = -1;
        }
        else {
            ungetc(c, in);
        }
        while (isdigit(c = getc(in))) {
            num = (num * 10) + (c - '0');
        }
        num *= sign;
        if (is_delimiter(c)) {
            ungetc(c, in);
            return wrap_fixnum(num);
        }
        else {
            fprintf(stderr, "number not followed by delimiter\n");
            exit(1);
        }
    }
    else if (is_initial(c) ||
             ((c == '+' || c == '-') &&
              is_delimiter(peek(in)))) { /* read a symbol */
        i = 0;
        while (is_initial(c) || isdigit(c) ||
               c == '+' || c == '-') {
            /* subtract 1 to save space for '\0' terminator */
            if (i < BUFFER_MAX - 1) {
                buffer[i++] = c;
            }
            else {
                fprintf(stderr, "symbol too long. "
                        "Maximum length is %d\n", BUFFER_MAX);
                exit(1);
            }
            c = getc(in);
        }
        if (is_delimiter(c)) {
            buffer[i] = '\0';
            ungetc(c, in);
            return cons_symbol(buffer);
        }
        else {
            fprintf(stderr, "symbol not followed by delimiter. "
                            "Found '%c'\n", c);
            exit(1);
        }
    }
    else if (c == '"') { /* read a string */
        i = 0;
        while ((c = getc(in)) != '"') {
            if (c == '\\') {
                c = getc(in);
                if (c == 'n') {
                    c = '\n';
                }
            }
            if (c == EOF) {
                fprintf(stderr, "non-terminated string literal\n");
                exit(1);
            }
            /* subtract 1 to save space for '\0' terminator */
            if (i < BUFFER_MAX - 1) {
                buffer[i++] = c;
            }
            else {
                fprintf(stderr, 
                        "string too long. Maximum length is %d\n",
                        BUFFER_MAX);
                exit(1);
            }
        }
        buffer[i] = '\0';
        assert(0);
        // FIXME        return make_string(buffer);
    }
    else if (c == '(') { /* read the empty list or pair */
        return read_pair(in);
    }
    else if (c == '\'') { /* read quoted expression */
      return cons(cons_symbol("quote"), cons(read(in), the_empty_list));
    }
    else {
        fprintf(stderr, "bad input. Unexpected '%c'\n", c);
        exit(1);
    }
    fprintf(stderr, "read illegal state\n");
    exit(1);
}

//////

char is_tagged_list(obj_t expr, struct symbol *tag) {
  if (is_pair(expr)) {
    if (is_symbol(car(expr))) {
      return unwrap_symbol(car(expr)) == tag;
    }
  }
  return 0;
}

enum opcode {
  OP_HALT,
  OP_REFER,
  OP_CONSTANT,
  OP_CLOSE,
  OP_TEST,
  OP_ASSIGN,
  OP_CONTI,
  OP_NUATE,
  OP_FRAME,
  OP_ARGUMENT,
  OP_APPLY,
  OP_RETURN
};

struct bytecode {
  enum opcode opcode;
  obj_t arguments;
};

obj_t list0() { return the_empty_list; }
obj_t list1(obj_t e) { return cons(e, list0()); }
obj_t list2(obj_t e0, obj_t e1) { return cons(e0, list1(e1)); }
obj_t list3(obj_t e0, obj_t e1, obj_t e2) { return cons(e0, list2(e1, e2)); }
obj_t list4(obj_t e0, obj_t e1, obj_t e2, obj_t e3) { return cons(e0, list3(e1, e2, e3)); }

static struct symbol * if_symbol;
static struct symbol * quote_symbol;

char is_tail(obj_t next) {
  return is_tagged_list(next, make_symbol("return"));
}

obj_t compile(obj_t x, obj_t next) {
  if (is_symbol(x)) {
    return list3(cons_symbol("refer"), x, next);
  } else if (is_pair(x)) {
    if (is_tagged_list(x, quote_symbol)) {
      return list3(cons_symbol("constant"), cadr(x), next);
    } else if (is_tagged_list(x, make_symbol("lambda"))) {
      return list4(cons_symbol("close"), 
		   cadr(x), 
		   compile(caddr(x), list1(cons_symbol("return"))),
		   next);
    } else if (is_tagged_list(x, if_symbol)) {
      obj_t test = cadr(x);
      obj_t on_true = caddr(x);
      obj_t on_false = cadddr(x);
      obj_t truec = compile(on_true, next);
      obj_t falsec = compile(on_false, next);
      return compile(test, list3(cons_symbol("test"), truec, falsec));
    } else {
      obj_t args = cdr(x);
      obj_t c = compile(car(x), list1(cons_symbol("apply")));
    loop:
      if (is_the_empty_list(args)) {
	if (is_tail(next))
	  return c;
	else
	  return list3(cons_symbol("frame"), next, c);
      } else {
	c = compile(car(args), list2(cons_symbol("argument"), c));
	args = cdr(args);
	goto loop;
      }
    }
  }
  return list3(cons_symbol("constant"), x, next);
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
  printf("%s\n", unwrap_symbol(car(x))->value);

  if (is_tagged_list(x, make_symbol("halt"))) {
    return a;
  } else if (is_tagged_list(x, make_symbol("refer"))) {
    obj_t val = lookup(cadr(x), e)->car;
    printf("%lld", val);
    return VM(val, caddr(x), e, r, s);
  } else if (is_tagged_list(x, make_symbol("constant"))) {
    return VM(cadr(x), caddr(x), e, r, s);
  } else if (is_tagged_list(x, make_symbol("close"))) {
    obj_t vars = cadr(x);
    obj_t body = caddr(x);
    obj_t x_ = cadddr(x);
    return VM(closure(body, e, vars), x_, e, r, s);
  } else if (is_tagged_list(x, make_symbol("test"))) {
    return VM(a, (a!=false) ? cadr(x) : caddr(x), e, r, s);
  } else if (is_tagged_list(x, make_symbol("frame"))) {
    return VM(a, caddr(x), e, the_empty_list, call_frame(cadr(x), e, r, s));
  } else if (is_tagged_list(x, make_symbol("argument"))) {
    return VM(a, cadr(x), e, cons(a, r), s);
  } else if (is_tagged_list(x, make_symbol("apply"))) {
    obj_t body = car(a);
    obj_t e = cadr(a);
    obj_t vars = caddr(a);
    return VM(a, body, extend(e, vars, r), the_empty_list, s);
  } else if (is_tagged_list(x, make_symbol("return"))) {
    return VM(a, car(s), cadr(s), caddr(s), cadddr(s));
  } else {
    assert(0);
  }
} 


obj_t eval(obj_t x) {
  //  obj_t fn = make_primitive_proc(is_null_proc);

  obj_t globals = list2(list1(cons_symbol("x")), list1(wrap_fixnum(44)));

  return VM(the_empty_list, 
	    compile(x, list1(cons_symbol("halt"))),
	    list1(globals),
	    the_empty_list,
	    the_empty_list);
}

int main() {
  if_symbol = make_symbol("if");
  quote_symbol = make_symbol("quote");

#if 0
  obj_t expr = list4(wrap_symbol(if_symbol),
		     wrap_fixnum(11),
		     wrap_fixnum(-22),
		     wrap_fixnum(33));
#else
  obj_t expr = list2(list3(cons_symbol("lambda"),
			   list1(cons_symbol("n")),
			   cons_symbol("n")),
		     wrap_fixnum(4));
#endif

  while (1) {
    printf("\n> ");
    write(eval(read(stdin)));
  }

  //obj_t a = eval(expr);


}
