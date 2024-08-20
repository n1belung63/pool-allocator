#include <iostream>
#include <cstdint>

#include "pool_allocator.h"
#include "pool_allocator_port.h"
#include "stl_adaptor.h"

using namespace allocator;

constexpr uint32_t block_size = 512;
constexpr uint32_t block_count = 4;

template <typename T>
struct Buffer {
    T data[block_size] = {0};

    static PoolAllocator<block_size*sizeof(T),block_count> allocator;

    static void *operator new(size_t size) {
        return allocator.allocate(size);
    }

    static void operator delete(void *ptr, size_t size) {
        return allocator.deallocate(ptr, size);
    }
};

PoolAllocatorPort port;

template <typename T>
PoolAllocator<block_size*sizeof(T),block_count> Buffer<T>::allocator(port);

int main(int argc, char const *argv[]) {
    auto fill {
        [&](uint8_t data[block_size], uint8_t shift) {
            for (int i = 0; i < block_size; ++i) {
                data[i] = static_cast<uint8_t>(i) + shift;
            }
        }
    };
    auto print {
        [&](std::string name, uint8_t data[block_size]) {
            std::cout << name << " elements: ";
            std::cout << std::endl;
            for (int i = 0; i < block_size; ++i) {
                std::cout << std::hex <<  static_cast<int>(data[i]) << " ";
            }
            std::cout << std::endl;
        }
    };

    Buffer<uint8_t>* buf1 = new Buffer<uint8_t>();\
    fill(buf1->data, 10);
    print("buf1", buf1->data);

    Buffer<uint8_t>* buf2 = new Buffer<uint8_t>();
    fill(buf2->data, 20);
    print("buf2", buf2->data);

    {
        Buffer<uint8_t>* buf3 = new Buffer<uint8_t>();
        fill(buf3->data, 30);
        print("buf3", buf3->data);

        Buffer<uint8_t>* buf4 = new Buffer<uint8_t>();
        fill(buf4->data, 40);
        print("buf4", buf4->data);

        delete buf3;
        delete buf4;
    }

    Buffer<uint8_t>* buf5 = new Buffer<uint8_t>();
    fill(buf5->data, 50);
    print("buf5", buf5->data);

    Buffer<uint8_t>* buf6 = new Buffer<uint8_t>();
    fill(buf6->data, 60);
    print("buf6", buf6->data);
}
