Copyright Razvan-Constantin Rus 312CAa 2022-2023

## Description
A vitual memory allocator is responsible for reserving memory at the library level. It's mark certain memory areas from a pre-allocated pool of bytes, known as an arena, as being in use. Additionally, the memory allocator handles the release of reserved zones.


## Usage
The input is provided from stdin, and the output is displayed to stdout, following the following format:

1. `ALLOC_ARENA <arena_size>` - A contiguous buffer of size is allocated to be used as a kernel buffer or arena. The allocation is purely virtual, meaning this buffer is used only to simulate the existence of a physical space for the memory-linked zones.
2. `DEALLOC_ARENA` - Deallocates the arena allocated at the beginning of the program. Since the arena allocation was virtual, this command results in freeing all used resources, such as the list of blocks and the associated miniblock lists.
3. `ALLOC_BLOCK <arena_address> <block_size>` - Marks a zone starting at address <arena_address> in the kernel buffer as reserved, with a size of <block_size>.
    * If no address within the memory zone [arena_address, arena_address + size) has been previously allocated and there are no allocated memory zones [x, arena_address - 1) and [arena_address + size + 1, y], where x < arena_address - 1 and y > arena_address + size + 1, then a new block is inserted into the list of allocated zones.
    * If no address within the memory zone [arena_address, arena_address + size) has been previously allocated and there are allocated memory zones [x, arena_address - 1) or [arena_address + size + 1, y], where x < arena_address - 1 and y > arena_address + size + 1, then the adjacent blocks are removed from the list of allocated zones and added to the internal list of miniblocks for the new contiguous memory zone. In other words, adjacent memory zones will always be in the same block.
  
4. `FREE_BLOCK <arena_address>` - Frees a miniblock.
    * If the only miniblock within a block is freed, then the block itself is freed.
    * If a miniblock is freed from the beginning/end of the block, the structure of the block remains unchanged.
    * If a miniblock from the middle of the block is freed, it will be split into two distinct blocks.

5. `READ <arena_address> <size>` - Displays <size> bytes starting from address <arena_address>, and ends with \n.
    * If the block does not contain <size> bytes starting from address <arena_address>, "Warning: size was bigger than the block size. Reading <size> characters.\n" will be displayed, and the available data will be shown.

6. `WRITE <arena_address> <size> <data>` - Writes <size> bytes from <data> to the address <arena_address>.
    * If <data> does not contain <size> bytes on the same line, reading will continue until the specified size is reached.
    * If the block does not contain <size> bytes starting from address <arena_address>, "Warning: size was bigger than the block size. Writing <size> characters.\n" will be displayed.
7. `PMAP` - Traditionally, the pmap() call is used to visualize memory zones used by a process.
    Information about existing blocks and miniblocks is displayed.
    The permission format is RWX. If any of these permissions are missing, they will be replaced with -.

