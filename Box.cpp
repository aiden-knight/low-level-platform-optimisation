#include "Box.h"
#include "MemoryManager.h"

void* Box::operator new(size_t size)
{
    return ::operator new(size, BOX);
}
