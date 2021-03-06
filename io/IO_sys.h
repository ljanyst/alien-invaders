//------------------------------------------------------------------------------
// Copyright (c) 2016 by Lukasz Janyst <lukasz@jany.st>
//------------------------------------------------------------------------------
// This file is part of silly-invaders.
//
// silly-invaders is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// silly-invaders is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with silly-invaders.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#pragma once

#include <stdint.h>

//------------------------------------------------------------------------------
//! Enable interrupts
//------------------------------------------------------------------------------
void IO_enable_interrupts();

//------------------------------------------------------------------------------
//! Disable interrupts
//------------------------------------------------------------------------------
void IO_disable_interrupts();

//------------------------------------------------------------------------------
//! Wait for an interrupt
//------------------------------------------------------------------------------
void IO_wait_for_interrupt();

//------------------------------------------------------------------------------
//! Semaphore
//------------------------------------------------------------------------------
typedef int32_t IO_sys_semaphore;

//------------------------------------------------------------------------------
//! Thread control block
//------------------------------------------------------------------------------
struct IO_sys_thread {
  uint32_t             *stack_ptr;
  uint32_t              flags;
  void (*func)();
  struct IO_sys_thread *next;
  uint32_t              sleep;
  IO_sys_semaphore     *blocker;
  uint8_t               priority;
};

typedef struct IO_sys_thread IO_sys_thread;

//------------------------------------------------------------------------------
//! Register a thread
//!
//! @param thread     thread control block
//! @param func       thread function
//! @param stack_size size of the stack
//! @param priority   thread priority
//------------------------------------------------------------------------------
int32_t IO_sys_thread_add(IO_sys_thread *thread, void (*func)(),
  uint32_t stack_size, uint8_t priority);

//------------------------------------------------------------------------------
//! Run the operating system
//!
//! @param time_slice timeslice in microseconds
//------------------------------------------------------------------------------
int32_t IO_sys_run(uint32_t time_slice);

//------------------------------------------------------------------------------
//! Yield the CPU
//------------------------------------------------------------------------------
void IO_sys_yield();

//------------------------------------------------------------------------------
//! Sleep
//!
//! @param time number of miliseconds to sleep for
//------------------------------------------------------------------------------
void IO_sys_sleep(uint32_t time);

//------------------------------------------------------------------------------
//! Initialize the semaphore
//!
//! @param sem semaphore
//! @param val initial value
//------------------------------------------------------------------------------
void IO_sys_semaphore_init(IO_sys_semaphore *sem, int32_t val);

//------------------------------------------------------------------------------
//! Signal
//!
//! @param sem semaphore
//------------------------------------------------------------------------------
void IO_sys_signal(IO_sys_semaphore *sem);

//------------------------------------------------------------------------------
//! Wait
//!
//! @param sem semaphore
//------------------------------------------------------------------------------
void IO_sys_wait(IO_sys_semaphore *sem);
