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

#include <io/IO.h>
#include <io/IO_font.h>

//------------------------------------------------------------------------------
// Object flags
//------------------------------------------------------------------------------
#define SI_OBJECT_VISIBLE 1
#define SI_OBJECT_HIDDEN  0
#define SI_OBJECT_BITMAP  1
#define SI_OBJECT_TEXT    2
#define SI_OBJECT_DYNAMIC 3

//------------------------------------------------------------------------------
//! A scene object
//------------------------------------------------------------------------------
struct SI_object {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint8_t  type;
  uint8_t  flags;
};

typedef struct SI_object SI_object;

//------------------------------------------------------------------------------
//! A bitmap
//------------------------------------------------------------------------------
struct SI_object_bitmap {
  SI_object        obj;
  const IO_bitmap *bmp;
};

typedef struct SI_object_bitmap SI_object_bitmap;

static inline void SI_object_set_bitmap(SI_object_bitmap *obj, const IO_bitmap *bmp)
{
  obj->bmp        = bmp;
  obj->obj.width  = bmp->width;
  obj->obj.height = bmp->height;
  obj->obj.type   = SI_OBJECT_BITMAP;
}

//------------------------------------------------------------------------------
//! A text object
//------------------------------------------------------------------------------
struct SI_object_text {
  SI_object   obj;
  const char *text;
  IO_font    *font;
};

typedef struct SI_object_text SI_object_text;

//------------------------------------------------------------------------------
//! A dynamic object drawn directly to the screen
//------------------------------------------------------------------------------
struct SI_object_dynamic {
  SI_object obj;
  void (*draw)(IO_io *display, struct SI_object_dynamic *obj);
};

typedef struct SI_object_dynamic SI_object_dynamic;

//------------------------------------------------------------------------------
// Scene flags
//------------------------------------------------------------------------------
#define SI_SCENE_IGNORE 0
#define SI_SCENE_RENDER 1

//------------------------------------------------------------------------------
//! Scene descriptor
//------------------------------------------------------------------------------
struct SI_scene {
  SI_object **objects;                                       //!< list of object pointers
  void       *data;                                          //!< user data
  void      (*pre_render)(struct SI_scene *);                //!< called before rendering
  void      (*collision)(SI_object *obj1, SI_object *obj2);  //!< collision callback
  uint8_t     fps;                                           //!< frames per second
  uint8_t     num_objects;                                   //!< number of objects
  uint8_t     flags;                                         //!< flags
};

typedef struct SI_scene SI_scene;

//------------------------------------------------------------------------------
//! Rendere scene on the display
//------------------------------------------------------------------------------
void SI_scene_render(SI_scene *scene, IO_io *display, IO_io *timer);