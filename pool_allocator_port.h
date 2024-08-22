#pragma once

#include "pool_allocator_port_api.h"
#include "singelton/singelton.h"
//#include "rtos_library.h"

namespace allocator {

static constexpr uint32_t DEFAULT_ALIGNMENT = 4;
	
static constexpr uint32_t DefaultBlockSize = 512;
static constexpr uint32_t DefaultBlockCount = 4;

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