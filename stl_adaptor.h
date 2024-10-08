#pragma once

#include "pool_allocator.h"

namespace memory {

template<typename T, typename TAlloc>
class StlAdaptor {
public:
    typedef T value_type; 
    
    StlAdaptor() = delete;

    StlAdaptor(TAlloc& allocator)
        : allocator_(allocator)
        { }  

    template<typename U>
    StlAdaptor(const StlAdaptor<U, TAlloc>& other)
        : allocator_(other.allocator_)
        { }

    [[nodiscard]] constexpr T* allocate(std::size_t n) {
        return reinterpret_cast<T*>(allocator_.allocate(n * sizeof(T)));
    }

    constexpr void deallocate(T* p, std::size_t n) {
        allocator_.deallocate(p, n * sizeof(T));
    }

    bool operator==(const StlAdaptor<T,TAlloc>& rhs) const {
        return allocator_.GetPoolBaseAddress() == rhs.allocator_.GetPoolBaseAddress();
    }

    bool operator!=(const StlAdaptor<T,TAlloc>& rhs) const {
        return !(*this == rhs);
    }

private:
    TAlloc& allocator_; 
};

}