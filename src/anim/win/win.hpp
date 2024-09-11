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

/* "win.hpp" - Win32 windows handling file */

#ifndef __win_hpp__
#define __win_hpp__

#include <def.h>

#include <deque>

#include "win_events.hpp"

#include "win_def.h"

/* Animation namespace // windows related namespace */
namespace anim::win
{
  /* Window class */
  class window
  {
    friend class shared_base;

  private:
    /* Initialization information */
    std::atomic_bool WasInit {false};

    /* Base handles */
    HINSTANCE hInst {GetModuleHandleW(nullptr)};
    HWND hWnd {nullptr};

  public:
    /* Constructor
     * ARGUMENTS:
     *   - Name of created window:
     *       std::wstring_view WindowName;
     */
    window( std::wstring_view WindowName );

    /* Destructor */
    ~window( void );

  private:
    events_queue EventsQueue {};
    window_state State {};

  public:
    /* Event processing function.
     * ARGUMENTS:
     *   - Callback. Takes event from std::visit and window state:
     *       callable &&Callback;
     *   - Waiting flag (default: false):
     *       bool Wait;
     * RETURNS:
     *   (bool) Event existence flag.
     */
    template<class callable>
      bool GetEvent( callable &&Callback, bool Wait = false )
      {
        /* Read event or return as empty */
        events::any Event {std::nullopt};

        if (Wait)
        {
          if (!EventsQueue.PopWait(Event))
            return false;
        }
        else
        {
          if (!EventsQueue.Pop(Event))
            return false;
        }

        return std::visit(
          [this, Callback]( auto &Event ) -> bool
          {
            using event_type = std::remove_cvref_t<decltype(Event)>;

            if constexpr (std::same_as<event_type, std::nullopt_t>)
              return false;

            if constexpr (std::same_as<event_type, events::mouse>)
            {
              Event.MouseDX = Event.MouseNewX - State.MouseX;
              Event.MouseDY = Event.MouseNewY - State.MouseY;

              if (Event.MouseDX == 0.f && Event.MouseDY == 0.f && Event.MouseDZ == 0.f)
                return false;
            }

            Callback(Event, State);

            if constexpr (requires( window_state State, event_type Event ){ State.Update(Event); })
              State.Update(Event);

            return true;
          },
          Event
        );
      } /* End of 'GetEvent' function */

    /* Handles getting function
     * ARGUMENTS: None.
     * RETURNS:
     *   (std::pair<HINSTANCE, HWND>) Handles.
     */
    std::pair<HINSTANCE, HWND> GetHandles( void ) const noexcept
    {
      return {hInst, hWnd};
    } /* End of 'GetHandles' function */
  }; /* end of 'window' class */

  /* Shared windows' data handling class */
  class shared_base
  {
    /* Window class name - Random GUID, generated by VS */
    inline static constexpr WCHAR ClassName[] {L"9DF0869C-0EDD-4033-A594-DD829A393ECF"};

    /* Custom messages' names - Random GUIDs, generated by VS */
    inline static constexpr WCHAR WindowCreateMsgName[] {L"0668F3D4-CA76-4220-8C2E-5E03FC793C66"};

  private:
    /* State maintaining data */
    std::atomic_size_t UsageCnt {0}; // Global windows counter
    std::atomic_bool Exists {false}; // Existance flag

    /* Custom messages' ids */
    UINT WindowCreationMessage {0};

