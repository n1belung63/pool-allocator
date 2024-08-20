#pragma once

#include <cstdint>

namespace allocator {

class IPoolAllocatorPort {
public:
    virtual ~IPoolAllocatorPort() = default;
    virtual void AllocatorPortEnterCriticalSection(void) = 0;
    virtual void AllocatorPortExitCriticalSection(void) = 0;
};

}