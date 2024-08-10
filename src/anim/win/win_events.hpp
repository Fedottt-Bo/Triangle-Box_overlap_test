/* Copyright 2024 Fedor Borodulin

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* "win_events.hpp" - window events and state file */

#ifndef __win_events_hpp__
#define __win_events_hpp__

#include <def.h>

#include <bitset>
#include <variant>

/* Animation namespace // windows related namespace */
namespace anim::win
{
  /* Window events namespace */
  namespace events
  {
    /* Resize event */
    struct resize
    {
      uint32_t NewWidth, NewHeight; // new size (zero indicates collapsing)
      bool LastResize;              // Indicates that this resize event is the final size
    }; /* end of 'resize' structure */
  
    /* Button state change event */
    struct button
    {
      uint8_t Index; // Button index
      bool State;    // New state
    }; /* end of 'button' structure */
  
    /* Mouse moving event */
    struct mouse
    {
      float MouseDX, MouseDY;     // Mouse move value
      float MouseNewX, MouseNewY; // Mouse new position
    }; /* end of 'mouse' structure */
  
    using any = std::variant<
        resize,
        button,
        mouse
      >;
  } /* end of 'events' namespace */

  /* Window state data storage */
  class window_state
  {
    friend class window;

  public:
    /* State data */
    uint32_t Width {0}, Height {0};   // Window size
    std::bitset<0x100> Keys {};       // Pressed keys mask
    float MouseX {0.f}, MouseY {0.f}; // Mouse window-relative coordinates 

    /* Empty constructor */
    window_state( void )
    {
    } /* End of constructor */
  }; /* end of 'window_state' class */
} /* end of 'win' namespace */

#endif /* __win_events_hpp__ */

/* END OF 'win_events.hpp' FILE */
