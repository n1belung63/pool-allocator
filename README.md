# pool-allocator
## Модуль блочного аллокатора

Принцип работы блочного аллокатора основан на разделении выделяемого объёма памяти на более мелкие сегменты одинакового размера. При выделении памяти он просто возвращает один из свободных сегментов фиксированного размера. А при освобождении памяти он сохраняет этот сегмент для дальнейшего использования.

## Использование

Чтобы применить блочный аллокатор в проекте, нужно добавить этот репозиторий в качестве подмодуля в git. Также необходимо создать в файл pool_allocator_port.h и включить в него заголовочный файл библиотеки rtos. В функциях ```AllocatorPortEnterCriticalSection``` и ```AllocatorPortExitCriticalSection``` нужно прописать вызов функций из добавленной библиотеки, которые будут выполнять вход и выход из критической секции.

```cpp
#pragma once
#include "pool_allocator_port_api.h"
#include "singelton.h"
// include rtos library with EnterCriticalSection and LeaveCriticalSection functions

namespace allocator {
struct PoolAllocatorPort : public Singleton<PoolAllocatorPort>, public IPoolAllocatorPort {
friend class Singleton<PoolAllocatorPort>;
public:
    virtual void AllocatorPortEnterCriticalSection(void) override {
        // add EnterCriticalSection function from rtos library
    }
    virtual void AllocatorPortExitCriticalSection(void) override {
        // add LeaveCriticalSection function from rtos library
    }
};
}
```

Объект аллокатора создаётся в единственном виде при помощи паттерна Singleton при следующем вызове:
```cpp
constexpr uint32_t block_size = 8;
constexpr uint32_t block_count = 10;
PoolAllocator<block_size,block_count,PoolAllocatorPort>& allocator = 
    PoolAllocator<block_size,block_count,PoolAllocatorPort>::GetInstance();
```

Блочный аллокатор позволяет:
- выделять блок памяти (аргумент ```size_t size``` используется для унификации с ```std::allocator()```):
```cpp
void* allocate(size_t size);
```
- высвобождать блок памяти (аргумент ```size_t size``` используется для унификации с ```std::allocator()```)::
```cpp
void deallocate(void *ptr, size_t size);
```
- возвращать адрес пула блоков:
```cpp
void* GetPoolBaseAddress(void);
```
- возвращать количество свободных блоков:
```cpp
int GetFreeBlocksCount(void);
```
- освобождать все блоки в пуле:
```cpp
void FreeAllBlocks(void);
```

## Сборка

Для сборки проекта используется CMake (версия не ниже 3.27). Чтобы собрать проект, необходимо выполнить следующие команды в командной строке, находясь в директории с проектом:

```
mkdir build
cd build
cmake ../ -G "Ninja"
cmake --build .
```

## Запуск и проверка работы

Проверка работы осуществляется с помощью фреймворка googletest. Тесты запускаются следующей командой из командной строки, находясь в директории с проектом:

```
build\test\Test.exe
```