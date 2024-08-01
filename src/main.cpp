/* "main.cpp" - Program entry point file */

#include "def.h"

#include "anim/anim.hpp"

/* Program entry point */
int main( int ArgC, char *ArgV[] )
{
  try
  {
    anim::anim Anim {};
  }
  catch (std::runtime_error &)
  {
    return 1;
  }
  catch (...)
  {
    return -1;
  }

  return 0;
} /* End of 'main' function */

/* END OF 'main.cpp' FILE */
