//
//  IORuntime.h
//  libio
//
//  Created by Sidney Just
//  Copyright (c) 2012 by Sidney Just
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
//  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
//  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
//  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#ifndef _IORUNTIME_H_
#define _IORUNTIME_H_

#include "IOTypes.h"

extern "C"
{
	void panic(const char *message, ...) __attribute__ ((noreturn));
}

void *operator new(size_t size);
void operator delete(void *addr);

void *operator new[](size_t size);
void operator delete[](void *ptr);

typedef void (*function_ptr_t)(void);
class IOObject;

static inline function_ptr_t __IOObject_memberFunctionToPointer(const IOObject *object, void (IOObject::*func)(void))
{
	union {
		void (IOObject::*fIn)(void);
		uintptr_t fVTOffset;
		function_ptr_t fPFN;
	} map;

	map.fIn = func;

	if(map.fVTOffset & 0x1) 
	{
		// virtual function
		union {
			const IOObject *fObj;
			function_ptr_t **vtablep;
		} u;
		u.fObj = object;

		return *(function_ptr_t *)(((uintptr_t)*u.vtablep) + map.fVTOffset - 1);
	} 

	// Normal member function
	return map.fPFN;
}

#define IOMemberFunctionCast(type, object, func) \
	(type)__IOObject_memberFunctionToPointer(object, (void (IOObject::*)(void))func)

#endif /* _IORUNTIME_H_ */
