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

#include "SI.h"
#include "SI_hardware.h"

#include <io/IO.h>
#include <io/IO_display.h>
#include <io/IO_font.h>
#include <io/IO_utils.h>
#include <io/IO_malloc.h>

#include <string.h>

//------------------------------------------------------------------------------
// Scene variables
//------------------------------------------------------------------------------
uint32_t lives = 3;
uint32_t score = 0;
uint32_t level = 1;

//------------------------------------------------------------------------------
// Bitmaps
//------------------------------------------------------------------------------
extern const IO_bitmap BunkerDamagedImg;
extern const IO_bitmap BunkerImg;
extern const IO_bitmap DefenderImg;
extern const IO_bitmap HeartImg;
extern const IO_bitmap Invader1Img;
extern const IO_bitmap Invader2Img;
extern const IO_bitmap Invader3Img;
extern const IO_bitmap Invader4Img;

const char MissleUpImgData[] = {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const IO_bitmap MissleUpImg = {3, 6, 1, (void *)MissleUpImgData};

const char MissleDownImgData[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1};
const IO_bitmap MissleDownImg = {3, 6, 1, (void *)MissleDownImgData};

//------------------------------------------------------------------------------
// Game objects
//------------------------------------------------------------------------------
static SI_object_bitmap  defender_obj;
static SI_object         score_obj;
static SI_object_bitmap  life_obj[3];
static SI_object_bitmap  invader_obj[5];
static SI_object_bitmap  bunker_obj[3];
static SI_object_bitmap  missle_obj[6];

//------------------------------------------------------------------------------
// Game object types
//------------------------------------------------------------------------------
#define SI_DEFENDER  1
#define SI_INVADER   2
#define SI_BUNKER    3
#define SI_MISSLE    4

//------------------------------------------------------------------------------
// Draw score
//------------------------------------------------------------------------------
static void game_scene_draw_score(SI_object *obj, IO_io *display)
{
  const IO_font *font = IO_font_get_by_name("SilkScreen8");
  IO_display_set_font(display, font);
  IO_display_cursor_goto(display, 0, 0);
  IO_print(display, "%u", score);
}

//------------------------------------------------------------------------------
// Compute new positions of objects
//------------------------------------------------------------------------------
static void game_scene_pre_render(SI_scene *scene)
{
  uint32_t defx = display_attrs.width - defender_obj.obj.width;
  defx *= slider_value;
  defx /= 4095;
  defender_obj.obj.x = defx;

  //----------------------------------------------------------------------------
  // Defender missle
  //----------------------------------------------------------------------------
  if(button_value && !(missle_obj[0].obj.flags & SI_OBJECT_VISIBLE)) {
    missle_obj[0].obj.y = display_attrs.height - 10;
    missle_obj[0].obj.x = defender_obj.obj.x + 4;
    missle_obj[0].obj.flags |= SI_OBJECT_VISIBLE;
    button_value = 0;
  }
  else if(missle_obj[0].obj.flags & SI_OBJECT_VISIBLE) {
    if(missle_obj[0].obj.y <= 6)
      missle_obj[0].obj.flags &= ~SI_OBJECT_VISIBLE;
    else
      missle_obj[0].obj.y -= 2;
  }

  //----------------------------------------------------------------------------
  // Invader missle
  //----------------------------------------------------------------------------
  for(int i = 0; i < 5; ++i) {
    uint8_t shoot = IO_random() % 20;
    if(shoot && !(missle_obj[i+1].obj.flags & SI_OBJECT_VISIBLE) &&
       (invader_obj[i].obj.flags & SI_OBJECT_VISIBLE)) {
      missle_obj[i+1].obj.y  = 8 + invader_obj[i].obj.height;
      uint16_t x_off = invader_obj[i].obj.width - missle_obj[i+1].obj.width;
      x_off /= 2;
      missle_obj[i+1].obj.x = invader_obj[i].obj.x + x_off;
      missle_obj[i+1].obj.flags |= SI_OBJECT_VISIBLE;
    }
    else if(missle_obj[i+1].obj.flags & SI_OBJECT_VISIBLE) {
      if(missle_obj[i+1].obj.y >= display_attrs.height - 6)
        missle_obj[i+1].obj.flags &= ~SI_OBJECT_VISIBLE;
      else
        missle_obj[i+1].obj.y += 2;
    }
  }

  button_value = 0;
}

//------------------------------------------------------------------------------
// React to object collisions
//------------------------------------------------------------------------------
static void game_scene_collision(SI_object *obj1, SI_object *obj2)
{
  switch(obj2->user_flags) {
    case SI_INVADER:
      score += 25;
      obj2->flags &= ~SI_OBJECT_VISIBLE;
      obj1->flags &= ~SI_OBJECT_VISIBLE;
      break;

    case SI_DEFENDER:
      obj1->flags &= ~SI_OBJECT_VISIBLE;
      if(lives) {
        --lives;
        life_obj[lives].obj.flags &= ~SI_OBJECT_VISIBLE;
        if(lives == 0)
          set_active_scene(SI_SCENE_INTRO);
      }
      break;

    case SI_MISSLE:
      obj2->flags &= ~SI_OBJECT_VISIBLE;
      obj1->flags &= ~SI_OBJECT_VISIBLE;
      break;

    case SI_BUNKER:
    {
      SI_object_bitmap *bunker = CONTAINER_OF(SI_object_bitmap, obj, obj2);
      if(bunker->bmp == &BunkerDamagedImg)
        obj2->flags &= ~SI_OBJECT_VISIBLE;
      else
        SI_object_bitmap_cons(bunker, &BunkerDamagedImg);
      obj1->flags &= ~SI_OBJECT_VISIBLE;
      break;
    }
  }
}

//------------------------------------------------------------------------------
// Set up the scene
//------------------------------------------------------------------------------
void game_scene_setup(SI_scene *scene)
{
  memset(scene, 0, sizeof(SI_scene));
  if(!scene->num_objects) {
    scene->objects = IO_malloc(19*sizeof(SI_object *));
    scene->num_objects = 19;
  }

  memset(&defender_obj, 0, sizeof(defender_obj));
  SI_object_bitmap_cons(&defender_obj, &DefenderImg);
  defender_obj.obj.y = display_attrs.height - defender_obj.obj.height;
  defender_obj.obj.flags = SI_OBJECT_VISIBLE;
  defender_obj.obj.user_flags = SI_DEFENDER;
  scene->objects[0] = &defender_obj.obj;

  memset(&score_obj, 0, sizeof(score_obj));
  score_obj.draw = game_scene_draw_score;
  score_obj.flags = SI_OBJECT_VISIBLE;
  scene->objects[1] = &score_obj;

  memset(life_obj, 0, sizeof(life_obj));
  for(int i = 0; i < 3; ++i) {
    SI_object_bitmap_cons(&life_obj[i], &HeartImg);
    life_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    life_obj[i].obj.x = display_attrs.width+1 - (i+1)*(HeartImg.width+1);
    scene->objects[i+2] = &life_obj[i].obj;
  }

  const IO_bitmap *invader_img = 0;
  switch(level) {
    case 1: invader_img = &Invader1Img; break;
    case 2: invader_img = &Invader2Img; break;
    case 3: invader_img = &Invader3Img; break;
    case 4: invader_img = &Invader4Img; break;
  }
  memset(invader_obj, 0, sizeof(invader_obj));
  for(int i = 0; i < 5; ++i) {
    SI_object_bitmap_cons(&invader_obj[i], invader_img);
    invader_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    invader_obj[i].obj.user_flags = SI_INVADER;
    invader_obj[i].obj.y = 8;
    invader_obj[i].obj.x = i*(invader_img->width+1);
    scene->objects[i+5] = &invader_obj[i].obj;
  }

  memset(bunker_obj, 0, sizeof(bunker_obj));
  uint16_t bunker_area = display_attrs.width/3;
  uint16_t bunker_offset = (bunker_area - BunkerImg.width)/2;
  for(int i = 0; i < 3; ++i) {
    SI_object_bitmap_cons(&bunker_obj[i], &BunkerImg);
    bunker_obj[i].obj.flags = SI_OBJECT_VISIBLE;
    bunker_obj[i].obj.user_flags = SI_BUNKER;
    bunker_obj[i].obj.y = display_attrs.height - 8;
    bunker_obj[i].obj.x = i*bunker_area + bunker_offset;;
    scene->objects[i+10] = &bunker_obj[i].obj;
  }

  memset(missle_obj, 0, sizeof(missle_obj));
  SI_object_bitmap_cons(&missle_obj[0], &MissleUpImg);
  missle_obj[0].obj.flags = SI_OBJECT_TRACKABLE;
  missle_obj[0].obj.user_flags = SI_MISSLE;
  scene->objects[13] = &missle_obj[0].obj;

  for(int i = 1; i < 6; ++i) {
    SI_object_bitmap_cons(&missle_obj[i], &MissleDownImg);
    missle_obj[i].obj.flags = SI_OBJECT_TRACKABLE;
    missle_obj[i].obj.user_flags = SI_MISSLE;
    scene->objects[13+i] = &missle_obj[i].obj;
  }

  scene->pre_render = game_scene_pre_render;
  scene->collision  = game_scene_collision;
  scene->fps   = 25;
  scene->flags = SI_SCENE_RENDER;
}