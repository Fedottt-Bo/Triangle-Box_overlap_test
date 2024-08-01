/* "win.hpp" - Win32 windows handling file */

#include <def.h>

#include <Windows.h>
#undef CreateWindow

/* Animation namespace // windows related namespace */
namespace anim::win
{
  /* Win32 API based window handler */
  class win
  {
  private:
    /* Base handles */
    HINSTANCE hInst {GetModuleHandleW(nullptr)};
    std::atomic_bool WasInit {};
    HWND hWnd;

    /* Shared windows' data handling class */
    class win_base
    {
    public:
      /* Window class name - Random GUID, generated by VS */
      inline static constexpr WCHAR ClassName[] {L"9DF0869C-0EDD-4033-A594-DD829A393ECF"};

      /* Window creation message name - Random GUID, generated by VS */
      inline static constexpr WCHAR WindowCreateMsgName[] {L"0668F3D4-CA76-4220-8C2E-5E03FC793C66"};

    private:
      /* Thread id */
      std::atomic<DWORD> ThreadId {0};

      /* Window creation message */
      std::atomic<UINT> WindowCreationMessage {0};

      /* Thread */
      std::jthread Thread;

      /* Window creation information structure */
      struct window_create_info
      {
        /* Inner workaround */
        HWND hWnd {};                   // Result field
        std::binary_semaphore Wait {0}; // Waiting primitive

        /* New window information */
        std::wstring_view WindowName;   // Window name
      }; /* end of 'create_info' structure */

      /* Window class callback function
       * ARGUMENTS:
       *  - Window handle:
       *     HWND hWnd;
       *  - Message code:
       *     UINT Msg;
       *  - Additional message information:
       *     WPARAM wParam;
       *  - Additional message information:
       *     LPARAM lParam;
       * RETURNS:
       *   (LRESULT) Result code.
       */
      static LRESULT CALLBACK WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
      {
        win *This {};

        switch (Msg)
        {
        case WM_GETMINMAXINFO:
            ((MINMAXINFO *)lParam)->ptMaxTrackSize.y =
            GetSystemMetrics(SM_CYMAXTRACK) +
            GetSystemMetrics(SM_CYCAPTION) +
            GetSystemMetrics(SM_CYMENU) +
            GetSystemMetrics(SM_CYBORDER) * 2;
          return 0;
        case WM_CREATE:
          This = reinterpret_cast<win *>(((CREATESTRUCT *)lParam)->lpCreateParams);
          This->hWnd = hWnd;
          SetWindowLongPtrW(hWnd, 0, (UINT_PTR)((CREATESTRUCT *)lParam)->lpCreateParams);
          [[fallthrough]];
        default:
          This = reinterpret_cast<win *>(GetWindowLongPtrW(hWnd, 0));
          if (This != nullptr)
            switch (Msg)
            {
            case WM_CREATE:
              This->WasInit.store(true);
              return 0;
            case WM_SIZE:
              break; //return 0;
            case WM_ERASEBKGND:
              return 1;
            case WM_PAINT:
            {
              PAINTSTRUCT PS;
              HDC hDC {BeginPaint(hWnd, &PS)};
              EndPaint(hWnd, &PS);
            }
              return 0;
            case WM_TIMER:
              return 0;
            case WM_MOUSEWHEEL:
              return 0;
            case WM_CLOSE:
              [[fallthrough]];
            case WM_DESTROY:
              SetWindowLongPtrW(hWnd, 0, 0);
              This->WasInit.store(false);
              This->WasInit.notify_all();
              break;
            case WM_QUIT:
              break;
            }
        }

        return DefWindowProcW(hWnd, Msg, wParam, lParam);
      } /* End of 'WndProc' function */

