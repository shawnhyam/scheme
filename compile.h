#ifndef COMPILE_H_
#define COMPILE_H_

#include "obj.h"

any compile(any obj);

any car(any obj);
any cdr(any obj);
#define cadr(x) car(cdr(x))


#endif
