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

#ifndef __IO_IMPL__
#error "This is a low-level header for driver implementation purposes only"
#endif

#include "IO_sys.h"
#include <stdint.h>

//------------------------------------------------------------------------------
//! Initialize systick
//!
//! @param time_slice time slice in microseconds
//------------------------------------------------------------------------------
void IO_sys_tick_init(uint32_t time_slice);

//------------------------------------------------------------------------------
//! Start the system
//!
//! @param time_slice timeslice in microseconds
//------------------------------------------------------------------------------
void IO_sys_start(uint32_t time_slice);

//------------------------------------------------------------------------------
//! Schedule the next thread to run
//------------------------------------------------------------------------------
void IO_sys_schedule();

//------------------------------------------------------------------------------
//! Initialize the stack
//!
//! @param thread     the thread control block
//! @param func       the thread function
//! @param arg        the argument to the thread functinon
//! @param stack      the stack
//! @param stack_size size of the stack
//------------------------------------------------------------------------------
void IO_sys_stack_init(IO_sys_thread *thread, void (*func)(void *), void *arg,
  void *stack, uint32_t stack_size);

//------------------------------------------------------------------------------
//! Timer tick to be called every milisecond
//!
//! @param time current time
//------------------------------------------------------------------------------
void IO_sys_timer_tick(uint64_t time);
