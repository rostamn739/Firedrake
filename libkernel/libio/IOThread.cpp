//
//  IOThread.cpp
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

#include "IOThread.h"
#include "IORunLoop.h"
#include "IODictionary.h"
#include "IORuntime.h"
#include "IONumber.h"
#include "IOLog.h"

#ifdef super
#undef super
#endif
#define super IOObject

IORegisterClass(IOThread, super);

extern "C"
{
	uint32_t __IOPrimitiveThreadCreate(void *entry, void *arg1, void *arg2);
	uint32_t __IOPrimitiveThreadID();
	void __IOPrimitiveThreadSetName(uint32_t id, const char *name);
	void *__IOPrimitiveThreadAccessArgument(size_t index);
	void __IOPrimitiveThreadDied();
}

static kern_spinlock_t __IOThreadLock = KERN_SPINLOCK_INIT;
static IODictionary *__IOThreadDictionary = 0;


IOThread *IOThread::initWithFunction(IOThread::Function function)
{
	if(super::init())
	{
		_topPool = 0;
		_runLoop = 0;
		_properties = 0;

		_detached = false;
		_entry    = function;
		_lock     = KERN_SPINLOCK_INIT;
	}

	return this;
}

void IOThread::free()
{
	if(_properties)
		_properties->release();

	super::free();
}


IOThread *IOThread::withFunction(IOThread::Function function)
{
	IOThread *thread = IOThread::alloc()->initWithFunction(function);
	if(!thread)
		return 0;

	thread->detach();

	return thread->autorelease();
}

IOThread *IOThread::currentThread()
{
	IOThread *thread = 0;
	IONumber *threadID = IONumber::alloc()->initWithUInt32(__IOPrimitiveThreadID());

	kern_spinlock_lock(&__IOThreadLock);
	thread = (IOThread *)__IOThreadDictionary->objectForKey(threadID);
	kern_spinlock_unlock(&__IOThreadLock);

	threadID->release();
	return thread;
}


void IOThread::__threadEntry()
{
	IOThread *thread = (IOThread *)__IOPrimitiveThreadAccessArgument(0);
	IONumber *threadID = IONumber::alloc()->initWithUInt32(thread->_id);

	kern_spinlock_lock(&thread->_lock);
	kern_spinlock_lock(&__IOThreadLock);
	{
		if(!__IOThreadDictionary)
		{
			__IOThreadDictionary = IODictionary::alloc()->initWithCapacity(5);
			if(!__IOThreadDictionary)
				panic("Couldn't create __IOThreadDictionary!");
		}
		
		__IOThreadDictionary->setObjectForKey(thread, threadID);
	}
	kern_spinlock_unlock(&__IOThreadLock);
	kern_spinlock_unlock(&thread->_lock);

	// Create a run loop for the thread
	thread->_runLoop = IORunLoop::alloc()->init();
	thread->_entry(thread);
	thread->_runLoop->release();


	kern_spinlock_lock(&__IOThreadLock);
	{
		__IOThreadDictionary->removeObjectForKey(threadID);
		threadID->release();
	}
	kern_spinlock_unlock(&__IOThreadLock);

	thread->release();
	__IOPrimitiveThreadDied();
}

void IOThread::detach()
{
	kern_spinlock_lock(&_lock);

	if(!_detached)
	{
		retain();

		_detached = true;
		_id = __IOPrimitiveThreadCreate((void *)&IOThread::__threadEntry, this, 0);
	}

	kern_spinlock_unlock(&_lock);
}

IORunLoop *IOThread::getRunLoop()
{
	return _runLoop;
}

void IOThread::setName(IOString *name)
{
	__IOPrimitiveThreadSetName(_id, name->CString());
}

void IOThread::setPropertyForKey(IOObject *property, IOObject *key)
{
	kern_spinlock_lock(&_lock);

	if(!_detached)
	{
		if(!_properties)
			_properties = IODictionary::alloc()->init();

		_properties->setObjectForKey(property, key);
	}

	kern_spinlock_unlock(&_lock);
}

IOObject *IOThread::propertyForKey(IOObject *key)
{
	kern_spinlock_lock(&_lock);

	if(!_properties)
	{
		kern_spinlock_unlock(&_lock);
		return 0;
	}

	IOObject *property = _properties->objectForKey(key);
	kern_spinlock_unlock(&_lock);

	return property;
}
