#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

// types: fixnum, bool, string, symbol, primitive procedure, environment, ...

// basic structure of things, patterned after Larcency
// ref: http://larceny.ccs.neu.edu/doc/LarcenyNotes/note2-repr.html

// 64-bit implementation (lower 32 bits shown)
// xxxx xxxx xxxx xxxx xxxx xxxx xxxx x000   fixnum
// xxxx xxxx xxxx xxxx xxxx xxxx xxxx x001   immediate
// pppp pppp pppp pppp pppp pppp pppp p010   unused
// pppp pppp pppp pppp pppp pppp pppp p011   unused
// pppp pppp pppp pppp pppp pppp pppp p100   pointer to pair
// pppp pppp pppp pppp pppp pppp pppp p101   pointer to bytevector struct
// pppp pppp pppp pppp pppp pppp pppp p110   unused (pointer to closure)
// pppp pppp pppp pppp pppp pppp pppp p111   pointer to primitive procedure


// immediates; lowest 2 bits must be 01; next 6 bits are the type
// 0000 00--   #t
// 0000 01--   #f
// 0000 10--   empty list
// 0000 11--   character
// 0001 00--   miscellaneous (#!undefined, #!eof, etc.)


// bytevector struct headers (64 bits)
// size: n bits
// type: n bits (string, symbol, ...)
// ref count: ?? optional
// 


// procedure struct header
// 



// functions to operate on primitive types

typedef int64_t any;
typedef struct { int64_t value; } fixnum_t;
typedef struct { char value; } boolean_t;
typedef struct { char *value; } string_t;
typedef struct { char *value; } symbol_t;
typedef struct pair {
  any car;
  any cdr;
} *pair_t;

typedef struct environment {
  size_t sz;
  struct environment *prev_frame;
  symbol_t *vars;
  any *vals;
} *environment_t;

typedef any (*proc2_t)(any, any);

any tag_fixnum(fixnum_t);
fixnum_t untag_fixnum(any);
fixnum_t make_fixnum(int64_t);
char is_fixnum(any);
void write_fixnum(fixnum_t, FILE*);

any tag_boolean(boolean_t);
boolean_t untag_boolean(any);
boolean_t make_boolean(char);
char is_boolean(any);
void write_boolean(boolean_t, FILE*);

any tag_string(string_t);
string_t untag_string(any);
string_t make_string(char*);
char is_string(any);
void write_string(string_t, FILE*);

any tag_symbol(symbol_t);
symbol_t untag_symbol(any);
symbol_t make_symbol(char*);
char is_symbol(any);
char equal_symbols(symbol_t s1, symbol_t s2) { return strcmp(s1.value, s2.value) == 0; }
void write_symbol(symbol_t, FILE*);

any tag_pair(pair_t);
pair_t untag_pair(any);
pair_t make_pair(any, any);
char is_pair(any);
void write_pair(pair_t, FILE*);

any tag_environment(environment_t);
environment_t untag_environment(any);
environment_t make_environment(size_t sz, environment_t prev);
char is_environment(any);
void write_environment(environment_t, FILE*);

any tag_proc(proc2_t);
proc2_t untag_proc(any);
char is_proc(any);
void write_proc(proc2_t, FILE*);



void write(any obj, FILE *f) {
  if (is_fixnum(obj)) {
    write_fixnum(untag_fixnum(obj), f);
  } else if (is_string(obj)) {
    write_string(untag_string(obj), f);
  } else if (is_symbol(obj)) {
    write_symbol(untag_symbol(obj), f);
  } else if (is_proc(obj)) {
    write_proc(untag_proc(obj), f);
  } else {
    assert(0);
  }
}

//

any tag_fixnum(fixnum_t n) { return n.value << 3; }
fixnum_t untag_fixnum(any obj) { 
  fixnum_t result = { obj>>3 };
  return result;
}
fixnum_t make_fixnum(int64_t n) { 
  fixnum_t result = { n };
  return result; 
}
char is_fixnum(any obj) { return (obj&0x7) == 0; }
void write_fixnum(fixnum_t n, FILE *f) {
  fprintf(f, "%lld", n.value);
}

any tag_boolean(boolean_t n) { return (n.value<<3) + 3; }
boolean_t untag_boolean(any obj) { 
  boolean_t result = { obj>>3 };
  return result;
}
boolean_t make_boolean(char n) { 
  boolean_t result = { n };
  return result; 
}
char is_boolean(any obj) { return (obj&0x7) == 3; }


any tag_string(string_t s) {  
  assert(((int64_t)s.value & 0x7) == 0);
  return (int64_t)s.value+1;
}
string_t untag_string(any obj) {
  string_t result = { (char*)(obj-1) };
  return result;
}
string_t make_string(char* s) {
  // should probably copy this memory
  string_t r = { .value = (char*)malloc(strlen(s)+1) };
  strcpy(r.value, s);
  return r;
}
char is_string(any obj) {
  return (obj & 0x7) == 1;
}

void write_string(string_t s, FILE *f) {
  char *str = s.value;
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
} 

