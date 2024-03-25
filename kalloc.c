#include<stdio.h>
#include<unistd.h>
#include<assert.h>
#include<stdbool.h>

typedef struct Block {
    size_t size;
    struct Block* next;
    bool free;
} Block;

#define BLOCK_SIZE (sizeof(Block))

Block *global = NULL;

Block* find_free_block(Block **last, size_t size) {
    Block* curr = global;

    while (curr && !(curr->free && curr->size >= size)) {
        *last = curr;
        curr = curr->next;
    }

    return curr;
}

void *request_space(Block *last, size_t size) {
    Block *block = sbrk(0);
    void *request = sbrk(size + BLOCK_SIZE);

    assert((void*) block == request);

    if (request == (void*) -1)
        return NULL;

    if (last) last->next = block;

    block->size = size;
    block->next = NULL;
    block->free = false;

    return block;
}

void *kalloc(size_t size) {
    Block *block;

    if (size <= 0) return NULL;

    if (!global) {
        block = request_space(global, size);
        if (!block) return NULL;
        global = block;
        return (void*) (block + 1);
    }

    Block *last = global;
    block = find_free_block(&last, size);

    if (block) {
        block->free = false;
        return (void*) (block + 1);
    }

    block = request_space(last, size);
    if (!block) return NULL;
    return (void*) (block + 1);
}

void free(void *p) {
    if (!p) return;
    Block *block = (Block*) p - 1;
    assert(!block->free);
    block->free = true;
}

int main() {
    int **q = (int**) kalloc(10 * sizeof(int*));

    for (int i = 0; i < 10; i++) {
        q[i] = (int*) kalloc(10 * sizeof(int));

        for (int j = 0; j < 10; j++)
            q[i][j] = (i == j) ? 1 : 0;
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++)
            printf("%d\t", q[i][j]);
        puts("");
    }

    for (int i = 0; i < 10; i++)
        free(q[i]);
    free(q);

    puts("hello");
}
