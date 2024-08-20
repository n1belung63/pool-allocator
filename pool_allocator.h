#pragma once

#include "pool_allocator_port_api.h"

#include <cstring>
#include <cstdint>
#include <cassert>

namespace allocator {

static constexpr size_t DEFAULT_ALIGNMENT = 4;

struct Block {
    Block *next;
};

template <uint32_t blockSize, uint32_t blockCount>
class PoolAllocator {
public:
    explicit PoolAllocator(IPoolAllocatorPort& port);

    void* allocate(size_t size);
    void deallocate(void *ptr, size_t size);

    void* GetPoolBaseAddress(void);

    int GetFreeBlocksCount(void);

private:
    static constexpr uint32_t block_size_ = blockSize;
    static constexpr uint32_t block_count_ = blockCount;

    char pool_[block_count_][block_size_];
    Block* first_free_block_ = nullptr;
    IPoolAllocatorPort& port_;

    bool IsPowerOfTwo(uintptr_t x);

    template <typename T1, typename T2>
    uintptr_t AlignForward(T1 val, T2 align);
};

template <uint32_t blockSize, uint32_t blockCount>
PoolAllocator<blockSize,blockCount>::PoolAllocator(IPoolAllocatorPort& port) : port_(port) {
    assert(block_size_ >= sizeof(Block) && "Block size is too small");
	assert(block_size_ == AlignForward(block_size_, DEFAULT_ALIGNMENT) && "Block size not aligned");
    port_.AllocatorPortEnterCriticalSection();
    for (int i = 0; i < block_count_; i++) {
        Block *block = reinterpret_cast<Block*>(reinterpret_cast<char*>(&pool_[i][0]));
        block->next = first_free_block_;
        first_free_block_ = block;
    }
    port_.AllocatorPortExitCriticalSection();
}

template <uint32_t blockSize, uint32_t blockCount>
void* PoolAllocator<blockSize,blockCount>::allocate(size_t size) {
    assert(size <= block_size_ && "Block size is too small");
    assert(first_free_block_ != nullptr && "No free blocks");
    port_.AllocatorPortEnterCriticalSection();
    Block *block = first_free_block_;
    first_free_block_ = first_free_block_->next;
    port_.AllocatorPortExitCriticalSection();
    return memset(block, 0, block_size_);
}

template <uint32_t blockSize, uint32_t blockCount>
void PoolAllocator<blockSize,blockCount>::deallocate(void *ptr, size_t size) {
    if (ptr == NULL) {
        return;
    }  
    ptrdiff_t diff = reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(&pool_[0][0]);
    assert(diff >= (ptrdiff_t)0 && (size_t)diff < sizeof(pool_) && "Bad address");
    assert((size_t)diff % (size_t)block_size_ == (size_t)0 && "Not aligned");
    port_.AllocatorPortEnterCriticalSection();
    reinterpret_cast<Block*>(ptr)->next = first_free_block_;
    first_free_block_ = reinterpret_cast<Block*>(ptr);
    port_.AllocatorPortExitCriticalSection();
}

template <uint32_t blockSize, uint32_t blockCount>
void* PoolAllocator<blockSize,blockCount>::GetPoolBaseAddress(void) {
    return (void*)pool_;
}

template <uint32_t blockSize, uint32_t blockCount>
int PoolAllocator<blockSize,blockCount>::GetFreeBlocksCount(void) {
    port_.AllocatorPortEnterCriticalSection();
    int counter = 0;
    Block* block = first_free_block_;
    while (block != nullptr) {
        block = block->next;
        counter++;
    }
    port_.AllocatorPortExitCriticalSection();
    return counter;
}

template <uint32_t blockSize, uint32_t blockCount>
bool PoolAllocator<blockSize,blockCount>::IsPowerOfTwo(uintptr_t x) {
	return (x & (x-1)) == 0;
}

template <uint32_t blockSize, uint32_t blockCount>
template <typename T1, typename T2>
uintptr_t PoolAllocator<blockSize,blockCount>::AlignForward(T1 val, T2 align) {
	uintptr_t a, p, modulo;
	assert(IsPowerOfTwo(reinterpret_cast<uintptr_t>(align)));
	a = align;
	p = val;
	modulo = p & (a-1);
	if (modulo != 0) {
		p += a - modulo;
	}
	return p;
}

}