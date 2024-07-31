/* "anim.hpp" - Animation implementation file */

#include "box_triangle_overlap_test.hpp"

#include <Windows.h>
#include <string>

/* Animation namespace */
namespace anim
{
  /* Win32 API based window handler */
  class win
  {
  private:
    /* Base handles */
    HINSTANCE hInst {GetModuleHandleW(nullptr)};
    HWND hWnd;

  public:
    /* Constroctor
     * ARGUMENTS:
     *   - Name of created window:
     *       std::string_view WindowName
     */
    win( std::string_view WindowName )
    {
    } /* End of constructor */
  }; /* end of 'win' class */

  /* OpenGL based render */
  class render
  {
  }; /* end of 'render' class */

  /* Program logic mainloop class */
  class mainloop
  {
  }; /* end of 'mainloop' class */

  /* Mainloop separate thread wrapper class */
  class mainloop_thread
  {
  }; /* end of 'mainloop_thread' class */

  /* Scene animation class */
  class anim
  {
  private:
    /* Two main classes - window and render */
    win Win;
    render Render;

    /* Mainloop and it's separate thread wrapper */
    mainloop Mainloop;
    mainloop_thread MainloopThread;

  public:
    /* The only constructor - default */
    anim( void ) :
      Win {"Triangle-Box overlap test output window"},
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
