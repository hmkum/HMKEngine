#pragma once
#include <malloc.h>
#include <cstdlib>

template<size_t Alignment>
class AlignedAllocation
{
public:
	static void* operator new(size_t size)
	{
#ifdef _WIN32
		return _aligned_malloc(size, Alignment);
#elif __linux__
        void *mem = malloc(size + Alignment + sizeof(void*));
        void **ptr = (void**)((long)(mem + Alignment + sizeof(void*)) & ~(Alignment-1));
        ptr[-1] = mem;
        return ptr;
#else
#error Platform not supported!
#endif
	}

	static void operator delete(void *memory)
	{
#ifdef _WIN32
		_aligned_free(memory);
#elif __linux__
        free(((void**)memory)[-1]);
#else
#error Platform not supported!
#endif
	}
};
