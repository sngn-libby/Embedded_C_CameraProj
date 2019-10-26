#include "option.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <locale.h>

#define STACK_BASE		BASE_OF_STACK
#define STACK_LIMIT		END_OF_STACK

#define __HEAP_BASE__ 	HEAP_BASE_ADDRESS
#define __HEAP_LIMIT__ 	HEAP_END_ADDRESS

static caddr_t heap = (caddr_t)__HEAP_BASE__;

caddr_t _sbrk (int size)
{
	caddr_t prevHeap;
	caddr_t nextHeap;

	if (heap == NULL)
	{
		heap = (caddr_t)__HEAP_BASE__;
	}

	prevHeap = heap;
	nextHeap = (caddr_t)(((unsigned int)(heap + size) + 7) & ~7);

	if((unsigned int)nextHeap >= __HEAP_LIMIT__)
	{
		return (caddr_t)NULL;
	}

	heap = nextHeap;

	return prevHeap;}

unsigned int Get_Stack_Base(void)
{
	return STACK_BASE;
}

unsigned int Get_Stack_Limit(void)
{
	return STACK_LIMIT;
}

unsigned int Get_Heap_Base(void)
{
	return __HEAP_BASE__;
}

unsigned int Get_Heap_Limit(void)
{
	return __HEAP_LIMIT__;
}

void Run_Time_Config(void)
{
	setlocale(LC_ALL, "C");
}
