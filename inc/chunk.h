#ifndef chunk_h
# define chunk_h

#include <stdint.h>
#include <stddef.h>

#define CHUNK_HEADER_SIZE sizeof(chunk)

#define SIZE_MASK (~0xF)
#define FLAG_IN_USE 0x1
#define FLAG_IS_MMAP 0x2

typedef struct s_chunk chunk;

typedef struct s_chunk {
    uint64_t size;
    struct s_chunk* next;
} chunk;

typedef struct s_chunk* chunkptr;

void chunk_add_back(chunkptr* lst, chunkptr new);
chunkptr get_chunk_from_ptr(void* ptr);

void set_chunk_in_use(chunkptr c);
void set_chunk_free(chunkptr c);
int is_chunk_free(chunkptr c);

void set_chunk_mmap(chunkptr c);
int is_chunk_mmap(chunkptr c);

size_t get_chunk_size(chunkptr c);

#endif
