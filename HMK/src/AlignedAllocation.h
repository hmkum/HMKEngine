#pragma once
#include <malloc.h>

template<size_t Alignment>
class AlignedAllocation
{
public:
	static void* operator new(size_t size)
	{
#ifdef _WIN32
		return _aligned_malloc(size, Alignment);
#elif __linux__
		return memalign(size, Alignment);
#else
#error Platform not supported!
#endif
	}

	static void operator delete(void *memory)
	{
#ifdef _WIN32
		_aligned_free(memory);
#elif __linux__
		free(memory);
#else
#error Platform not supported!
#endif
	}
};