      /* Thread function
       * ARUGMENTS:
       *   - this pointer:
       *       message_loop *This;
       * RETURNS:
       *   (int) exit code.
       */
      static int ThreadFunc( win_base *This )
      {
        /* Create message queue */
        {
          MSG Msg;
          PeekMessageW(&Msg, nullptr, 0, 0, PM_NOREMOVE);
        }

        This->ThreadId.store(GetCurrentThreadId());
        This->ThreadId.notify_all();

        while (TRUE)
          if (MSG Msg; PeekMessageW(&Msg, nullptr, 0, 0, PM_REMOVE))
          {
            /* Stop loop on quit message */
            if (Msg.message == WM_QUIT)
              break;
            /* Process window creation */
            else if (Msg.message == This->WindowCreationMessage)
            {
              auto &Info {*(window_create_info *)(void *)Msg.lParam};

              Info.hWnd = CreateWindowExW(
                0,
                win_base::ClassName, Info.WindowName.data(),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                nullptr, nullptr, GetModuleHandleW(nullptr), This
              );

              Info.Wait.release();
            }
            /* Dispatch message */
            else
            {
              TranslateMessage(&Msg);
              DispatchMessageW(&Msg);
            }
          }

        return 0;
      } /* End of 'ThreadFunc' function */

      /* Class constructor */
      win_base( void ) :
        Thread {ThreadFunc, this}
      {
        /* Register window class */
        {
          WNDCLASSEXW Info
          {
            .cbSize = sizeof WNDCLASSEXW,
            .style = /* CS_DBLCLKS | */CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = WndProc,
            .cbClsExtra = 0,
            .cbWndExtra = sizeof (win *),
            .hInstance = GetModuleHandleW(nullptr),
            .hIcon = LoadIconW(nullptr, IDI_APPLICATION),
            .hCursor = LoadCursorW(nullptr, IDC_ARROW),
            .hbrBackground = (HBRUSH)COLOR_WINDOW,
            .lpszMenuName = nullptr,
            .lpszClassName = ClassName
          };

          if (RegisterClassExW(&Info) == 0)
            throw std::runtime_error {"Failed to register window class"};
        }

        /* Register window creation message */
        WindowCreationMessage = RegisterWindowMessageW(WindowCreateMsgName);
      } /* End of constructor */

    public:
      /* Class destructor */
      ~win_base( void )
      {
        ThreadId.wait(0);
        PostThreadMessageW(ThreadId, WM_QUIT, 0, 0);
        Thread.join();
      } /* End of destructor */

      /* Singleton getter
       * ARGUMENTS: None.
       * RETURNS:
       *   (message_loop &) Class reference.
       */
      static win_base &Get( void )
      {
        static win_base Instance {};
        Instance.ThreadId.wait(0);
        return Instance;
      } /* End of 'Get' function */

      /* Window creation
       * ARGUMENTS:
       *   std::wstring_view WindowName;
       * RETURNS:
       *   (HWND) Window handle.
       */
      HWND CreateWindow( std::wstring_view WindowName )
      {
        window_create_info Info {};
        Info.WindowName = WindowName;

        if (PostThreadMessageW(ThreadId, WindowCreationMessage, 0, (LPARAM)&Info) == 0)
          return nullptr;

        Info.Wait.acquire();

        return Info.hWnd;
      } /* End of 'CreateWindow' function */
    }; /* end of 'win_base' class */

  public:
    /* Constroctor
     * ARGUMENTS:
     *   - Name of created window:
     *       std::string_view WindowName
     */
    win( const std::wstring_view WindowName )
    {
      hWnd = win_base::Get().CreateWindow(WindowName);

      if (hWnd == nullptr)
        throw std::runtime_error {"Failed to create window"};

      ShowWindow(hWnd, SW_SHOWNORMAL);
    } /* End of constructor */

    /* Destructor */
    ~win( void )
    {
      DestroyWindow(hWnd);
      WasInit.wait(true);
      hWnd = nullptr;
    } /* End of destructor */
  }; /* end of 'win' class */
} /* end of 'anim::win' namespace */

/* END OF 'win.hpp' FILE */
