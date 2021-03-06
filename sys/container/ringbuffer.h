//
//  ringbuffer.h
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

#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include <types.h>

typedef struct
{
	uint8_t *buffer;
	uint8_t *bufferLimit;

	uint8_t *begin;
	uint8_t *end;

	size_t size;
	size_t count;
} ringbuffer_t;

ringbuffer_t *ringbuffer_create(size_t size);
void ringbuffer_destroy(ringbuffer_t *ringbuffer);

void ringbuffer_write(ringbuffer_t *ringbuffer, uint8_t *data, size_t size);
size_t ringbuffer_read(ringbuffer_t *ringbuffer, uint8_t *buffer, size_t size);

size_t ringbuffer_length(ringbuffer_t *ringbuffer);

#endif /* _RINGBUFFER_H_ */
