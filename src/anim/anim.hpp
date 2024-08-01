/* "anim.hpp" - Animation implementation file */

#include <def.h>

// #include "box_triangle_overlap_test.hpp"

#include "win.hpp"

namespace anim
{
  /* OpenGL based render */
  class render
  {
  private:
  }; /* end of 'render' class */

  /* Program logic mainloop class */
  class mainloop
  {
  private:
  }; /* end of 'mainloop' class */

  /* Mainloop separate thread wrapper class */
  class mainloop_thread
  {
  private:
  }; /* end of 'mainloop_thread' class */

  /* Scene animation class */
  class anim
  {
  private:
    /* Two main classes - window and render */
    win::window Win;
    render Render;

    /* Mainloop and it's separate thread wrapper */
    mainloop Mainloop;
    mainloop_thread MainloopThread;

  public:
    /* The only constructor - default */
    anim( void ) :
      Win {L"Triangle-Box overlap test output window"},
      Render {},
      Mainloop {},
      MainloopThread {}
    {
    } /* end of constructor */

    /* Destructor */
    ~anim( void )
    {
    } /* end of destructor */
  }; /* end of 'anim' class */
} /* end of 'anim' namespace */
