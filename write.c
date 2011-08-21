#include <assert.h>
#include "write.h"
#include "pair.h"

/**************************** PRINT ******************************/


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
    } else if (is_pair(obj)) {
      printf("(");
      write_pair(obj);
      printf(")");
    } else if (is_symbol(obj)) {
      printf("%s", unwrap_symbol(obj)->value);
    } else if (is_string(obj)) {
      printf("\"%s\"", unwrap_string(obj)->value);
    } else if (is_boolean(obj)) {
      printf("#%c", obj==imm_false ? 'f' : 't');
    } else if (is_fixnum(obj)) {
      printf("%lld", unwrap_fixnum(obj));
    } else if (is_thunk(obj)) {
      printf("#<thunk>");
    } else if (is_primitive_proc(obj)) { 
      printf("#<primitive fn>");
    } else if (obj == imm_undefined) {
      printf("#<undefined>");
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
