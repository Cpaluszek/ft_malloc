#ifndef chunk_h
# define chunk_h

#include <stdint.h>
#include <stddef.h>

#define CHUNK_HEADER_SIZE sizeof(chunk)

#define SIZE_MASK (~0xF)
#define FLAG_IN_USE 0x1

#define get_size(size) ((size) & SIZE_MASK)
#define is_in_use(size) ((size) & FLAG_IS_FREE)

#define set_is_free(size) ((size) | FLAG_IS_FREE)
#define clear_is_free(size) ((size) & ~FLAG_IS_FREE)

typedef struct s_chunk chunk;

// #define next_chunk(p) ((chunkptr)((char*)(p) + ((p)->size & ~size_bits)))

typedef struct s_chunk {
    uint64_t size;
    struct s_chunk* next;
} chunk;

typedef struct s_chunk* chunkptr;

void chunk_add_back(chunkptr* lst, chunkptr new);

void set_chunk_in_use(chunkptr c);
void set_chunk_free(chunkptr c);
int is_chunk_free(chunkptr c);

size_t get_chunk_size(chunkptr c);

#endif
