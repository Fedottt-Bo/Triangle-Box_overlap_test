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

  /* Single shader wrapper */
  class shader
  {
  private:
    /* Shader information */
    std::fs::path Path {};

    /* OpenGL resources */
    uint32_t ShaderPrgId {0};

  public:
    /* Shader constructor.
     * ARGUMENTS:
     *   - Shader root-relative path:
     *       std::fs::path &&SrcPath;
     */
    shader( std::fs::path &&SrcPath ) :
      Path {std::forward<std::fs::path>(SrcPath)}
    {
      /* Shader stage information structure */
      struct stage_desc
      {
        bool Required;                 // Indicates that this stage need for a complete program
        std::fs::path_string_view Ext; // Particular stage file extension
        uint32_t GLShaderIndex;        // OpenGl stage index
      }; /* end of 'stage_desc' structure */

      /* Standard graphics stages descriptions array */
      constexpr static stage_desc StagesDesc[]
      {
        {true,  L".vert", GL_VERTEX_SHADER},
        {false, L".tesc", GL_TESS_CONTROL_SHADER},
        {false, L".tese", GL_TESS_EVALUATION_SHADER},
        {false, L".geom", GL_GEOMETRY_SHADER},
        {true,  L".frag", GL_FRAGMENT_SHADER}
      };

      /* Created stage information structure */
      struct stage_info
      {
        uint32_t GLObjectId {0}; // Created resource object id
        std::string Source {};   // Shader source temporary storage
        bool Success {false};    // Creation success flag
      }; /* end of 'stage_info' structure */

      /* Created stages */
      stage_info Stages[std::size(StagesDesc)] {};

      std::wostringstream DebugOutput {};

      /* Go through all stages */
      bool Fail {false};

      for (size_t i {0}; !Fail && i < std::size(StagesDesc); i++)
      {
        const auto &Desc {StagesDesc[i]};
        auto &Stage {Stages[i]};

        /* Construct path */
        std::fs::path StagePath {Path};
        StagePath.replace_extension(Desc.Ext);

        /* Try to load source */
        {
          std::error_code Err;
          size_t Size {std::fs::file_size(StagePath, Err)};

          if (!Err)
          {
            Stage.Source.resize(Size);

            std::ifstream {StagePath}.read(Stage.Source.data(), Size);
          }
        }

        /* Skip/stop if failed to read source */
        if (Stage.Source.empty())
        {
          if (Desc.Required)
          {
            Fail = true;
            DebugOutput << std::format(L"Failed to read stage {} from {}, it was required, aborting.\n", i, StagePath.c_str());
          }

          continue;
        }

        /* Create shader object and try to compile source */
        std::pair<char *, int> TmpStr
        {
          Stage.Source.data(),
          (int)std::min(Stage.Source.size(), (size_t)std::numeric_limits<int>::max())
        };
        glShaderSource(Stage.GLObjectId = glCreateShader(Desc.GLShaderIndex),
                       1, &TmpStr.first, &TmpStr.second);
        glCompileShader(Stage.GLObjectId);

        /* Check errors */
        {
          int Success {0};
          glGetShaderiv(Stage.GLObjectId, GL_COMPILE_STATUS, &Success);

          if (!Success)
          {
            if (Desc.Required)
            {
              Fail = true;
              DebugOutput << std::format(L"Failed to compile stage {} from {}, it was required, aborting. Error message:\n", i, StagePath.c_str());
            }
            else
              DebugOutput << std::format(L"Failed to compile stage {} from {}. Error message:\n", i, StagePath.c_str());

            /* Get error message */
            {
              int LogSize {0};
              glGetShaderiv(Stage.GLObjectId, GL_INFO_LOG_LENGTH, &LogSize);

              std::string Tmp {};
              Tmp.resize(LogSize);

              glGetShaderInfoLog(Stage.GLObjectId, LogSize, &LogSize, Tmp.data());

              std::wstring WTmp {};

              LogSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, Tmp.c_str(), (int)Tmp.size(), nullptr, 0);
              WTmp.resize(LogSize);
              MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, Tmp.c_str(), (int)Tmp.size(), WTmp.data(), LogSize);

              DebugOutput << WTmp;
            }

            /* Destroy shader object */
            glDeleteShader(Stage.GLObjectId);
            Stage.GLObjectId = 0;

            continue;
          }
        }

        /* Set stage success flag */
        Stage.Success = true;
      }

      std::wcout << DebugOutput.rdbuf()->str();

      return;
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

  /* Shader loading manager */
  class shaders_manager
  {
  private:
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
     *   - Shader root-relative path (copy is intended):
     *       std::fs::path Path;
     * RETURNS:
     *   (shader &) Loaded shader.
     */
    shader &Load( std::fs::path Path )
    {
      /* Deduce actual file name */
      Path = ShadersRoot / Path;

      if (!Path.has_filename())
      {
        if (!Path.has_parent_path())
          throw std::fs::filesystem_error {"Cannot accept empty file path", Path, {}};

        Path.replace_filename(Path.parent_path().filename());
      }
      else if (Path.has_extension())
        throw std::fs::filesystem_error {"File extension deduces automatically", Path, {}};

      if (auto It {ShadersStorage.find(Path)}; It != ShadersStorage.end())
        return It->second;

      shader Tmp {std::move(Path)};

      auto [It, Flag] {ShadersStorage.emplace(Tmp.GetPath(), std::move(Tmp))};
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
      shader &Shd {Shaders.Load("simple_geom/shd")};

      return;
    } /* End of constructor */

    /* Destructor */
    ~render( void )
    {
    } /* End of destructor */
  }; /* end of 'render' class */
} /* end of 'anim::rnd' namespace */

#endif /* __render_hpp__ */
