#include <stdint.h>
#include <gc.h>
#include <czmq.h>
#include <assert.h>

// based on http://www.eecs.usma.edu/webs/people/okasaki/ml98maps.sml

typedef uint64_t im_key_t;
typedef void* im_value_t;

typedef struct im {
  char tag;  // 0 = leaf, 1 = branch
  union {
    struct {
      im_key_t key;
      im_value_t val;
    } leaf;
    
    struct {
      im_key_t prefix;
      im_key_t branching_bit;
      struct im *left;
      struct im *right;
    } branch;
  } u;
} *im_t;

im_t im_empty();
im_value_t im_lookup(im_key_t key, im_t dict);
im_t im_insert(im_key_t key, im_value_t val, im_t dict);

im_t im_empty() { return NULL; }

im_key_t mask(im_key_t k, im_key_t m) { return k & (m-1); }
im_key_t lowest_bit(im_key_t x) { return x & (0-x); }
im_key_t branching_bit(im_key_t p0, im_key_t p1) { return lowest_bit(p0 ^ p1); }
im_key_t zero_bit(im_key_t k, im_key_t m) { return (k&m) == 0; }
im_key_t match_prefix(im_key_t k, im_key_t p, im_key_t m) { return mask(k,m) == p; }

im_value_t im_lookup(im_key_t key, im_t dict) {
  if (!dict) { return NULL; }


  if (dict->tag == 0) {
    // leaf
    // printf("lookup: %llu  (%llu  %p)\n", key, dict->u.leaf.key, dict->u.leaf.val);
    if (dict->u.leaf.key == key) { return dict->u.leaf.val; } else { return NULL; }
  } else {
    // branch
    //   printf("lookup: %llu  (%llu  %llu  %p  %p)\n", key, dict->branch.prefix,
    //     dict->branch.branching_bit, dict->branch.left, dict->branch.right);
    if (zero_bit(key, dict->u.branch.branching_bit)) {
      return im_lookup(key, dict->u.branch.left);
    } else {
      return im_lookup(key, dict->u.branch.right);
    }
  }
}

im_t make_branch(im_key_t p, im_key_t m, im_t t0, im_t t1) {
  im_t node = (im_t)malloc(sizeof(struct im));
  node->tag = 1;
  node->u.branch.prefix = p;
  node->u.branch.branching_bit = m;
  node->u.branch.left = t0;
  node->u.branch.right = t1;
  return node;
}

im_t make_leaf(im_key_t k, im_value_t v) {
  im_t leaf = (im_t)malloc(sizeof(struct im));
  leaf->tag = 0;
  leaf->u.leaf.key = k;
  leaf->u.leaf.val = v;
  return leaf;
}

im_t im_join(im_key_t p0, im_t t0, im_key_t p1, im_t t1) {
  // combine two trees with prefixes p0 and p1, where p0 and p1 are known
  // to disagree
  im_key_t m = branching_bit(p0, p1);
  im_t branch = (im_t)malloc(sizeof(struct im));

  if (zero_bit(p0, m)) {
    return make_branch(mask(p0, m), m, t0, t1);
  } else {
    return make_branch(mask(p0, m), m, t1, t0);
  }
}

im_t im_insert(im_key_t key, im_value_t val, im_t dict) {
  if (!dict) { return make_leaf(key, val); }

  if (dict->tag == 0) {
    // leaf
    if (dict->u.leaf.key == key) {
      assert(0); // need a way to resolve the conflict
    } else {
      return im_join(key, make_leaf(key, val), dict->u.leaf.key, dict);
    }

  } else {
    // branch
    im_key_t p = dict->u.branch.prefix;
    im_key_t m = dict->u.branch.branching_bit;
    im_t t0 = dict->u.branch.left;
    im_t t1 = dict->u.branch.right;

    if (match_prefix(key, p, m)) {
      if (zero_bit(key, m)) {
        return make_branch(p, m, im_insert(key, val, t0), t1);
      } else {
        return make_branch(p, m, t0, im_insert(key, val, t1));
      }
    } else {
      return im_join(key, make_leaf(key, val), p, dict);
    }
  }
  assert(0);
}


int main() {
  zlist_t *l = zlist_new();

  char *msg = "Hello World\n";
  char *msg2 = "Goodbye World\n";

  im_t im = im_empty();
  im = im_insert(3, msg, im);
  im = im_insert(444, msg2, im);
  im = im_insert(1, msg2, im);
  im = im_insert(3452345234523451, msg, im);

  assert(im_lookup(3, im) == msg);
  assert(im_lookup(444, im) == msg2);
  assert(im_lookup(1, im) == msg2);
  assert(im_lookup(3452345234523451, im) == msg);

  for (int i=0; i<10000; i++) {
    int x = rand();
    im = im_insert(x, msg, im);
    assert(im_lookup(x, im) == msg);
  }


  return 0;
}
