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

/* "render.hpp" - OpenGL based render implementation file */

#ifndef __render_hpp__
#define __render_hpp__

#include "../win/win.hpp"

#define GLEW_STATIC 1
#include <glew-2.2.0/include/glew.h>

namespace anim::rnd
{
  /* OpenGL based render */
  class render
  {
  private:
    /* Window handles */
    HWND hWnd {};
    HDC WindowDC {};

    /* Render base handles */
    HGLRC GLContext {};

  public:
    /* Constructor from target window
     * ARGUMENST:
     *   - Target window:
     *       window &Window;
     */
    render( win::window &Window )
    {
      auto [hInst, hWnd] {Window.GetHandles()};

      this->hWnd = hWnd;
      WindowDC = GetDC(hWnd);

      PIXELFORMATDESCRIPTOR FmtDesc
      {
        .nSize = sizeof FmtDesc,
        .nVersion = 1,
        .dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL,
        .cColorBits = 32,
        .cDepthBits = 24,
      };

      int FmtIndex {ChoosePixelFormat(WindowDC, &FmtDesc)};
      DescribePixelFormat(WindowDC, FmtIndex, sizeof(FmtDesc), &FmtDesc);
      SetPixelFormat(WindowDC, FmtIndex, &FmtDesc);

      GLContext = wglCreateContext(WindowDC);
      wglMakeCurrent(WindowDC, GLContext);

      if (glewInit() != GLEW_OK)
        throw std::runtime_error {"Failed to initialize OpenGL"};

      glClearColor(.30f, .47f, .8f, 1.f);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glEnable(GL_PRIMITIVE_RESTART);
      glPrimitiveRestartIndex(-1);

#ifdef _DEBUG
      // glEnable(GL_DEBUG_OUTPUT);
      // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      // glDebugMessageCallback(GLDebugOutput, NULL);
      // glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
#endif /* _DEBUG */
    } /* End of constructor */
  }; /* end of 'render' class */
} /* end of 'anim::rnd' namespace */

#endif /* __render_hpp__ */
