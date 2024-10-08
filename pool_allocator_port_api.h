#pragma once

#include <cstdint>

namespace allocator {

static constexpr uint32_t DEFAULT_ALIGNMENT = 4;

static constexpr uint32_t DefaultBlockSize = 512;
static constexpr uint32_t DefaultBlockCount = 4;

class IPoolAllocatorPort {
public:
    virtual void AllocatorPortEnterCriticalSection(void) = 0;
    virtual void AllocatorPortExitCriticalSection(void) = 0;
};

}