any tag_symbol(symbol_t s) {
  assert(((int64_t)s.value & 0x7) == 0);
  return (int64_t)s.value+4;
}
symbol_t untag_symbol(any obj) {
  symbol_t result = { (char*)(obj-4) };
  return result;
}
symbol_t make_symbol(char* s) {
  // FIXME should probably copy this memory
  // FIXME must intern these strings
  symbol_t result = { s };
  return result;
}
char is_symbol(any obj) {
  return (obj & 0x7) == 4;
}
void write_symbol(symbol_t s, FILE *f) {
  fprintf(f, "%s", s.value);
}


any tag_pair(pair_t p) {
  assert(((int64_t)p & 0x7) == 0);
  return (int64_t)p + 2;
}
pair_t untag_pair(any obj) {
  return (pair_t)(obj-2);
}
pair_t make_pair(any car, any cdr) {
  pair_t pair = (pair_t)malloc(sizeof(struct pair));
  pair->car = car;
  pair->cdr = cdr;
  return pair;
}
char is_pair(any obj) {
  return (obj & 0x7) == 2;
}


any tag_environment(environment_t e) {
  assert((int64_t)e & 0x7 == 0);
  return (int64_t)e + 5;  
}
environment_t untag_environment(any obj) {
  return (environment_t)(obj-5);
}
environment_t make_environment(size_t sz, environment_t prev) {
  environment_t env = (environment_t)malloc(sizeof(struct environment));
  env->sz = sz;
  env->prev_frame = prev;
  env->vars = (symbol_t*)malloc(sizeof(symbol_t)*sz);
  env->vals = (any*)malloc(sizeof(any)*sz);
  return env;
}
void bind(symbol_t sym, any obj, environment_t env, size_t idx) {
  env->vars[idx] = sym;
  env->vals[idx] = obj;
}

char is_environment(any obj) {
  return (obj & 0x7) == 5;
}
void write_environment(environment_t e, FILE* f) {
  assert(0);
}

any tag_proc(proc2_t p) {
  return (int64_t)p + 6;    
}
proc2_t untag_proc(any obj) {
  return (proc2_t)(obj-6);
}
char is_proc(any obj) {
  return (obj & 0x7) == 6;
}
void write_proc(proc2_t p, FILE* f) {
  fprintf(f, "<#!primfn>");
}


// constants: #f, #t, '()

static any the_empty_list = 0x34;
boolean_t false = { 0 };
boolean_t true = { 1 };
symbol_t quote_symbol;




// list functions

any cons(any car, any cdr) {
  return tag_pair(make_pair(car, cdr));
}

any car(any pair) { return untag_pair(pair)->car; }
any cdr(any pair) { return untag_pair(pair)->cdr; }

#define cadr(x) car(cdr(x))
#define caddr(x) car(cdr(cdr(x)))

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

any read_character(FILE *in) {
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
                //return make_character(' ');
                assert(0);
            }
            break;
        case 'n':
            if (peek(in) == 'e') {
                eat_expected_string(in, "ewline");
                peek_expected_delimiter(in);
                //return make_character('\n');
                assert(0);
            }
            break;
    }
    peek_expected_delimiter(in);
    assert(0);
    // return make_character(c);
}

any read(FILE *in);

any read_pair(FILE *in) {
    int c;
    any car_obj;
    any cdr_obj;
    
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

any read(FILE *in) {
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
              return tag_boolean(true);
            case 'f':
              return tag_boolean(false);
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
            return tag_fixnum(make_fixnum(num));
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
            return tag_symbol(make_symbol(buffer));
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
        return tag_string(make_string(buffer));
    }
    else if (c == '(') { /* read the empty list or pair */
        return read_pair(in);
    }
    else if (c == '\'') { /* read quoted expression */
      return cons(tag_symbol(quote_symbol), cons(read(in), the_empty_list));
    }
    else {
        fprintf(stderr, "bad input. Unexpected '%c'\n", c);
        exit(1);
    }
    fprintf(stderr, "read illegal state\n");
    exit(1);
}

/// EVAL

any lookup_variable_value(symbol_t sym, environment_t env) {
  for (int i=0; i<env->sz; i++) {
    if (equal_symbols(sym, env->vars[i])) {
      return env->vals[i];
      }
  }
}

any eval(any exp, environment_t env) {
  if (is_symbol(exp)) {
    return lookup_variable_value(untag_symbol(exp), env);
  } else if (is_fixnum(exp)) {
    return exp;
  } else if (is_pair(exp)) {
    any proc = eval(car(exp), env);

    // TODO check to see if macro

    return untag_proc(proc)(eval(cadr(exp), env),
                            eval(caddr(exp), env));
  }

}



any add(any n1, any n2) {
  return n1+n2;
}


int main() {
  printf("Welcome to Bootstrap Scheme. "
         "Use ctrl-c to exit.\n");

  environment_t env = make_environment(4, NULL);
  bind(make_symbol("x"), tag_fixnum(make_fixnum(44)), env, 0);
  bind(make_symbol("+"), tag_proc(&add), env, 1);
  write(env->vals[0], stdout);

  while (1) {
    printf("> ");
    write(eval(read(stdin), env), stdout);
    printf("\n");
  }

  return 0;

}