    /* Thread data */
    std::atomic<DWORD> ThreadId {0};
    std::thread MessageLoopThread {};

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
      window *Win {};

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
        Win = reinterpret_cast<window *>(((CREATESTRUCT *)lParam)->lpCreateParams);
        Win->hWnd = hWnd;
        SetWindowLongPtrW(hWnd, 0, (UINT_PTR)((CREATESTRUCT *)lParam)->lpCreateParams);
        [[fallthrough]];
      default:
        Win = reinterpret_cast<window *>(GetWindowLongPtrW(hWnd, 0));
        if (Win != nullptr)
          switch (Msg)
          {
          /* Creation event */
          case WM_CREATE:
            Win->WasInit.store(true);
            Win->WasInit.notify_one();
            return 0;

          /* Closing events */
          case WM_CLOSE:
            if (lParam != 30)
            {
              /* Send closing request */
              Win->EventsQueue.Push(events::close {});

              return 1;
            }

            [[fallthrough]];
          case WM_DESTROY:
            /* Inform about closing */
            Win->EventsQueue.Push(events::close {true});

            SetWindowLongPtrW(hWnd, 0, 0);
            Win->WasInit.store(false);
            Win->WasInit.notify_one();

            /* Quit if the last window was destroyed */
            if (Get().UsageCnt.fetch_sub(1) == 1)
              PostQuitMessage(0);

            break;
          case WM_QUIT:
            break;

          /* Visual events */
          case WM_SIZE:
            Win->EventsQueue.Push(events::resize {LOWORD(lParam), HIWORD(lParam)});
            return 0;
          case WM_ERASEBKGND:
            return 1;
          case WM_PAINT:
          {
            PAINTSTRUCT PS;
            HDC hDC {BeginPaint(hWnd, &PS)};
            EndPaint(hWnd, &PS);
          }
            return 0;

          /* Utility events */
          case WM_TIMER:
            return 0;

          /* Input events */
          case WM_MOUSEMOVE:
          {
            POINT Coord {((short)LOWORD(lParam)), ((short)HIWORD(lParam))};

            events::mouse Event
            {
              .MouseNewX = (float)Coord.x,
              .MouseNewY = (float)Coord.y
            };

            Win->EventsQueue.Push(std::move(Event));
          }
            return 0;
          case WM_LBUTTONUP:
          case WM_RBUTTONUP:
          case WM_MBUTTONUP:
            switch (Msg)
            {
            case WM_LBUTTONUP:
              wParam = VK_LBUTTON;
              break;
            case WM_RBUTTONUP:
              wParam = VK_RBUTTON;
              break;
            case WM_MBUTTONUP:
              wParam = VK_MBUTTON;
              break;
            }

            lParam = 0;

            [[fallthrough]];
          case WM_KEYUP:
          case WM_SYSKEYUP:
            /* Check key is in range */
            if (wParam <= (uint8_t)key::_LastValue)
            {
              events::keyboard Event {(key)wParam, false};

              Win->EventsQueue.Push(std::move(Event));
            }

            return 0;
          case WM_LBUTTONDOWN:
          case WM_RBUTTONDOWN:
          case WM_MBUTTONDOWN:
            switch (Msg)
            {
            case WM_LBUTTONDOWN:
              wParam = VK_LBUTTON;
              break;
            case WM_RBUTTONDOWN:
              wParam = VK_RBUTTON;
              break;
            case WM_MBUTTONDOWN:
              wParam = VK_MBUTTON;
              break;
            }

            lParam = 0;

            [[fallthrough]];
          case WM_KEYDOWN:
          case WM_SYSKEYDOWN:
            /* Check key is in range */
            if (wParam <= (uint8_t)key::_LastValue)
            {
              events::keyboard Event {(key)wParam, false};

              /* Lock focus if it's a mouse button */
              if (Event.Key >= key::e_MouseMin &&
                  Event.Key <= key::e_MouseMax)
                SetFocus(hWnd);

              /* Prevent key autorepeat */
              if (!(lParam & ((LPARAM)1u << 30)))
              {
                /* Process Alt+F4 */
                if (Msg == WM_SYSKEYDOWN && Event.Key == key::eF4)
                  SendMessageW(hWnd, WM_CLOSE, 0, 0);

                Win->EventsQueue.Push(std::move(Event));
              }
            }

            return 0;
          case WM_MOUSEWHEEL:
          {
            /* Transform coordinates to local */
            POINT Coord {((short)LOWORD(lParam)), ((short)HIWORD(lParam))};

            ScreenToClient(hWnd, &Coord);

            events::mouse Event
            {
              .MouseNewX = (float)Coord.x,
              .MouseNewY = (float)Coord.y,
              .MouseDZ = (float)((short)HIWORD(wParam)) * (1.f / (float)WHEEL_DELTA)
            };

            Win->EventsQueue.Push(std::move(Event));
          }
            return 0;
          }
      }

