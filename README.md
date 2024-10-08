# pool-allocator
## Модуль блочного аллокатора

Принцип работы блочного аллокатора основан на разделении выделяемого объёма памяти на более мелкие сегменты одинакового размера. При выделении памяти он просто возвращает один из свободных сегментов фиксированного размера. А при освобождении памяти он сохраняет этот сегмент для дальнейшего использования.

## Использование

Чтобы применить блочный аллокатор в проекте, нужно добавить этот репозиторий в качестве подмодуля в Git. Также необходимо включить заголовочный файл библиотеки rtos в файл pool_allocator_port.h. В функциях AllocatorPortEnterCriticalSection и AllocatorPortExitCriticalSection нужно прописать вызов функций из добавленной библиотеки, которые будут выполнять вход и выход из критической секции.

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