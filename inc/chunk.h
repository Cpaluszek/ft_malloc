#ifndef chunk_h
# define chunk_h

#include <stdint.h>
#include <stddef.h>

// #define chunk_header_size (offsetof(chunk, body))
#define CHUNK_HEADER_SIZE (sizeof(chunk))

#define prev_inuse      0x1
#define is_mmapped      0x2
#define non_main_arena  0x4

#define size_bits (prev_inuse | is_mmapped | non_main_arena)

typedef struct s_chunk chunk;

#define next_chunk(p) ((chunkptr)((char*)(p) + ((p)->size & ~size_bits)))

typedef struct s_chunk {
    // uint64_t prev_size;     // size of prev chunk, if it is free
    uint64_t size;          // size in bytes, including overhead
    struct s_chunk* next;
    int is_free;
} chunk;

typedef struct s_chunk* chunkptr;

#endif