      return DefWindowProcW(hWnd, Msg, wParam, lParam);
    } /* End of 'WndProc' function */

    /* Window creation information structure */
    struct window_create_info
    {
      /* Inner workaround */
      HWND hWnd {};                   // Result field
      std::binary_semaphore Wait {0}; // Waiting primitive

      /* New window information */
      window *Owner;                  // Owner object pointer
      std::wstring_view WindowName;   // Window name
    }; /* end of 'create_info' structure */

    /* Thread function
     * ARUGMENTS:
     *   - this pointer:
     *       message_loop *This;
     * RETURNS:
     *   (int) exit code.
     */
    static int ThreadFunc( shared_base *This )
    {
      /* Create message queue */
      {
        MSG Msg;
        PeekMessageW(&Msg, nullptr, 0, 0, PM_NOREMOVE);
      }

      This->ThreadId.store(GetCurrentThreadId());
      This->ThreadId.notify_all();

      while (true)
        if (MSG Msg; GetMessageW(&Msg, nullptr, 0, 0))
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
              ClassName, Info.WindowName.data(),
              WS_OVERLAPPEDWINDOW,
              CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
              nullptr, nullptr, GetModuleHandleW(nullptr), Info.Owner
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

    /* Thread creation function
     * ARGUMENTS: None.
     * RETURNS: None.
     */
    void CreateThread( void )
    {
      ThreadId.store(0);

      std::thread Tmp {ThreadFunc, this};

      MessageLoopThread.swap(Tmp);

      if (Tmp.joinable())
        Tmp.join();

      ThreadId.wait(0);
    } /* End of 'CreateThread' function */

    /* Private constructor */
    shared_base( void )
    {
      /* Register window class */
      {
        WNDCLASSEXW Info
        {
          .cbSize = sizeof WNDCLASSEXW,
          .style = /* CS_DBLCLKS | */CS_HREDRAW | CS_VREDRAW,
          .lpfnWndProc = WndProc,
          .cbClsExtra = 0,
          .cbWndExtra = sizeof (window *),
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

    /* Destructor */
    ~shared_base( void )
    {
      if (MessageLoopThread.joinable())
        MessageLoopThread.detach();
    } /* End of destructor */

  public:
    /* Getting function */
    static shared_base &Get( void )
    {
      static shared_base Instance {};
      return Instance;
    } /* End of 'Get' function */

    /* Window creation
     * ARGUMENTS:
     *   - Owner pointer:
     *       window *Owner;
     *   - Window name:
     *       std::wstring_view WindowName;
     * RETURNS:
     *   (HWND) Window handle.
     */
    HWND CreateWindow( window *Owner, std::wstring_view WindowName )
    {
      /* Create thread if this is first window */
      if (UsageCnt.fetch_add(1) == 0)
      {
        CreateThread();

        Exists.store(true);
        Exists.notify_all();
      }
      /* Or wait to guarantee created thread */
      else
        Exists.wait(false);

      window_create_info Info {};
      Info.Owner = Owner;
      Info.WindowName = WindowName;

      if (PostThreadMessageW(ThreadId, WindowCreationMessage, 0, (LPARAM)&Info) == 0)
        return nullptr;

      Info.Wait.acquire();

      return Info.hWnd;
    } /* End of 'CreateWindow' function */

    /* Window destruction function
     * ARGUMENTS:
     *   - Window handle:
     *       HWND hWnd;
     * RETURNS: None.
     */
    void DestroyWindow( HWND hWnd )
    {
      PostMessageW(hWnd, WM_CLOSE, 0, 30);
    } /* End of 'DestroyWindow' function */
  }; /* end of 'shared_base' class */

  /* Constructor
   * ARGUMENTS:
   *   - Name of created window:
   *       std::string_view WindowName
   */
  inline window::window( const std::wstring_view WindowName )
  {
    hWnd = shared_base::Get().CreateWindow(this, WindowName);

    ShowWindow(hWnd, SW_SHOWNORMAL);
  } /* End of constructor */

  /* Destructor */
  inline window::~window( void )
  {
    shared_base::Get().DestroyWindow(hWnd);
    WasInit.wait(true);
    hWnd = nullptr;
  } /* End of destructor */
} /* end of 'anim::win' namespace */

#endif /* __win_hpp__ */

/* END OF 'win.hpp' FILE */
