#include "../pool_allocator.h"
#include "../pool_allocator_port_api.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace allocator;

class PoolAllocatorPortMock : public Singleton<IPoolAllocatorPort>, public IPoolAllocatorPort {
friend class Singleton<IPoolAllocatorPort>;
public:
    MOCK_METHOD(void, AllocatorPortEnterCriticalSection, (), (override));
    MOCK_METHOD(void, AllocatorPortExitCriticalSection, (), (override));
};

TEST(PoolAllocatorTest, SimpleAllocation) {
    constexpr uint32_t block_size = 8;
    constexpr uint32_t block_count = 10;

    PoolAllocatorPortMock& mock_port = Singleton<PoolAllocatorPortMock>::GetInstance();

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);

    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count,PoolAllocatorPortMock>& allocator = 
        PoolAllocator<block_size,block_count,PoolAllocatorPortMock>::GetInstance();

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

    PoolAllocatorPortMock& mock_port = Singleton<PoolAllocatorPortMock>::GetInstance();

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count,PoolAllocatorPortMock>& allocator = 
        PoolAllocator<block_size,block_count,PoolAllocatorPortMock>::GetInstance();

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

    PoolAllocatorPortMock& mock_port = Singleton<PoolAllocatorPortMock>::GetInstance();

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count,PoolAllocatorPortMock>& allocator = 
        PoolAllocator<block_size,block_count,PoolAllocatorPortMock>::GetInstance();

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

    PoolAllocatorPortMock& mock_port = Singleton<PoolAllocatorPortMock>::GetInstance();

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count,PoolAllocatorPortMock>& allocator = 
        PoolAllocator<block_size,block_count,PoolAllocatorPortMock>::GetInstance();

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    void* block = allocator.allocate(block_size);
    ASSERT_NE(block, nullptr);

    EXPECT_DEATH(allocator.deallocate((char*)allocator.GetPoolBaseAddress() - 1, block_size), "Bad address");
    EXPECT_DEATH(allocator.deallocate((char*)allocator.GetPoolBaseAddress() + 1, block_size), "Not aligned");
}

TEST(PoolAllocatorTest, AllocateBuffers) {
    constexpr uint32_t block_size = 512;
    constexpr uint32_t block_count = 4;

    auto fill_buffer {
        [&](uint8_t buf[block_size], uint8_t shift) {
            for (int i = 0; i < block_size; ++i) {
                buf[i] = static_cast<uint8_t>(i + shift);
            }
        }
    };

    auto check_buffer {
        [&](uint8_t buf[block_size], uint8_t shift) {
            for (int i = 0; i < block_size; ++i) {
                ASSERT_EQ(buf[i], static_cast<uint8_t>(i + shift));
            }
        }
    };

    PoolAllocatorPortMock& mock_port = Singleton<PoolAllocatorPortMock>::GetInstance();

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    PoolAllocator<block_size,block_count,PoolAllocatorPortMock>& allocator = 
        PoolAllocator<block_size,block_count,PoolAllocatorPortMock>::GetInstance();

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    uint8_t* buf1 = static_cast<uint8_t*>(allocator.allocate(block_size));
    ASSERT_NE(buf1, nullptr);
    fill_buffer(buf1, 10);
    check_buffer(buf1, 10);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    uint8_t* buf2 = static_cast<uint8_t*>(allocator.allocate(block_size));
    ASSERT_NE(buf2, nullptr);
    fill_buffer(buf2, 20);
    check_buffer(buf2, 20);

    {
        EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
            .Times(1);
        EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
            .Times(1);

        uint8_t* buf3 = static_cast<uint8_t*>(allocator.allocate(block_size));
        ASSERT_NE(buf3, nullptr);
        fill_buffer(buf3, 30);
        check_buffer(buf3, 30);

        EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
            .Times(1);
        EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
            .Times(1);

        uint8_t* buf4 = static_cast<uint8_t*>(allocator.allocate(block_size));
        ASSERT_NE(buf4, nullptr);
        fill_buffer(buf4, 40);
        check_buffer(buf4, 40);

        EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
            .Times(1);
        EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
            .Times(1);

        allocator.deallocate(buf3, block_size);

        EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
            .Times(1);
        EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
            .Times(1);
        allocator.deallocate(buf4, block_size);
    }

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    uint8_t* buf5 = static_cast<uint8_t*>(allocator.allocate(block_size));
    ASSERT_NE(buf5, nullptr);
    fill_buffer(buf5, 50);
    check_buffer(buf5, 50);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    uint8_t* buf6 = static_cast<uint8_t*>(allocator.allocate(block_size));
    ASSERT_NE(buf6, nullptr);
    fill_buffer(buf6, 60);
    check_buffer(buf6, 60);

    EXPECT_DEATH(allocator.allocate(block_size), "No free blocks");

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);
    allocator.deallocate(buf6, block_size);

    EXPECT_CALL(mock_port, AllocatorPortEnterCriticalSection())
        .Times(1);
    EXPECT_CALL(mock_port, AllocatorPortExitCriticalSection())
        .Times(1);

    uint32_t* buf7 = static_cast<uint32_t*>(allocator.allocate(10*sizeof(uint32_t)));
    ASSERT_NE(buf7, nullptr);
 }

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}