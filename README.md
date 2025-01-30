# ft_malloc

This project involves creating a dynamic memory allocation mechanism by implementing custom versions of the `malloc`, `free` and `realloc` functions, along with a memory visualization utility.

## Permitted functions
- `mmap(2)`
- `munmap(2)`
- `sysconf(_SC_PAGESIZE)`
- `getrlimit(2)`
- functions from libpthread

## Mandatory Part
Implement a custom dynamic memory allocation library, including the following functions:
- `malloc(size_t size)` - Allocates memory of the `size` and returns a pointer.
- `realloc(void *ptr, size_t size)` - Resizes the allocation at `ptr` to `size`. If insufficient space is available, it creates a new allocation, copies data, frees the old allocation, and returns a pointer to the new memory.
- `free(void *ptr)` - Deallocates the memory at `ptr`. If `ptr` is `NULL`, no operation is performed.
- `show_alloc_mem()` - Displays the current memory allocations, grouped into **TINY**, **SMALL**, and **LARGE** zones.

### Requirements
- Optimize for performance by minimizing system calls and pre-allocating memory zones.
- Define **TINY**, **SMALL**, and **LARGE** categories, each with at least 100 allocations.
- Align allocated memory properly.

## Bonus Part
1. **Thread Safety** using `libpthread`.
2. **Debugging Tools** by introducing debugging environment variables or custom debugging utilities.
3. **Extended Visualization** to display allocation history or perform memory dumps.
4. **Memory defragmentation**


## Concepts
**Heap / Zone**
The "heap" is a contiguous region of memory managed by the allocator.
- Grows dynamically during program execution.
- Can be fragmented over time due to the allocation and deallocation of memory.

**Chunk**
A unit of memory within the heap which represents a single allocation or freeable block of memory.
A chunk typically contains:
- Header (metadata): Information about the size and state (allocated or free) of the chunk.
- Payload: The memory returned to the user when malloc is called.


## Memory zone layout - Tiny and small allocations
Each zone contains a header and a list of chunks, some of which may be allocated (A, B, C), and others are free chunks.

1. Zone After Initialization
```
+---------------------------------------------------------------+
| Zone Header | Free Chunk                                      |
+---------------------------------------------------------------+
```
The zone is initialized with only a free chunk available.

2. Zone After One Allocation
```
+---------------------------------------------------------------+
| Zone Header | A | [Free Chunk]                                |
+---------------------------------------------------------------+
```
The first free chunk large enough is used to allocate A.
If the chunk is large enough it will be split.

The remaining space is still available for future allocations.

3. Zone After Multiple Allocations
```
+---------------------------------------------------------------+
| Zone Header | A | B | C | [Free Chunk]                        |
+---------------------------------------------------------------+
```
Chunks A, B, and C are allocated.
The remaining free space is after C.

4. After Freeing Chunk B
```
+---------------------------------------------------------------+
| Zone Header | A | [Free Chunk] | C | [Free Chunk]             |
+---------------------------------------------------------------+
```
Chunk B is now freed.
It is added back to the free list but remains between allocated chunks.

5. After Freeing and Merging Free Chunks
```
+---------------------------------------------------------------+
| Zone Header | A | [Free Chunk]                                |
+---------------------------------------------------------------+
```
Chunk C is freed.
The allocator merges adjacent free chunks into a larger free region.

## Large allocations - Handled separately via `mmap()`
Large allocations are managed separately from zones:
```
+---------------------+    +---------------------+    +---------------------+
| Large Chunk Header | -> | Large Chunk Header | -> | Large Chunk Header |
| Allocated Data     |    | Allocated Data     |    | Allocated Data     |
+---------------------+    +---------------------+    +---------------------+
```
