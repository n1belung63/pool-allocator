#pragma once

#include "pool_allocator_port_api.h"
#include "singelton/singelton.h"

//#include "rtos_library.h"

namespace allocator {

struct PoolAllocatorPort : public Singleton<PoolAllocatorPort>, public IPoolAllocatorPort {
friend class Singleton<PoolAllocatorPort>;
public:
    virtual void AllocatorPortEnterCriticalSection(void) override {
        return;
    }

    virtual void AllocatorPortExitCriticalSection(void) override {
        return;
    }
};

}