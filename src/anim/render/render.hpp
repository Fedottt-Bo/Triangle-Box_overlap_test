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

#include <def.h>

#include "../win/win.hpp"

#define GLEW_STATIC 1
#include <glew-2.2.0/include/glew.h>

namespace anim::rnd
{
  /* Render OpenGL core */
  class core
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
    core( win::window &Window )
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

      BindContext();

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

    /* Context rebinding to the current thread function
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    void BindContext( void )
    {
      wglMakeCurrent(WindowDC, GLContext);
    } /* End of 'BindContext' function */

    /* Destructor */
    ~core( void )
    {
      wglMakeCurrent(nullptr, nullptr);

      wglDeleteContext(GLContext);
      ReleaseDC(hWnd, WindowDC);
    } /* End of destructor */
  }; /* end of 'core' class */

  /* Shader loading manager */
  class shaders_manager
  {
  private:
    /* Single shader wrapper */
    class shader
    {
    private:
      std::fs::path Path {};

    public:
      /* Shader constructor.
       * ARGUMENTS:
       *   - Shader root-relative path:
       *       const std::fs::path &Path;
       */
      shader( const std::fs::path &Path ) :
        Path {Path}
      {
      } /* End of constructor */

      /* Shader path getting function
       * ARGUMENTS: None.
       * RETURNS:
       *   (const std::fs::path &) Path constant reference.
       */
      constexpr const std::fs::path &GetPath( void ) const noexcept
      {
        return Path;
      } /* End of 'GetPath' function */
    }; /* end of 'shader' class */

    /* Shaders storage cache */
    std::map<std::reference_wrapper<const std::fs::path>, shader> ShadersStorage {};

    /* Shaders filesystem info */
    std::fs::path ShadersRoot;

  public:
    /* Constructor.
     * ARGUMENTS:
     *   - Shaders search root path:
     *       std::fs::path &&SearchRoot;
     */
    shaders_manager( std::fs::path &&SearchRoot ) :
      ShadersRoot {std::forward<std::fs::path>(SearchRoot)}
    {
    } /* End of constructor */

    /* Shader loading function.
     * ARGUMENTS:
     *   - Shader root-relative path:
     *       path_type Path;
     * RETURNS:
     *   (shader &) Loaded shader.
     */
    shader &Load( const std::fs::path &Path )
    {
      if (auto It {ShadersStorage.find(Path)}; It != ShadersStorage.end())
        return It->second;

      shader Tmp {Path};

      auto [It, Flag] {ShadersStorage.emplace(Path, std::move(Tmp))};
      const_cast<std::remove_const_t<decltype(It->first)> &>(It->first) = It->second.GetPath();

      return It->second;
    } /* End of 'Load' function */
  }; /* end of 'shaders_manager' class */

  /* OpenGL based render */
  class render
  {
  private:
    core Core;
    shaders_manager Shaders;

  public:
    /* Constructor from target window
     * ARGUMENST:
     *   - Target window:
     *       window &Window;
     */
    render( win::window &Window ) :
      Core {Window},
      Shaders {L"./bin/"}
    {
    } /* End of constructor */

    /* Destructor */
    ~render( void )
    {
    } /* End of destructor */
  }; /* end of 'render' class */
} /* end of 'anim::rnd' namespace */

#endif /* __render_hpp__ */
