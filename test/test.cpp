#include "../pool_allocator.h"
#include "../pool_allocator_port_api.h"
#include "../pool_allocator_port.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace allocator;

class PoolAllocatorPortMock : public IPoolAllocatorPort {
public:
    ~PoolAllocatorPortMock() override = default;
    MOCK_METHOD(void, AllocatorPortEnterCriticalSection, (), (override));
    MOCK_METHOD(void, AllocatorPortExitCriticalSection, (), (override));
};

TEST(PoolAllocatorTest, SimpleAllocation) {
    constexpr uint32_t block_size = 8;
    constexpr uint32_t block_count = 10;

    PoolAllocatorPortMock mock_port;

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count> allocator(mock_port);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    void* block = allocator.allocate(block_size);
    ASSERT_NE(block, nullptr);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    allocator.deallocate(block, block_size);
}

TEST(PoolAllocatorTest, CountFreeBlocksLeft) {
    constexpr uint32_t block_size = 8;
    constexpr uint32_t block_count = 10;

    PoolAllocatorPortMock mock_port;

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count> allocator(mock_port);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);


    int free_blocks = allocator.GetFreeBlocksCount();
    ASSERT_EQ(free_blocks, block_count);

    uint32_t free_blocks_true = 4;

    void* block;
    for (int i = 0; i < block_count - free_blocks_true; ++i) {
        EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
            .Times(1);
        EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
            .Times(1);
        block = allocator.allocate(block_size);
        ASSERT_NE(block, nullptr);
    }

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);
    free_blocks = allocator.GetFreeBlocksCount();
    ASSERT_EQ(free_blocks, free_blocks_true);
}

TEST(PoolAllocatorTest, AllBlocksAllocated) {
    constexpr uint32_t block_size = 8;
    constexpr uint32_t block_count = 10;

    PoolAllocatorPortMock mock_port;

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count> allocator(mock_port);

    void* block;
    for (int i = 0; i < block_count; ++i) {
        EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
            .Times(1);
        EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
            .Times(1);
        block = allocator.allocate(block_size);
        ASSERT_NE(block, nullptr);
    }
    void* last_allocated_block = block;

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(0);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(0);
    EXPECT_DEATH(allocator.allocate(block_size), "No free blocks");

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);
    int free_blocks = allocator.GetFreeBlocksCount();
    ASSERT_EQ(free_blocks, 0);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);
    allocator.deallocate(last_allocated_block, block_size);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);
    free_blocks = allocator.GetFreeBlocksCount();
    ASSERT_EQ(free_blocks, 1);
}

TEST(PoolAllocatorTest, BadAddress) {
    constexpr uint32_t block_size = 8;
    constexpr uint32_t block_count = 10;

    PoolAllocatorPortMock mock_port;

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count> allocator(mock_port);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    void* block = allocator.allocate(block_size);
    ASSERT_NE(block, nullptr);

    EXPECT_DEATH(allocator.deallocate((char*)allocator.GetPoolBaseAddress() - 1, block_size), "Bad address");
    EXPECT_DEATH(allocator.deallocate((char*)allocator.GetPoolBaseAddress() + 1, block_size), "Not aligned");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}