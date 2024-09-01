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

    /* Debug output function.
     * ARGUMENTS:
     *   - source APi or device:
     *      UINT Source;
     *   - error type:
     *      UINT Type;
     *   - error message id:
     *      UINT Id, 
     *   - message severity:
     *      UINT severity, 
     *   - message text length:
     *      INT Length, 
     *   - message text:
     *      CHAR *Message, 
     *   - user addon parameters pointer:
     *      VOID *UserParam;
     * RETURNS: None.
     */
    static void APIENTRY glDebugOutput( unsigned Source, unsigned Type, unsigned Id, unsigned Severity,
                                        int Length, const char *Message, const void *UserParam )
    {
      /* Ignore non-significant error/warning codes */
      switch (Id)
      {
      case 131169:
      case 131185:
      case 131218:
      case 131204:
        return;
      }

      std::stringstream Buf {};

      Buf << std::format("Debug message ({}) {}\n", Id, Message);
      switch (Source)
      {
      case GL_DEBUG_SOURCE_API:
        Buf << "Source: API\n";
        break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        Buf << "Source: Window System\n";
        break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER:
        Buf << "Source: Shader Compiler\n";
        break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:
        Buf << "Source: Third Party\n";
        break;
      case GL_DEBUG_SOURCE_APPLICATION:
        Buf << "Source: Application\n";
        break;
      case GL_DEBUG_SOURCE_OTHER:
        Buf << "Source: Other\n";
        break;
      }

      switch (Type)
      {
      case GL_DEBUG_TYPE_ERROR:
        Buf << "Type: Error\n";
        break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        Buf << "Type: Deprecated Behaviour\n";
        break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        Buf << "Type: Undefined Behaviour\n";
        break; 
      case GL_DEBUG_TYPE_PORTABILITY:
        Buf << "Type: Portability\n";
        break;
      case GL_DEBUG_TYPE_PERFORMANCE:
        Buf << "Type: Performance\n";
        break;
      case GL_DEBUG_TYPE_MARKER:
        Buf << "Type: Marker\n";
        break;
      case GL_DEBUG_TYPE_PUSH_GROUP:
        Buf << "Type: Push Group\n";
        break;
      case GL_DEBUG_TYPE_POP_GROUP:
        Buf << "Type: Pop Group\n";
        break;
      case GL_DEBUG_TYPE_OTHER:
        Buf << "Type: Other\n";
        break;
      }
    
      switch (Severity)
      {
      case GL_DEBUG_SEVERITY_HIGH:
        Buf << "Severity: high\n\n";
        break;
      case GL_DEBUG_SEVERITY_MEDIUM:
        Buf << "Severity: medium\n\n";
        break;
      case GL_DEBUG_SEVERITY_LOW:
        Buf << "Severity: low\n\n";
        break;
      case GL_DEBUG_SEVERITY_NOTIFICATION:
        Buf << "Severity: notification\n\n";
        break;
      }
    
      std::cout << Buf.rdbuf()->str();
    } /* End of 'glDebugOutput' function */

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
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(glDebugOutput, NULL);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
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

    /* Frame presentation function
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    void Present( void )
    {
      glFinish();
      wglSwapLayerBuffers(WindowDC, WGL_SWAP_MAIN_PLANE);
    } /* End of 'Present' function */
  }; /* end of 'core' class */

  /* Single shader wrapper */
  class shader
  {
  private:
    /* Shader information */
    std::fs::path Path {};

    /* OpenGL resources */
    int GLShaderPrgId {0};

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

      /* Compile the whole program, if stages are valid */
      if (!Fail)
      {
        GLShaderPrgId = glCreateProgram();

        /* Attach shaders */
        for (const auto &Stage : Stages)
          if (Stage.Success)
            glAttachShader(GLShaderPrgId, Stage.GLObjectId);

        glLinkProgram(GLShaderPrgId);

        /* Detach and delete shaders */
        for (const auto &Stage : Stages)
          if (Stage.Success)
          {
            glDetachShader(GLShaderPrgId, Stage.GLObjectId);
            glDeleteShader(Stage.GLObjectId);
          }

        /* Check success */
        {
          int Success {0};
          glGetProgramiv(GLShaderPrgId, GL_LINK_STATUS, &Success);

          if (!Success)
          {
            Fail = true;

            /* Get error message */
            {
              int LogSize {0};
              glGetProgramiv(GLShaderPrgId, GL_INFO_LOG_LENGTH, &LogSize);

              std::string Tmp {};
              Tmp.resize(LogSize);

              glGetProgramInfoLog(GLShaderPrgId, LogSize, &LogSize, Tmp.data());

              std::wstring WTmp {};

              LogSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, Tmp.c_str(), (int)Tmp.size(), nullptr, 0);
              WTmp.resize(LogSize);
              MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, Tmp.c_str(), (int)Tmp.size(), WTmp.data(), LogSize);

              DebugOutput << WTmp;
            }

            /* Destroy program object */
            glDeleteShader(GLShaderPrgId);
            GLShaderPrgId = 0;
          }
        }
      }

      std::wcout << DebugOutput.rdbuf()->str();

      if (Fail)
        throw std::runtime_error {"Failed to load shader"};
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

    /* Shader program id getting function
     * ARGUMENTS: None.
     * RETURNS:
     *   (int) Program id.
     */
    constexpr int GetId( void ) const noexcept
    {
      return GLShaderPrgId;
    } /* End of 'GetId' function */
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

  /* Render buffer wrapper */
  class buffer
  {
  private:
    uint32_t OpenGLId;  // Resource id
    size_t Size;        // Buffer size

  public:
    /* Buffer type enumerable */
    enum class type
    {
      eUniform,       // Constant data storage
      eStorageRW,     // Big or/and shader modifiable data storage with CPU read/write
      eStorageWrite,  // Big or/and shader modifiable data storage with CPU write
      eStorageRead,   // Big or/and shader modifiable data storage with CPU read
      eStorageStatic, // Big or/and shader modifiable data storage with CPU read/write
      eVertex,        // Vertices data buffer
      eIndex          // Indices data buffer
    }; /* end of 'type' enumerable */

  private:
    type Type; // Type

    /* OpenGL base types */
    uint32_t BufferBindType;
    uint32_t AccessType;

    /* Initialization base
     * ARGUMENTS:
     *   - Initial data pointer (default: nullptr):
     *       const void *Data;
     * RETURNS: None.
     */
    void InitBase( const void *Data = nullptr )
    {
      glCreateBuffers(1, &OpenGLId);

      switch (Type)
      {
      case type::eUniform:
        BufferBindType = GL_UNIFORM_BUFFER;
        AccessType = GL_STATIC_DRAW;
        break;
      case type::eStorageRW:
        BufferBindType = GL_SHADER_STORAGE_BUFFER;
        AccessType = GL_DYNAMIC_COPY;
        break;
      case type::eStorageWrite:
        BufferBindType = GL_SHADER_STORAGE_BUFFER;
        AccessType = GL_DYNAMIC_DRAW;
        break;
      case type::eStorageRead:
        BufferBindType = GL_SHADER_STORAGE_BUFFER;
        AccessType = GL_DYNAMIC_READ;
        break;
      case type::eStorageStatic:
        BufferBindType = GL_SHADER_STORAGE_BUFFER;
        AccessType = GL_STATIC_DRAW;
        break;
      case type::eVertex:
        BufferBindType = GL_ARRAY_BUFFER;
        AccessType = GL_STATIC_DRAW;
        break;
      case type::eIndex:
        BufferBindType = GL_ELEMENT_ARRAY_BUFFER;
        AccessType = GL_STATIC_DRAW;
        break;

      default:
        std::unreachable();
      }

      glBindBuffer(BufferBindType, OpenGLId);
      glBufferData(BufferBindType, Size, Data, AccessType);
    } /* End of 'InitBase' function */

  public:
    /* Constructor from only size.
     * ARGUMENTS:
     *   - Size:
     *       size_t Size;
     *   - Type:
     *       type Type;
     */
    buffer( size_t Size, type Type ) :
      Size {Size}, Type {Type}
    {
      InitBase();
    } /* End of constructor */

    /* Constructor from initial data span.
     * ARGUMENTS:
     *   - Type:
     *       type Type;
     *   - Data span:
     *       std::span<const uint8_t> Data;
     */
    buffer( type Type, std::span<const uint8_t> Data ) :
      Size {Data.size()}, Type {Type}
    {
      InitBase(Data.data());
    } /* End of constructor */

    /* Data upload function
     * ARGUMENTS:
     *   - Data span:
     *       std::span<const uint8_t> Data;
     *   - Offset (default: 0):
     *       size_t Offset;
     * RETURNS: None.
     */
    void Upload( std::span<const uint8_t> Data, size_t Offset = 0 )
    {
      glBindBuffer(BufferBindType, OpenGLId);
      glBufferSubData(BufferBindType, Offset, Data.size(), Data.data());
    } /* End of 'Upload' function */

    /* Data readback function
     * ARGUMENTS:
     *   - Data write span:
     *       std::span<uint8_t> Data;
     *   - Offset (default: 0):
     *       size_t Offset;
     * RETURNS: None.
     */
    void Readback( std::span<uint8_t> Data, size_t Offset = 0 )
    {
      glBindBuffer(BufferBindType, OpenGLId);
      glGetBufferSubData(BufferBindType, Offset, Data.size(), Data.data());
    } /* End of 'Readback' function */
  }; /* end of 'buffer' class */

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

    /* Core getting function
     * ARGUMENTS: None.
     * RETURNS:
     *   (core &) Core reference.
     */
    constexpr shaders_manager &GetShadersManager( void ) noexcept
    {
      return Shaders;
    } /* End of 'GetShadersManager' function */

  public:
    /* Rendering primitive */
    class primitive abstract
    {
    public:
      /* Type enumerable */
      enum class type
      {
        eTransparent, // Main type for this project - transparency approximation

        _LastValue
      }; /* end of 'type' enumerable */

    private:
      /* Initialization data */
      render &Parent;
      type Type;

    public:
      /* Class constructor from parent render instance
       * ARGUMENTS:
       *   - Parent reference:
       *       render &Parent;
       *   - Primitive type:
       *       type Type;
       */
      primitive( render &Parent, type Type ) :
        Parent {Parent}, Type {Type}
      {
        Parent.PrimitivesPools[(size_t)Type].emplace(this);
      } /* End of constructor */

      /* Primitive render function
       * ARGUMENTS: None.
       * RETURNS: None.
       */
      virtual void Render( void ) = 0;

      /* Destructor */
      ~primitive( void )
      {
        Parent.PrimitivesPools[(size_t)Type].erase(Parent.PrimitivesPools[(size_t)Type].find(this));
      } /* End of destructor */
    }; /* end of 'primitive' class */

  private:
    /* Primitives pool storage */
    friend primitive;
    std::array<std::set<primitive *>, (size_t)primitive::type::_LastValue>
      PrimitivesPools {};

  private:
    /* Render frames */
    

  public:
    /* Resize responce
     * ARGUMENTS:
     *   - New size:
     *       uint32_t W, H;
     * RETURNS:
     *   (bool) Render availability flag.
     */
    bool Resize( uint32_t W, uint32_t H )
    {
      return W != 0 && H != 0;
    } /* End of 'Resize' function */

  public:
    /* Frame rendering function
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    void Render( void )
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      for (auto &Prim : PrimitivesPools[(size_t)primitive::type::eTransparent])
        Prim->Render();

      Core.Present();
    } /* End of 'Render' function */
  }; /* end of 'render' class */
} /* end of 'anim::rnd' namespace */

#endif /* __render_hpp__ */
