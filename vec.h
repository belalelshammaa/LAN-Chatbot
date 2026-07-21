#ifndef VEC_H
#define VEC_H
#include <stddef.h>
struct Vec {
  void *b;
  int bucket_size;
  int len;
  int element_size;
};
int vec_new(struct Vec *vec, int element_size);
int vec_from(struct Vec *vec, const void *arr, int element_size, int len);
int vec_with_capacity(struct Vec *vec, int element_size, int size);
int vec_push(struct Vec *v, void *val);
int vec_pop(struct Vec *v);
int vec_swap_remove(struct Vec *v, size_t index);
void vec_free(struct Vec *v);

#endif
