#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "scheme.h"
#include "pair.h"

struct pair * lookup(obj_t var, obj_t e) {
 nxtrib: {
  obj_t vars = caar(e);
  obj_t vals = cdar(e);
  
 nxtelt:
  if (vars == imm_empty_list) {
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
    else if (cdr_obj == imm_empty_list) { 
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
    
    if (obj == imm_empty_list) {
      printf("()");
    } else if (is_boolean(obj)) {
      printf("#%c", obj==imm_false ? 'f' : 't');
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
        return imm_empty_list;
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
                return imm_true;
            case 'f':
                return imm_false;
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
            return mk_symbol(buffer);
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
      return cons(mk_symbol("quote"), cons(read(in), imm_empty_list));
    }
    else {
        fprintf(stderr, "bad input. Unexpected '%c'\n", c);
        exit(1);
    }
    fprintf(stderr, "read illegal state\n");
    exit(1);
}

//////

char is_tagged_list(obj_t expr, obj_t tag) {
  if (is_pair(expr)) {
    if (is_symbol(car(expr))) {
      return car(expr) == tag;
    }
  }
  return 0;
}


obj_t if_symbol;
obj_t quote_symbol;

char is_tail(obj_t next) {
  return is_tagged_list(next, mk_symbol("return"));
}

obj_t compile(obj_t x, obj_t next) {
  if (is_symbol(x)) {
    return list3(mk_symbol("refer"), x, next);
  } else if (is_pair(x)) {
    if (is_tagged_list(x, quote_symbol)) {
      return list3(mk_symbol("constant"), cadr(x), next);
    } else if (is_tagged_list(x, mk_symbol("lambda"))) {
      return list4(mk_symbol("close"), 
		   cadr(x), 
		   compile(caddr(x), list1(mk_symbol("return"))),
		   next);
    } else if (is_tagged_list(x, if_symbol)) {
      obj_t test = cadr(x);
      obj_t on_true = caddr(x);
      obj_t on_false = cadddr(x);
      obj_t truec = compile(on_true, next);
      obj_t falsec = compile(on_false, next);
      return compile(test, list3(mk_symbol("test"), truec, falsec));
    } else {
      obj_t args = cdr(x);
      obj_t c = compile(car(x), list1(mk_symbol("apply")));
    loop:
      if (args == imm_empty_list) {
	if (is_tail(next))
	  return c;
	else
	  return list3(mk_symbol("frame"), next, c);
      } else {
	c = compile(car(args), list2(mk_symbol("argument"), c));
	args = cdr(args);
	goto loop;
      }
    }
  }
  return list3(mk_symbol("constant"), x, next);
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

  if (is_tagged_list(x, mk_symbol("halt"))) {
    return a;
  } else if (is_tagged_list(x, mk_symbol("refer"))) {
    obj_t val = lookup(cadr(x), e)->car;
    printf("%lld", val);
    return VM(val, caddr(x), e, r, s);
  } else if (is_tagged_list(x, mk_symbol("constant"))) {
    return VM(cadr(x), caddr(x), e, r, s);
  } else if (is_tagged_list(x, mk_symbol("close"))) {
    obj_t vars = cadr(x);
    obj_t body = caddr(x);
    obj_t x_ = cadddr(x);
    return VM(closure(body, e, vars), x_, e, r, s);
  } else if (is_tagged_list(x, mk_symbol("test"))) {
    return VM(a, (a!=imm_false) ? cadr(x) : caddr(x), e, r, s);
  } else if (is_tagged_list(x, mk_symbol("frame"))) {
    return VM(a, caddr(x), e, imm_empty_list, call_frame(cadr(x), e, r, s));
  } else if (is_tagged_list(x, mk_symbol("argument"))) {
    return VM(a, cadr(x), e, cons(a, r), s);
  } else if (is_tagged_list(x, mk_symbol("apply"))) {
    obj_t body = car(a);
    obj_t e = cadr(a);
    obj_t vars = caddr(a);
    return VM(a, body, extend(e, vars, r), imm_empty_list, s);
  } else if (is_tagged_list(x, mk_symbol("return"))) {
    return VM(a, car(s), cadr(s), caddr(s), cadddr(s));
  } else {
    assert(0);
  }
} 


obj_t eval(obj_t x) {
  obj_t globals = list2(list1(mk_symbol("x")), list1(wrap_fixnum(44)));

  return VM(imm_empty_list, 
	    compile(x, list1(mk_symbol("halt"))),
	    list1(globals),
	    imm_empty_list,
	    imm_empty_list);
}

int main() {
  if_symbol = mk_symbol("if");
  quote_symbol = mk_symbol("quote");

  while (1) {
    printf("\n> ");
    write(eval(read(stdin)));
  }

}
