#pragma once

#include "pool_allocator_port_api.h"

namespace allocator {

class PoolAllocatorPort : public IPoolAllocatorPort {
public:
    void AllocatorPortEnterCriticalSection(void) {

    }

    void AllocatorPortExitCriticalSection(void) {

    }
};

}