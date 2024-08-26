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

/* "anim.hpp" - Animation implementation file */

#ifndef __anim_hpp__
#define __anim_hpp__

#include <def.h>

// #include "box_triangle_overlap_test.hpp"

#include "win/win.hpp"
#include "render/render.hpp"

namespace anim
{
  /* Scene animation class */
  class animation
  {
  private:
    /* Two main classes - window and render */
    win::window Win;
    rnd::render Render;

    /* Mainloop function */
    void RunMainloop( void )
    {
      bool Run {true};

      while (Run)
        Win.GetEvent([this, &Run]( auto &Event, const win::window_state &State ) -> void
          {
            using event_type = std::remove_cvref_t<decltype(Event)>;

            /* End mainloop on close event */
            if constexpr (std::same_as<event_type, win::events::close>)
            {
              Run = false;
              return;
            }
          }, true);
    } /* End of 'RunMainloop' function */

  public:
    /* The only constructor - default */
    animation( void ) :
      Win {L"Triangle-Box overlap test output window"},
      Render {Win}
    {
      class test_primitive : public rnd::render::primitive
      {
      public:
        /* Class constructor from parent render instance
         * ARGUMENTS:
         *   - Parent reference:
         *       render &Parent;
         */
        test_primitive( rnd::render &Render ) :
          rnd::render::primitive {Render, rnd::render::primitive::type::eTransparent}
        {
        } /* End of constructor */

        /* Primitive render function
         * ARGUMENTS: None.
         * RETURNS: None.
         */
        void Render( void ) override
        {
        } /* End of 'Render' function */
      } TestPrim {Render};

      RunMainloop();
    } /* end of constructor */

    /* Destructor */
    ~animation( void )
    {
    } /* end of destructor */
  }; /* end of 'animation' class */
} /* end of 'anim' namespace */

#endif /* __anim_hpp__ */

/* END OF 'anim.hpp' FILE */
