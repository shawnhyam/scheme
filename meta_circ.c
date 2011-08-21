#include <assert.h>
#include "scheme.h"
#include "read.h"
#include "write.h"
#include "pair.h"


static int gensym_count = 0;




// evaluation

obj_t eval(obj_t exp, obj_t env) {
  if (is_symbol(exp)) {
    
  } else if (is_fixnum(exp) || is_boolean(exp) || is_string(exp)) {
    return exp;
  } else {
    assert(0);
  }

}


int main() {
}
