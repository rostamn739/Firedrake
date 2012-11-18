//
//  iomodule.h
//  Firedrake
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

#ifndef _IOMODULE_H_
#define _IOMODULE_H_

#include <types.h>
#include "iolibrary.h"

struct io_module_s;

typedef void (*io_module_init_t)();

typedef bool (*io_module_start_t)(struct io_module_s *module);
typedef bool (*io_module_stop_t)(struct io_module_s *module);

typedef struct io_module_s
{
	io_library_t *library;
	char *name;
	spinlock_t lock;

	void *module;
	uint32_t references;

	io_module_start_t start;
	io_module_stop_t stop;
} io_module_t;

io_module_t *io_moduleWithName(const char *name);

void io_moduleRetain(io_module_t *module);
void io_moduleRelease(io_module_t *module);

void io_moduleStop(io_module_t *module);

#endif /* _IOMODULE_H_ */
