#ifndef __CHAPTER_1_BITMAP_H_
#define __CHAPTER_1_BITMAP_H_
#include <stdio.h>

static inline int __find_last_bit(size_t word)
{
	int bits = 0;

	while (word >>= 1) {
		bits++;
	}

	return bits;
}	

#define BITPERTYPE(type) (sizeof(type) << 3)
#define SHIFTTYPE(type) (__find_last_bit(sizeof(type)) + 3)
#define MASKTYPE(type) ((1UL << BITPERTYPE(type)) - 1)

#define REGISTER_BITMAP_OPS(type) \
static inline int get_##type##_array_length(size_t nums) \
{ \
	return (nums / BITPERTYPE(type)) + 1;\
} \
static inline void set_bit_##type(type *array, int i)\
{ \
	array[i >> SHIFTTYPE(type)] |= (1 << (i & MASKTYPE(type))); \
} \
static inline void clear_bit_##type(type *array, int i)\
{ \
	array[i >> SHIFTTYPE(type)] &= ~(1 << (i & MASKTYPE(type))); \
} \
static inline int test_bit_##type(type *array, int i)\
{ \
	return !!(array[i >> SHIFTTYPE(type)] & (1 << (i & MASKTYPE(type)))); \
}

#endif /* __CHAPTER_1_BITMAP_H_ */
