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
// IO flags
//------------------------------------------------------------------------------
#define IO_NONBLOCKING 0x0001

//------------------------------------------------------------------------------
// Initialize the IO modules
//------------------------------------------------------------------------------
int32_t IO_init();

//------------------------------------------------------------------------------
// IO definition
//------------------------------------------------------------------------------
struct IO_io {
  int32_t (*write)(struct IO_io *io, const void *data, uint32_t length);
  int32_t (*read)(struct IO_io *io, void *data, uint32_t length);
  void *data;
  uint16_t flags;
};

typedef struct IO_io IO_io;

//------------------------------------------------------------------------------
// Write to an IO device
//------------------------------------------------------------------------------
int32_t IO_write(IO_io *io, const void *data, uint32_t length);

//------------------------------------------------------------------------------
// Print to an IO device - similar to printf
//------------------------------------------------------------------------------
int32_t IO_print(IO_io *io, const char *format, ...);

//------------------------------------------------------------------------------
// Read from and IO device
//------------------------------------------------------------------------------
int32_t IO_read(IO_io *io, void *data, uint32_t length);

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
#define IO_STRING 0
#define IO_INT32  1
#define IO_INT64  2
#define IO_UINT32 3
#define IO_UINT64 4
#define IO_DOUBLE 5

//------------------------------------------------------------------------------
// Read and parse data from the input
//
// @param io    the io device
// @param type  type of data to by read
// @param data  buffer for the data
// @param param length of the buffer when IO_STRING is requested or base
//              of the integer when an IO_UINT type is requested
// @return      number of bytes read from the device or an error
//------------------------------------------------------------------------------
int32_t IO_scan(IO_io *io, uint8_t type, void *data, uint32_t param);
