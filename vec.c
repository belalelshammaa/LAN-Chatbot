#include "vec.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
// file scope / global variables
/*struct Vec {
  void *b;
  int bucket_size;
  int len;
  int element_size;
};*/
// memcpy and checking before instead of copying byte by byte
int vec_new(struct Vec *vec, int element_size) {
  if (element_size <= 0 || vec == NULL) {
    return -1;
  }
  int bucket_size = 8;
  void *b;
  if ((b = calloc(bucket_size, element_size)) == NULL) {
    return -1;
  };
  vec->b = b;
  vec->bucket_size = bucket_size;
  vec->len = 0;
  vec->element_size = element_size;
  return 0;
}
int vec_from(struct Vec *vec, const void *arr, int element_size, int len) {
  if (arr == NULL || vec == NULL || len <= 0 || element_size <= 0) {
    return -1;
  }
  int bucket_size = (len > 8) ? len * 2 : 8;
  void *b;
  if ((b = calloc(bucket_size, element_size)) == NULL) {
    return -1;
  }
  memcpy(b, arr, len * element_size);
  vec->b = b;
  vec->bucket_size = bucket_size;
  vec->len = len;
  vec->element_size = element_size;
  return 0;
}
int vec_with_capacity(struct Vec *vec, int element_size, int size) {
  if (size <= 0 || element_size <= 0 || vec == NULL) {
    return -1;
  }
  void *b;
  if ((b = calloc(size, element_size)) == NULL) {
    return -1;
  }
  vec->b = b;
  vec->bucket_size = size;
  vec->len = 0;
  vec->element_size = element_size;
  return 0;
}
int vec_push(struct Vec *v, void *val) {
  if (v == NULL || val == NULL) {
    return -1;
  }
  if (v->len >= v->bucket_size) {
    void *new_bucket;
    if ((new_bucket = realloc(v->b, v->bucket_size * 2 * v->element_size)) ==
        NULL) {
      return -1;
    };
    v->bucket_size *= 2;
    v->b = new_bucket;
  }
  char *destination = (char *)v->b + v->len * v->element_size;
  memcpy(destination, val, v->element_size);
  v->len += 1;
  return 0;
}

void vec_free(struct Vec *v) {
  if (v && v->b) {
    free(v->b);
    v->len = 0;
    v->bucket_size = 0;
    v->element_size = 0;
  }
}
int vec_pop(struct Vec *v) {
  if (v == NULL || v->len <= 0) {
    return -1;
  }
  char *destination = (char *)v->b + (v->len - 1) * v->element_size;
  memset(destination, 0, v->element_size);

  if (--v->len < v->bucket_size / 4 && v->len > 8) {
    void *new_bucket;
    if ((new_bucket = realloc(v->b, (v->bucket_size / 2) * v->element_size)) ==
        NULL) {
      return 0;
    }

    v->b = new_bucket;
    v->bucket_size /= 2;
  }
  return 0;
}
int vec_swap_remove(struct Vec *v, size_t index) {
  if (v == NULL || index >= v->len || v->len == 0) {
    return -1;
  }
  char *destination = (char *)v->b + index * (size_t)v->element_size;
  memcpy(destination, v->b + (v->len - 1) * v->element_size, v->element_size);
  return vec_pop(v);
}
