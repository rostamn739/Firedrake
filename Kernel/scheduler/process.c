//
//  process.c
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

#include <memory/memory.h>
#include "process.h"

extern void _process_setFirstProcess(process_t *process); // Scheduler.c

uint32_t _process_getUniqueID()
{
	uint32_t uid = 0;
	
	bool found;
	do {
		found = true;
		process_t *process = process_getFirstProcess();
		
		while(process)
		{
			if(process->pid == uid)
			{
				found = false;
				break;
			}
			
			process = process->next;
		}
		
		if(!found)
			uid ++;
		
	} while(!found);
	
	return uid;
}

process_t *process_create(thread_entry_t entry)
{
	process_t *process = (process_t *)kalloc(sizeof(process_t));
	if(process)
	{
		process_t *parent = process_getCurrentProcess();
		process->context 		= vm_getKernelContext();

		process->pid 	= PROCESS_NULL;
		process->died 	= false;
		process->parent = parent ? parent->pid : PROCESS_NULL;

		process->mainThread = NULL; // Avoid that thread_create reads a false main thread and tries to attach the new thread to the non existent thread
		process->next 		= NULL;

		// Attach a thread
		thread_create(process, 4096, entry);
		
		// Attach the process to the process chain
		process->pid = _process_getUniqueID();
		if(parent)
		{
			process->next = parent->next;
			parent->next  = process;
		}
		else
			_process_setFirstProcess(process);
	}

	return process;
}

void process_destroy(process_t *process)
{
	thread_t *thread = process->mainThread;
	while(thread)
	{
		// Delete all threads...
		thread_t *next = thread->next;
		thread_destroy(thread);

		thread = next;
	}

	kfree(process);
}